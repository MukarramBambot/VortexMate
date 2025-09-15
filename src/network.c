#include "network.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
    #define CLOSESOCK closesocket
    static bool net_winsock_init = false;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <arpa/inet.h>
    #define CLOSESOCK close
#endif

#define NETBUF_SIZE 128

void net_init(NetContext* ctx) {
    memset(ctx, 0, sizeof(NetContext));
#ifdef _WIN32
    if (!net_winsock_init) {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
            printf("WSAStartup failed\n");
        } else {
            net_winsock_init = true;
        }
    }
#endif
    ctx->socket_fd = -1;
    ctx->client_fd = -1;
    ctx->state = NET_STATE_IDLE;
    ctx->is_my_turn = false;
    ctx->is_host = false;
    ctx->status_msg[0] = 0;
}

void net_cleanup(NetContext* ctx) {
    if (ctx->client_fd > 0) { CLOSESOCK(ctx->client_fd); ctx->client_fd = -1; }
    if (ctx->socket_fd > 0) { CLOSESOCK(ctx->socket_fd); ctx->socket_fd = -1; }
    ctx->state = NET_STATE_IDLE;
    ctx->status_msg[0] = 0;
#ifdef _WIN32
    // Don't call WSACleanup here (can be called on process exit)
#endif
}

bool start_server(NetContext* ctx, int port) {
    net_cleanup(ctx);
    ctx->mode = NET_SERVER;
    ctx->port = port;
    ctx->is_host = true;
    ctx->is_my_turn = true; // Host is White, moves first
    ctx->client_fd = -1;
    ctx->status_msg[0] = 0;
    // Create socket
    int s = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { snprintf(ctx->status_msg,64,"Socket error"); return false; }
    // Non-blocking
#ifdef _WIN32
    u_long mode = 1; ioctlsocket(s, FIONBIO, &mode);
#else
    fcntl(s, F_SETFL, O_NONBLOCK);
#endif
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        snprintf(ctx->status_msg, 64, "Bind failed");
        CLOSESOCK(s); return false;
    }
    if (listen(s, 1) < 0) {
        snprintf(ctx->status_msg, 64, "Listen failed");
        CLOSESOCK(s); return false;
    }
    ctx->socket_fd = s;
    ctx->state = NET_STATE_LISTENING;
    snprintf(ctx->status_msg, 64, "Waiting for opponent on port %d...", port);
    printf("Server listening on port %d\n", port);
    return true;
}

bool connect_to_server(NetContext* ctx, const char* ip, int port) {
    net_cleanup(ctx);
    ctx->mode = NET_CLIENT;
    ctx->port = port;
    strncpy(ctx->ip, ip, sizeof(ctx->ip)-1);
    ctx->is_host = false;
    ctx->is_my_turn = false; // Client is Black, moves second
    // Create socket
    int s = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { snprintf(ctx->status_msg,64,"Socket error"); return false; }
    // Non-blocking
#ifdef _WIN32
    u_long mode = 1; ioctlsocket(s, FIONBIO, &mode);
#else
    fcntl(s, F_SETFL, O_NONBLOCK);
#endif
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    int res = connect(s, (struct sockaddr*)&addr, sizeof(addr));
#ifdef _WIN32
    if (res == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK && WSAGetLastError() != WSAEINPROGRESS) {
        snprintf(ctx->status_msg,64,"Connect failed");
        CLOSESOCK(s); return false;
    }
#else
    if (res < 0 && errno != EINPROGRESS) {
        snprintf(ctx->status_msg,64,"Connect failed");
        CLOSESOCK(s); return false;
    }
#endif
    ctx->socket_fd = s;
    ctx->state = NET_STATE_CONNECTING;
    snprintf(ctx->status_msg,64,"Connecting to %s:%d...", ip, port);
    printf("Connecting to %s:%d...\n", ip, port);
    return true;
}

// Returns true if connected
bool net_is_connected(NetContext* ctx) {
    return ctx->state == NET_STATE_CONNECTED;
}

// Non-blocking socket polling for connection/accept/move
void net_poll(NetContext* ctx) {
    if (ctx->mode == NET_SERVER && ctx->state == NET_STATE_LISTENING) {
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);
        int cfd = (int)accept(ctx->socket_fd, (struct sockaddr*)&cliaddr, &len);
        if (cfd > 0) {
#ifdef _WIN32
            u_long mode = 1; ioctlsocket(cfd, FIONBIO, &mode);
#else
            fcntl(cfd, F_SETFL, O_NONBLOCK);
#endif
            ctx->client_fd = cfd;
            ctx->state = NET_STATE_CONNECTED;
            snprintf(ctx->status_msg,64,"Connected to client!");
            printf("Client connected!\n");
        }
    }
    else if (ctx->mode == NET_CLIENT && ctx->state == NET_STATE_CONNECTING) {
        fd_set wfds;
        FD_ZERO(&wfds); FD_SET(ctx->socket_fd, &wfds);
        struct timeval tv = {0};
        int res = select(ctx->socket_fd + 1, NULL, &wfds, NULL, &tv);
        if (res > 0 && FD_ISSET(ctx->socket_fd, &wfds)) {
            ctx->state = NET_STATE_CONNECTED;
            snprintf(ctx->status_msg,64,"Connected to server!");
            printf("Connected to server!\n");
        }
    }
}

static int get_active_fd(NetContext* ctx) {
    if (ctx->mode == NET_SERVER && ctx->client_fd > 0) return ctx->client_fd;
    if (ctx->mode == NET_CLIENT && ctx->socket_fd > 0) return ctx->socket_fd;
    return -1;
}

// Send move as a line (e.g., "e2e4\n")
bool send_move(NetContext* ctx, const char* move_str) {
    int fd = get_active_fd(ctx);
    if (fd <= 0) return false;
    char buf[NETBUF_SIZE];
    snprintf(buf, sizeof(buf), "%s\n", move_str);
    int tosend = (int)strlen(buf);
#ifdef _WIN32
    int sent = send(fd, buf, tosend, 0);
#else
    int sent = (int)write(fd, buf, tosend);
#endif
    return (sent == tosend);
}

// Non-blocking receive; fills move_buf with move string (no trailing \n)
// Returns true if move received, false otherwise
bool receive_move(NetContext* ctx, char* move_buf, int bufsize) {
    int fd = get_active_fd(ctx);
    if (fd <= 0) return false;
    char buf[NETBUF_SIZE] = {0};
#ifdef _WIN32
    int recvd = recv(fd, buf, NETBUF_SIZE-1, 0);
#else
    int recvd = (int)read(fd, buf, NETBUF_SIZE-1);
#endif
    if (recvd <= 0) return false;
    // Look for a line ending
    char* p = strchr(buf, '\n');
    if (!p) return false;
    *p = 0;
    strncpy(move_buf, buf, bufsize-1);
    move_buf[bufsize-1] = 0;
    return true;
}

void net_disconnect(NetContext* ctx) {
    net_cleanup(ctx);
    ctx->mode = NET_NONE;
    ctx->state = NET_STATE_IDLE;
    snprintf(ctx->status_msg,64,"Disconnected.");
}