#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];
size_t inbuf_used = 0;

// Based off of the example provided by bdonlan at
// http://stackoverflow.com/questions/6090594/c-recv-read-until-newline-occurs
void read_lines(int fd) {
    size_t buf_remain = sizeof(buffer) - inbuf_used;
    if (buf_remain == 0) {
        fprintf(stderr, "Line exceeded buffer length. \n");
    }

    ssize_t rv = read(fd, (void *)&buffer[inbuf_used], buf_remain);
    if (rv == 0) {
        exit(EXIT_SUCCESS);
    }
    if (rv < 0 && errno == EAGAIN) {
        // Socket has no data for us
        return;
    }
    if (rv < 0) {
       printf("Connection error");
    }
    inbuf_used += rv;

    char *line_start = buffer;
    char *line_end;
    while( (line_end = memchr((void*)line_start, '\n', inbuf_used - (line_start - buffer)))){
        *line_end = 0;
        printf("%s\n", line_start);
        line_start = line_end + 1;
    }
    inbuf_used -= (line_start - buffer);
    memmove(buffer, line_start, inbuf_used);
}

int main() {
    int fd = open("line_pos", O_RDONLY);
    while(1) {
        read_lines(fd);
    }
    close(fd);
}
