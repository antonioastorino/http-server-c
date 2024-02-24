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
#include <signal.h>
#include <stdlib.h> /* contains exit() and getenv() */
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ATTEMPTS 100

void wait_with_timeout(int pid)
{
    LOG_INFO("Waiting for process with ID `%d` to be done.", pid);
    for (uint8_t attempt = 0; attempt <= MAX_ATTEMPTS; attempt++)
    {
        // TODO: replace usleep() with nanosleep();
        usleep(50000);
        int process_exit_status;
        int watched_pid = waitpid(pid, &process_exit_status, WNOHANG);
        if (watched_pid == 0)
        {
            LOG_WARNING("Attempt: `%d` - The process with ID `%d` is not done yet.", attempt, pid);
            if (attempt == MAX_ATTEMPTS)
            {
                LOG_WARNING("Timer expired. Killing PID `%d`.", pid);
                kill(pid, SIGTERM);
                usleep(20000);
                if (waitpid(pid, &process_exit_status, WNOHANG) != pid)
                {
                    LOG_ERROR("The process with ID `%d` is a zombie.", pid);
                }
                return;
            }
        }
        else if (watched_pid == -1)
        {
            LOG_PERROR("waitpid() failed while waiting for `%d`.", pid);
            return;
        }
        else
        {
            LOG_INFO(
                "Child `%d` done. Exited with status `%d`.", pid, WEXITSTATUS(process_exit_status));
            return;
        }
    }
}

Error handle_request()
{
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
                 ret_result
                 = http_resp_header_to_string(&http_resp_obj.header, &http_resp_header_string_obj)))
    {
        LOG_ERROR("[CHILD] Failed to create response object.");
    }
    else if (is_err(ret_result = tcp_utils_write(http_resp_header_string_obj.str)))
    {
        LOG_ERROR("[CHILD] Failed to send header.");
    }
    else if (http_resp_obj.header.content_length == 0)
    {
        LOG_INFO("[CHILD] Content-Length zero. Nothing to send.");
    }
    else if (is_err(
                 ret_result = tcp_utils_send_file(
                     http_resp_obj.header.actual_location, http_resp_obj.header.content_length)))
    {
        LOG_ERROR("[CHILD] Failed to send payload.");
    }
    else
    {
        LOG_TRACE("[CHILD] Done.");
    }
    HttpRespObj_destroy(&http_resp_obj);
    HttpReqObj_destroy(&http_req_obj);
    String_destroy(&http_resp_header_string_obj);
    return ret_result;
}
#if TEST == 0
int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        if (strncmp(argv[1], "-v\0", 3) == 0)
        {
            printf("APP_VERSION: XXX");
            return 0;
        }
    }
    else if (argc != 1)
    {
        printf("Invalid parameters\n");
        return ERR_INVALID;
    }
    char* logger_out_file_str = "/tmp/app.log";
    logger_init(logger_out_file_str, logger_out_file_str);
    return_on_err(tcp_utils_server_init(8081));
    LOG_INFO("Server running");
    while (1)
    {
        if (is_err(tcp_utils_accept()))
        {
            LOG_ERROR("Failed to accept connection.");
            return -1;
        }

        int pid = fork();
        if (pid < 0)
        {
            LOG_PERROR("Fatal");
            exit(errno);
        }
        if (pid == 0)
        {
            // Child process
            tcp_utils_close_server_socket();
            Error ret_result = handle_request();
            tcp_utils_close_client_socket();

            return ret_result;
        }
        else
        {
            // Parent process
            tcp_utils_close_client_socket();
            wait_with_timeout(pid);
        }
    }

    return ERR_ALL_GOOD;
}
#elif TEST == 1
// void test_api_get();
// void test_api_post();
// void test_api_common();
int main()
{
    logger_init(NULL, NULL);
    test_http_req_header();
    test_http_resp_header();
    test_class_http_req();
    test_class_http_resp();
    //    test_api_get();
    //    test_api_post();
    //    test_api_common();
}
#else
#error "TEST must be 0 or 1"
#endif /* TEST == 0 */
