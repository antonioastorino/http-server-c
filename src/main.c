#include "class_http.h"
#include "class_json.h"
#include "class_string.h"
#include "class_string_array.h"
#include "common.h"
#include "converter.h"
#include "fs_utils.h"
#include <signal.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_MSG_LEN 512

void* watchdog(void* pid_p)
{
    pid_t pid = *((pid_t*)pid_p);
    LOG_TRACE("Waiting for child `%d` to finish", pid);
    sleep(HTTP_REQ_TIMEOUT);
    LOG_TRACE("Checking");
    int process_exit_status;
    if (waitpid(pid, &process_exit_status, WNOHANG) == 0)
    {
        LOG_WARNING("The process with ID `%d` is not done yet. Killing it.", pid);
        kill(pid, SIGTERM);
    }
    else
    {
        LOG_TRACE("Child done. Exited with status `%d`.", WEXITSTATUS(process_exit_status));
    }
    pthread_exit(NULL);
}

#if TEST == 0
int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        printf("Please specify one argument only - port number\n");
        return -1;
    }

    LOG_INFO("Server running\n");

    const int domain   = AF_INET; // (= PF_INET) Internet domain sockets for use with IPv4 addresses
    const int type     = SOCK_STREAM; // bitstream socket used in TCP
    const int protocol = 0; // default type of socket that works with the other chosen params
    int on             = 1;

    // create a socket
    int server_socket = socket(domain, type, protocol);
    if (server_socket == -1)
    {
        LOG_ERROR("Failed to create socket\n");
        return -1;
    }

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    // specify address
    struct sockaddr_in server_address;
    server_address.sin_family      = AF_INET;
    server_address.sin_port        = htons(atoi(argv[1]));
    server_address.sin_addr.s_addr = INADDR_ANY; // htonl(0x7F000001 /*127.0.0.1*/);

    int bind_status
        = bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    if (bind_status == -1)
    {
        LOG_ERROR("Failed to bind to network socket\n");
        return -1;
    }

    // listen to connections
    if (listen(server_socket, SOMAXCONN) == -1)
    {
        LOG_ERROR("Failed to listen to socket\n");
        return -1;
    }
    struct sockaddr_in client;
    socklen_t client_size;

    while (1)
    {
        int client_socket = accept(server_socket, (struct sockaddr*)&client, &client_size);

        if (client_socket == -1)
        {
            LOG_ERROR("Problem with client connecting\n");
            continue;
        }

        int pid = fork();
        if (pid == 0)
        {
            // Child process
            close(server_socket);
            char in_buff[MAX_MSG_LEN];
            char out_buff[512];
            // receive message from the client
            int bytes_recv;
            printf("\n---------------------------\n\n");
            memset(&in_buff, 0, MAX_MSG_LEN);
            memset(&out_buff, 0, 512);
            bytes_recv = read(client_socket, &in_buff, MAX_MSG_LEN);

            if (bytes_recv == -1)
            {
                printf("Socket error\n");
                close(client_socket);
                return -1;
            }
            printf("Client socket: %d - bytes %d\n", client_socket, bytes_recv);
            printf("%s\n", in_buff);
            HttpReqObj req_obj;
            Error ret_res = HttpReqObj_new(in_buff, &req_obj);
            if (ret_res == ERR_ALL_GOOD)
            {
                ret_res = HttpReqObj_handle(&req_obj, client_socket);
            }

            HttpReqObj_destroy(&req_obj);
            close(client_socket);
            printf("[CHILD] Closing socket Nr. %d\n", client_socket);
            shutdown(client_socket, SHUT_RDWR);

            return 0;
        }
        else
        {
            // Parent process
            close(client_socket);
            LOG_INFO("[PARENT] Connection to socket nr. %d accepted.\n", client_socket);
            pthread_t t1;
            if (pthread_create(&t1, NULL, watchdog, &pid) == -1)
            {
                LOG_ERROR("Failed to create request watchdog. Killing PID `%d`", pid);
                kill(pid, SIGKILL);
            }
        }
    }

    return 0;
}
#elif TEST == 1
int main()
{
    test_http_header();
    test_class_http();
    test_class_string();
    test_class_string_array();
    test_class_json();
    test_converter();
    test_fs_utils();
}
#else
#error "TEST must be 0 or 1"
#endif /* TEST == 0 */
