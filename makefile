#use: make [m=<module>]

.PHONY : build_lib clean test

build_lib:
	./build_lib.sh
	
test:
	./tests.sh $(m) $(v)

install:
	./install.sh $(PREFIX)
clean :
	./clean.sh 
	
