#!/bin/bash	
	(
		which g++
		which g++-12
		which g++-13
		which g++-14
		which g++-15
		which clang++
		which clang++-17
		which clang++-18
		which clang++-19
	) | compilers.pl compilers

