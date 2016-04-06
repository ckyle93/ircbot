all: client build_test


client: client.c
	gcc -g -O0 -Wall -Wextra -std=gnu11 client.c -o client

build_test: build_test.c
	gcc -g -O0 -Wall -Wextra -std=gnu11 build_test.c -o build_test

clean: client
	rm client
