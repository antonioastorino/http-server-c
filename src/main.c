#include "class_http.h"
#include "class_json.h"
#include "class_string.h"
#include "class_string_array.h"
#include "class_tcp.h"
#include "common.h"
#include "converter.h"
#include "fs_utils.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

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
int main()
{
    return_on_err(Tcp_server_init());

    LOG_INFO("Server running");
    while (1)
    {
        if (is_err(Tcp_accept()))
        {
            LOG_ERROR("Failed to accept connection.");
            continue;
        }

        int pid = fork();
        if (pid == 0)
        {
            // Child process
            Tcp_close_server_socket();
            // receive message from the client
            char in_buff[TCP_MAX_MSG_LEN] = {0};
            if (is_err(Tcp_read(in_buff)))
            {
                Tcp_close_client_socket();
                return ERR_UNEXPECTED;
            }
            HttpReqObj req_obj;
            Error ret_res = HttpReqObj_new(in_buff, &req_obj);
            if (!is_err(ret_res))
            {
                ret_res = HttpReqObj_handle(&req_obj, Tcp_get_client_socket());
            }

            HttpReqObj_destroy(&req_obj);
            Tcp_close_client_socket();

            return ret_res;
        }
        else
        {
            // Parent process
            Tcp_close_client_socket();
            pthread_t t1;
            if (pthread_create(&t1, NULL, watchdog, &pid) == -1)
            {
                LOG_ERROR("Failed to create request watchdog. Killing PID `%d`", pid);
                kill(pid, SIGKILL);
            }
        }
    }

    return ERR_ALL_GOOD;
}
#elif TEST == 1
int main()
{
    test_http_req_header();
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
