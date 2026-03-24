#!/bin/bash

#set -x
single_test="$1"
valgrind="$2"

[ "$valgrind" == "valgrind" ] || valgrind="" 

if [ "$single_test" == "valgrind" ]
then
	single_test=""
	valgrind="valgrind"
fi

[ "$valgrind" == "valgrind" ] || valgrind="" 

. bash_colors.sh

function set_libpath {
	local rootdir="$1"
	local cc="$2" 
	local ver="$3"
	local arch="$4"
	local debug="$5"
	local dir=$rootdir/libs/build/${cc}_${ver}_${arch}
	[ "$debug" -eq 1 ] && dir="${dir}_d"
	[ -d "$dir" ] || { echo "$dir: is not a dir">&2; return 1;}
	export LD_LIBRARY_PATH="$dir";
	return 0
}
set +x
set -e
rootdir="$PWD"
cd iron 
export PATH="$rootdir:$PATH"
compilers.sh > compilers.txt

for d in *
do 
	[ -d "$d" ] || continue;
	echo "$d" | grep "_module" > /dev/null && continue #for now skip test modules
	[ "$d" = "unicode_no_modules" ] && continue #main.cpp are external dependencies for test - is skypped
	[ "$d" = "net_no_modules" ] && continue #temporary skip 
	if [ "$single_test" ]
	then
		echo "$d" | grep "$single_test" > /dev/null || continue #filter for single test
	fi
	(
		cd "$d" || exit 0 
		[ -r main.cpp ] || exit 0
		cat ../compilers.txt | while read 
		do
			compiler="$REPLY"
			cfamily=$(compilers.pl compiler_family "$compiler")
			arch=$(compilers.pl arch "$compiler") 
			std=$(compilers.pl std "$compiler")
			base_compiler=$(compilers.pl base_compiler "$compiler")
			compiler_version=$(compilers.pl version_number "$base_compiler")
			for debug in 1 0
			do
				dir="test_$(basename "$base_compiler")_${compiler_version}_${debug}_${arch}"
				mkdir -p "$dir" || exit 1
					(
						cd 	"$dir"	|| exit 1
#						echo "current dir $dir">&2
						[ "$debug" -eq 1 ] && label="DEBUG" || label="RELEASE"
						echo >&2
						echo -e "${GREEN}use compiler $compiler${NC}">&2
						echo -e "${GREEN}$base_compiler TEST $label $d - arch $arch ${NC}" >&2
						test_makefile="../../makefile_test"
						[ -r "$test_makefile" ] || { echo "$test_makefile: not readable">&2;exit 1;}
						CXX="$base_compiler" make -f $test_makefile rootdir="$rootdir" arch="$arch" debug="$debug" \
							std="$std" modules="0" compiler_family="$cfamily" compiler_version="$compiler_version" || exit 1
						set_libpath "$rootdir" "$cfamily" "$compiler_version" "$arch" "$debug" || exit 1
						echo "execute LD_LIBRARY_PATH=$LD_LIBRARY_PATH IRON_STACKTRACE=1  $valgrind $PWD/a.out  < /dev/null" >&2
						IRON_STACKTRACE=1  $valgrind ./a.out  < /dev/null || exit 1
					) || exit 1
			done
		done || exit 1
		exit 0
	)  || exit 1
done

rm -f compilers.txt
