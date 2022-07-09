#include "class_tcp.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static bool g_initialized = false;
static int g_server_socket;
static int g_client_socket;

Error Tcp_server_init()
{
    if (g_initialized)
    {
        LOG_ERROR("TCP server already initialized");
        return ERR_PERMISSION_DENIED;
    }

    const int domain   = AF_INET; // (= PF_INET) Internet domain sockets for use with IPv4 addresses
    const int type     = SOCK_STREAM; // bitstream socket used in TCP
    const int protocol = 0; // default type of socket that works with the other chosen params
    int on             = 1;

    // create a socket
    g_server_socket    = socket(domain, type, protocol);
    if (g_server_socket == -1)
    {
        LOG_PERROR("Failed to create socket");
        return ERR_TCP_INTERNAL;
    }

    if (setsockopt(g_server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        LOG_PERROR("Failed to set socket options");
        return ERR_TCP_INTERNAL;
    }
    // specify address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons(8080);
#ifdef __linux__
    // On RPi, we get traffic to/from the network.
    server_address.sin_addr.s_addr = INADDR_ANY;
#else
    // On MacOS, we are probably using localhost as server address.
    server_address.sin_addr.s_addr = htonl(0x7F000001 /*127.0.0.1*/);
#endif

    if (bind(g_server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
    {
        LOG_PERROR("Failed to bind to network socket");
        return ERR_TCP_INTERNAL;
    }

    // listen to connections
    if (listen(g_server_socket, SOMAXCONN) == -1)
    {
        LOG_PERROR("Failed to listen to socket");
        return ERR_TCP_INTERNAL;
    }
    g_initialized = true;
    return ERR_ALL_GOOD;
}

Error Tcp_accept()
{
    if (!g_initialized)
    {
        LOG_ERROR("TCP server not initialized yet.");
        return ERR_PERMISSION_DENIED;
    }
    struct sockaddr_in client;
    socklen_t client_size;

    g_client_socket = accept(g_server_socket, (struct sockaddr*)&client, &client_size);
    if (g_client_socket == -1)
    {
        LOG_PERROR("Problem with client connecting");
        return ERR_TCP_INTERNAL;
    }

    LOG_INFO("Connection to socket nr. %d accepted.", g_client_socket);
    return ERR_ALL_GOOD;
}

int Tcp_get_client_socket() { return g_client_socket; }

void Tcp_close_server_socket()
{
    LOG_INFO("PID `%d` closing server socket Nr. %d\n", getpid(), g_server_socket);
    close(g_server_socket);
    shutdown(g_server_socket, SHUT_RDWR);
}

void Tcp_close_client_socket()
{
    LOG_INFO("Closing client socket Nr. %d\n", g_client_socket);
    close(g_client_socket);
    shutdown(g_client_socket, SHUT_RDWR);
}

Error Tcp_read(char* in_buff)
{
    int bytes_recv = read(g_client_socket, in_buff, TCP_MAX_MSG_LEN);
    if (bytes_recv == -1)
    {
        LOG_PERROR("Socket error");
        return ERR_TCP_INTERNAL;
    }
    LOG_TRACE("Client socket: %d - bytes %d\n", g_client_socket, bytes_recv);
    return ERR_ALL_GOOD;
}
