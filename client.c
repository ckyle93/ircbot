/* By: Chris Kyle
 * This program is based off the programming challenge
 * Challenge #259 [Medium] IRC: Making a Connection
 * www.reddit.com/r/dailyprogrammer/comments/4ad23z/20160314_challenge_258_easy_irc_making_a/
*/

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

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];
size_t inbuf_used = 0;

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void send_nickname(const char *nick, int sock) {
    bzero(buffer,BUFFER_SIZE);
    snprintf(buffer, sizeof buffer, "NICK %s\r\n", nick);
    printf("%s", buffer);
    int n = write(sock,buffer,strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");
}

void send_username(const char *user_name, const char *real_name, int sock) {
    bzero(buffer,BUFFER_SIZE);
    snprintf(buffer, sizeof buffer,
        "USER %s 0 * :%s\r\n", user_name, real_name);
    printf("%s", buffer);
    int n = write(sock,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
}

// Check for MOTD code 376 and PING check.
void process_line(char *line, int fd) {
    char *str = strdup(line);
    char *strcpy = str;
    assert(str != NULL);
    if (strcmp(strsep(&str, " "), "PING") == 0){
        char response[128];
        snprintf(response, sizeof response,
            "PONG %s\r\n", strsep(&str, " "));
        int n = write(fd, response, strlen(response));
        if (n < 0)
            error("ERROR writing to socket");
        printf("%s", response);
        free(strcpy);
        return;
    }
    if (strcmp(strsep(&str, " "), "376") == 0){
        char *join_message = "JOIN #reddit-dailyprogrammer,#botters-test\r\n";
        int n = write(fd, join_message, strlen(join_message)); 
        if (n < 0)
            error("ERROR writing to socket");
        printf("%s\n", join_message);
        free(strcpy);
        return;
    }
}

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
        process_line(line_start, fd);
        line_start = line_end + 1;
    }
    inbuf_used -= (line_start - buffer);
    memmove(buffer, line_start, inbuf_used);
}

int main(int argc, char *argv[]) {
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc != 2) {
        printf("Usage: client <filename>");
        exit(0);
    }
    FILE *address_file = fopen(argv[1], "r");

    // Get address of the form www.example.com:1234
    fgets(buffer, BUFFER_SIZE, address_file);
    char *str = strdup(buffer);
    char *web_address = strsep(&str, ":");
    size_t port_number = atoi(strsep(&str, ":"));

    // Get nickname and strip newlines.
    fgets(buffer, BUFFER_SIZE, address_file);
    char nickname[64];
    strtok(buffer, "\n");
    strcpy(nickname, buffer);

    // Get username and strip newlines.
    fgets(buffer, BUFFER_SIZE, address_file);
    char username[64];
    strtok(buffer, "\n");
    strcpy(username, buffer);

    // Get realname and strip newlines
    fgets(buffer, BUFFER_SIZE, address_file);
    char realname[64];
    strtok(buffer, "\n");
    strcpy(realname, buffer);
    fclose(address_file);

    // Connect to server
    // This is based off of the C socket tutorial at
    // http://www.linuxhowtos.org/C_C++/socket.htm
    portno = port_number;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(web_address);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    send_nickname(nickname, sockfd);
    send_username(username, realname, sockfd);
    // listen_to_server(sockfd);
    while(1) {
        read_lines(sockfd);
    }
    close(sockfd);
    return 0;
}
