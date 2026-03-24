#!/bin/bash
	
	. ./bash_colors.sh
	base_libname="libiron"
	root_dir="$PWD"
	mkdir -p $root_dir/libs || exit 1
	PATH="$root_dir:$PATH"

	compilers.sh | while read 
	do
		compiler="$REPLY"
		echo "use compiler $compiler">&2
		cfamily=$(compilers.pl compiler_family "$compiler")
		arch=$(compilers.pl arch "$compiler") 
		std=$(compilers.pl std "$compiler")
		base_compiler=$(compilers.pl base_compiler "$compiler")
		compiler_version=$(compilers.pl version_number "$base_compiler")
		for debug in 0 1
		do
			for static in 0 1
			do
				[ "$debug" -eq 1 ] && label="DEBUG" || label="RELEASE"
				echo >&2
				echo -e "${GREEN}$base_compiler build $label $d - arch $arch ${NC}" >&2
				CXX="$base_compiler" make  -f ../makefile_buildlib --directory=libs debug="$debug" static="$static" std="$std" \
						compiler_family="$cfamily" arch="$arch" base_libname="$base_libname" rootdir="$root_dir" compiler_version="$compiler_version" || exit 1
			done
		done
	done || exit 1
	exit 0
