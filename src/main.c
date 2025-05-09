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
#include <stdlib.h> /* free() */
#include <pthread.h>

#define MAX_ATTEMPTS 100
typedef struct
{
    int client_socket;
    Error out_result;
} ThreadData;

void* handle_request(void* thread_data_void_p)
{
    char in_buff[TCP_MAX_MSG_LEN] = {0};
    ThreadData* thread_data_p     = (ThreadData*)thread_data_void_p;
    thread_data_p->out_result     = ERR_ALL_GOOD;

    LOG_INFO("[SOCKET `%d`] Handling request.", thread_data_p->client_socket);
    HttpReqObj_empty(http_req_obj);
    HttpRespObj_empty(http_resp_obj);
    String_empty(http_resp_header_string_obj);
    if (is_err(thread_data_p->out_result = tcp_utils_read(in_buff, thread_data_p->client_socket)))
    {
        LOG_ERROR("[SOCKET `%d`] Failed to read input message.", thread_data_p->client_socket);
    }
    else if (is_err(thread_data_p->out_result = HttpReqObj_new(in_buff, &http_req_obj)))
    {
        LOG_ERROR("[SOCKET `%d`] Failed to create request object.", thread_data_p->client_socket);
    }
    else if (is_err(thread_data_p->out_result = HttpRespObj_new(&http_req_obj, &http_resp_obj)))
    {
        LOG_ERROR("[SOCKET `%d`] Failed to create response object.", thread_data_p->client_socket);
    }
    else if (is_err(
                 thread_data_p->out_result
                 = http_resp_header_to_string(&http_resp_obj.header, &http_resp_header_string_obj)))
    {
        LOG_ERROR("[SOCKET `%d`] Failed to create response object.", thread_data_p->client_socket);
    }
    else if (is_err(
                 thread_data_p->out_result
                 = tcp_utils_write(http_resp_header_string_obj.str, thread_data_p->client_socket)))
    {
        LOG_ERROR("[SOCKET `%d`] Failed to send header.", thread_data_p->client_socket);
    }
    else if (http_resp_obj.header.content_length == 0)
    {
        LOG_INFO("[SOCKET `%d`] Content-Length zero. Nothing to send.", thread_data_p->client_socket);
    }
    else if (is_err(
                 thread_data_p->out_result = tcp_utils_send_file(
                     http_resp_obj.header.actual_location,
                     http_resp_obj.header.content_length,
                     thread_data_p->client_socket)))
    {
        LOG_ERROR("[SOCKET `%d`] Failed to send payload.", thread_data_p->client_socket);
    }
    else
    {
        LOG_INFO("[SOCKET `%d`] Request handled`.", thread_data_p->client_socket);
    }
    HttpRespObj_destroy(&http_resp_obj);
    HttpReqObj_destroy(&http_req_obj);
    String_destroy(&http_resp_header_string_obj);
    tcp_utils_close_client_socket(thread_data_p->client_socket);
    free(thread_data_p);
    return NULL;
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
    char* logger_out_file_str = NULL;
    logger_init(logger_out_file_str, logger_out_file_str);
    return_on_err(tcp_utils_server_init(8081));
    LOG_INFO("Server running");
    while (1)
    {
        LOG_INFO("Ready for request.");
        int client_socket = 0;
        if (is_err(tcp_utils_accept(&client_socket)))
        {
            LOG_ERROR("Failed to accept connection.");
            return ERR_FATAL;
        }
        ThreadData* thread_data_p    = (ThreadData*)malloc(sizeof(ThreadData));
        thread_data_p->client_socket = client_socket;
        if (thread_data_p == NULL)
        {
            LOG_ERROR("FATAL: cannot allocate memory.");
            return ERR_FATAL;
        }
        pthread_t client_tread_id;
        int thread_create_result
            = pthread_create(&client_tread_id, NULL, handle_request, (void*)thread_data_p);
        if (thread_create_result != 0)
        {
            LOG_ERROR("FATAL: Failed to create thread: `%d`.", thread_create_result);
            return ERR_FATAL;
        }
    }
    tcp_utils_close_server_socket();

    return ERR_ALL_GOOD;
}
#elif TEST == 1
void test_api_get(void);
void test_api_post(void);
int main(void)
{
    logger_init(NULL, NULL);
    test_http_req_header();
    test_http_resp_header();
    test_class_http_req();
    test_class_http_resp();
    test_api_get();
    test_api_post();
}
#else
#error "TEST must be 0 or 1"
#endif /* TEST == 0 */
