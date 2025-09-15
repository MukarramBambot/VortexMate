#pragma once
#include <stdbool.h>

// Networking Modes
typedef enum {
    NET_NONE = 0,
    NET_SERVER,
    NET_CLIENT
} NetMode;

// Connection State
typedef enum {
    NET_STATE_IDLE = 0,
    NET_STATE_LISTENING,
    NET_STATE_CONNECTING,
    NET_STATE_CONNECTED,
    NET_STATE_DISCONNECTED
} NetState;

// Multiplayer game context
typedef struct {
    NetMode mode;
    NetState state;
    int socket_fd;       // Main socket (server or client)
    int client_fd;       // For server: accepted client
    int port;
    char ip[64];         // Remote IP (for client)
    bool is_my_turn;     // True if this player should move
    bool is_host;        // True if this side is host/white
    char last_move[16];  // Last move received from network ("e2e4")
    char status_msg[64]; // Connection status msg
} NetContext;

// --- Networking API ---
void net_init(NetContext* ctx);
void net_cleanup(NetContext* ctx);

// Server: start listening
bool start_server(NetContext* ctx, int port);

// Client: connect to server
bool connect_to_server(NetContext* ctx, const char* ip, int port);

// Returns true if connected
bool net_is_connected(NetContext* ctx);

// Send move (e.g., "e2e4\n")
bool send_move(NetContext* ctx, const char* move_str);

// Non-blocking receive: returns true if a move was received (fills move_buf)
bool receive_move(NetContext* ctx, char* move_buf, int bufsize);

// Polls sockets, updates state/status_msg
void net_poll(NetContext* ctx);

// Call to close sockets and reset context
void net_disconnect(NetContext* ctx);