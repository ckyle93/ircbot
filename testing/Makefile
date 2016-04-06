all: build_test parse_file

build_test: build_test.c
	gcc -g -O0 -Wall -Wextra -std=gnu11 build_test.c -o build_test

parse_file: parse_file.c
	gcc -g -O0 -Wall -Wextra -std=gnu11 parse_file.c -o parse_file 

clean: client
	rm client
