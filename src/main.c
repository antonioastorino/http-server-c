#include "class_http_req.h"
#include "class_http_resp.h"
#include "class_json.h"
#include "class_string.h"
#include "class_string_array.h"
#include "common.h"
#include "converter.h"
#include "fs_utils.h"
#include "http_resp_header.h"
#include "tcp_utils.h"
#include <pthread.h>
#include <signal.h>
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
    return_on_err(tcp_utils_server_init());

    LOG_INFO("Server running");
    while (1)
    {
        if (is_err(tcp_utils_accept()))
        {
            LOG_ERROR("Failed to accept connection.");
            return -1;
        }

        int pid = fork();
        if (pid == 0)
        {
            // Child process
            tcp_utils_close_server_socket();
            // receive message from the client
            char in_buff[TCP_MAX_MSG_LEN] = {0};
            Error ret_result              = ERR_ALL_GOOD;
            HttpReqObj_empty(http_req_obj);
            HttpRespObj_empty(http_resp_obj);
            String_empty(http_resp_header_string_obj);
            if (is_err(ret_result = tcp_utils_read(in_buff)))
            {
                LOG_ERROR("[CHILD] Failed to read input message.");
            }
            else if (is_err(ret_result = HttpReqObj_new(in_buff, &http_req_obj)))
            {
                LOG_ERROR("[CHILD] Failed to create request object.");
            }
            else if (is_err(ret_result = HttpRespObj_new(&http_req_obj, &http_resp_obj)))
            {
                LOG_ERROR("[CHILD] Failed to create response object.");
            }
            else if (is_err(
                         ret_result = http_resp_header_to_string(
                             &http_resp_obj.header, &http_resp_header_string_obj)))
            {
                LOG_ERROR("[CHILD] Failed to create response object.");
            }
            else if (is_err(ret_result = tcp_utils_write(http_resp_header_string_obj.str)))
            {
                LOG_ERROR("[CHILD] Failed to send header.");
            }
            else if (!http_resp_obj.header.content_length)
            {
                LOG_INFO("[CHILD] Content-Length not zero. Trying to send file.");
            }
            else if (is_err(
                         ret_result = tcp_utils_send_file(
                             http_resp_obj.header.actual_location,
                             http_resp_obj.header.content_length)))
            {
                LOG_ERROR("[CHILD] Failed to send payload.");
            }
            else
            {
                LOG_TRACE("[CHILD] Input message read.");
            }
            HttpRespObj_destroy(&http_resp_obj);
            HttpReqObj_destroy(&http_req_obj);
            String_destroy(&http_resp_header_string_obj);
            tcp_utils_close_client_socket();

            return ret_result;
        }
        else
        {
            // Parent process
            tcp_utils_close_client_socket();
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
    test_http_resp_header();
    test_class_http_req();
    test_class_http_resp();
}
#else
#error "TEST must be 0 or 1"
#endif /* TEST == 0 */
