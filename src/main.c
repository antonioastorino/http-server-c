#include "common.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        printf("Please specify one argument only - port number\n");
        return -1;
    }

    printf("Server running\n");

    const int domain   = AF_INET; // (= PF_INET) Internet domain sockets for use with IPv4 addresses
    const int type     = SOCK_STREAM; // bitstream socket used in TCP
    const int protocol = 0; // default type of socket that works with the other chosen params

    // create a socket
    int network_socket = socket(domain, type, protocol);
    if (network_socket == -1)
    {
        printf("Failed to create socket\n");
        return -1;
    }

    // specify address
    struct sockaddr_in server_address;
    server_address.sin_family      = AF_INET;
    server_address.sin_port        = htons(atoi(argv[1]));
    server_address.sin_addr.s_addr = INADDR_ANY; // htonl(0x7F000001 /*127.0.0.1*/);

    int bind_status
        = bind(network_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    if (bind_status == -1)
    {
        printf("Failed to bind to network socket\n");
        return -1;
    }

    // listen to connections
    if (listen(network_socket, SOMAXCONN) == -1)
    {
        printf("Failed to listen to socket\n");
        return -1;
    }
    struct sockaddr_in client;
    socklen_t client_size;

    while (1)
    {
        int client_socket = accept(network_socket, (struct sockaddr*)&client, &client_size);

        if (client_socket == -1)
        {
            printf("Problem with client connecting\n");
            return -1;
        }

        if (fork() == 0)
        {
            char buffer[MAX_MSG_LEN];
            buffer[0] = 0;
            memset(&buffer, 0, MAX_MSG_LEN);
            // receive message from the client
            int bytes_recv = recv(client_socket, &buffer, MAX_MSG_LEN, 0);

            printf("Client socket: %d - bytes %d\n", client_socket, bytes_recv);
            printf("%s", buffer);

            uint8_t out_buff[512] = "HTTP/1.0 200 OK\r\n\r\nHello\0";
            write(client_socket, (char*)out_buff, strlen((char*)out_buff));
            fprintf(stdout, (char*)out_buff, strlen((char*)out_buff));

            printf("Closing socket Nr. %d\n", client_socket);
            shutdown(client_socket, SHUT_RDWR);
            close(client_socket);
            return 0;
        }
        else
        {
            printf("Connection to socket nr. %d accepted.\n", client_socket);
        }
    }

    return 0;
}
