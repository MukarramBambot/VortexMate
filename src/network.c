#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>          // Needed for errno and EINPROGRESS
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CLOSESOCK close

int connect_to_server(const char *ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;
    int res;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        CLOSESOCK(sockfd);
        return -1;
    }

    // Set socket to non-blocking if needed here...

    res = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (res < 0 && errno != EINPROGRESS) {
        perror("connect");
        CLOSESOCK(sockfd);
        return -1;
    }

    // Continue with your networking logic...

    return sockfd;
}