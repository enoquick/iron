#!/usr/bin/perl 
use strict;
use warnings; 

sub std {
	local $_=shift;
	/\/clang\+\+/ && return "-std=c++23";
	/\/g\+\+/ && return "-std=c++23";
	return undef;
}

sub get_std {
	local $_=shift;
	/\-std=c\+\+(\d+)/ && return $1;
	return undef;
}

sub setcpp {
	local $_=shift;
	/\/clang\+\+/ && return "-x c++";
	/\/g\+\+/ && return "-x c++";
	return undef;
}

sub arch {
	local $_=shift;
	/\/clang\+\+/ && return ("-m64","-m32");
	/\/g\+\+/ && return ("-m64","-m32");
	return ();
}

sub get_arch {
	local $_=shift;
	/\-m(\d+)/ && return $1;
	return undef;
}

sub compiler_family {
	local $_=shift;
	/\/clang\+\+/ && return "clang++";
	/\/g\+\+/ && return "g++";
	return undef;
}	

sub base_compiler {
	local $_=shift;
	/^(\S+)/ && return $1;
	return undef; 
}

sub version_number {
	local $_=shift;
	if (/\/clang\+\+/) {
		my $out=`$_ --version`;
		return $1 if $out=~/(\d+\.\d+\.\d+)/;
	}
	elsif (/\/g\+\+/) {
		my $out=`$_ --version`;
		return $1 if $out=~/(\d+\.\d+\.\d+)/;
	}
	return undef;
}
sub version_key {
	local $_=shift;
	if (/\/clang\+\+/) {
		my $out=`$_ --version`;
		return "1clang++-$1" if $out=~/(\d+\.\d+\.\d+)/;
	}
	elsif (/\/g\+\+/) {
		my $out=`$_ --version`;
		return "0g++-$1" if $out=~/(\d+\.\d+\.\d+)/;
	}
	return undef;
}

sub test_compiler {
	my ($compiler,$std,$setcpp,$arch)=@_;
	my $cmd="(echo '#include <iostream>'; echo 'int main(){}' ) | $compiler $std $setcpp $arch - 2> /dev/null";
	if (system($cmd) == 0) {
		system("rm a.out");
		return 1;
	}
	return 0;
}

sub exclude_compiler {
	my ($c,$a)=@_;
	for my $e(@$a) {
		if ($c=~/\Q$e/) {
			return 1;
		}
	}
	return 0;
}

sub compilers {
	my %uniq=();
	my @exclude_compilers=();
	my $ec=$ENV{EXCLUDE_COMPILERS};
	if (defined $ec) {
		@exclude_compilers=split(/\s+/,$ec);
	}
	while(<STDIN>) {
		chomp;
		next if !length;
		my $compiler=$_;
		next if exclude_compiler($compiler,\@exclude_compilers);
		my $version=version_key $compiler;
		if (defined($version) && !exists $uniq{$version}) {
			#print "version $version\n";
			my $setcpp=setcpp $compiler;
			my $std=std $compiler;
			if (defined($setcpp) && defined($std)) {
				my @archs=arch $compiler;
				for my $arch(@archs) {
					if (test_compiler $compiler,$std,$setcpp,$arch) {
						my $key="$version $arch";
						$uniq{$key}="$compiler $std $arch";
					}
				}
			}
		}
	}
	for my $k(sort keys %uniq) {
	#	print "key $k\n";
		print $uniq{$k}."\n";
	}
	return 0;
}

#
#  main
#

if (scalar(@ARGV) == 0) {
	printf STDERR "missing command\n";
	exit 1;
}

my $cmd=$ARGV[0];

if ($cmd eq "compilers") { 
	compilers;
	exit 0;
}
if ($cmd eq "compiler_family") {
	my $compiler=$ARGV[1];
	if (!defined $compiler) {
		printf STDERR "missing arg\n";
		exit 1;
	}
	my $r=compiler_family $compiler;
	if (defined $r) {
		print $r;
		exit 0;
	}
	printf STDERR "%s\n","$compiler: unknow compiler family";
	exit 1;
}

if ($cmd eq "arch") {
	my $compiler=$ARGV[1];
	if (!defined $compiler) {
		printf STDERR "missing arg\n";
		exit 1;
	}
	my $r=get_arch $compiler;
	if (defined $r) {
		print $r;
		exit 0;
	}
	printf STDERR "%s\n","$compiler: unknow arch";
	exit 1;
}

if ($cmd eq "std") {
	my $compiler=$ARGV[1];
	if (!defined $compiler) {
		printf STDERR "missing arg\n";
		exit 1;
	}
	my $r=get_std $compiler;
	if (defined $r) {
		print $r;
		exit 0;
	}
	printf STDERR "%s\n","$compiler: unknow std";
	exit 1;
}	

if ($cmd eq "base_compiler") {
	my $compiler=$ARGV[1];
	if (!defined $compiler) {
		printf STDERR "missing arg\n";
		exit 1;
	}
	my $r=base_compiler $compiler;
	if (defined $r) {
		print $r;
		exit 0;
	}
	printf STDERR "%s\n","$compiler: unknow base_compiler";
	exit 1;
}	

if ($cmd eq "version_number") {
	my $compiler=$ARGV[1];
	if (!defined $compiler) {
		printf STDERR "missing arg\n";
		exit 1;
	}
	my $r=version_number $compiler;
	if (defined $r) {
		print $r;
		exit 0;
	}
	printf STDERR "%s\n","$compiler: unknow base_compiler";
	exit 1;
	
}

printf STDERR "%s\n","$cmd: unknow command";
exit 1;

