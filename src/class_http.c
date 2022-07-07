#include "class_http.h"
#include "class_string_array.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#ifdef __linux__
#include <sys/sendfile.h>
#endif /* __linux__ */
#include <sys/types.h>
#include <unistd.h>
#define ASSETS_DIR "assets"

Error HttpReqObj_new(const char* raw_request, HttpReqObj* out_http_req_obj)
{
    // Split the raw data into header and body.
    printf("%s\n", raw_request);
    StringArray raw_data_string_array_obj = StringArray_new(raw_request, "\r\n\r\n");
    LOG_INFO("Elements in the array: `%lu`", raw_data_string_array_obj.num_of_elements);
    if (raw_data_string_array_obj.num_of_elements < 2)
    {
        // StringArray_new() returns 0 elements if the string is empty, one element (the original
        // string) if the pattern is not found. Both cases are unacceptable.
        LOG_ERROR("Invalid header");
        StringArray_destroy(&raw_data_string_array_obj);
        return ERR_INVALID;
    }
    // Process the first half, assuming that it's the header.
    char* header  = raw_data_string_array_obj.str_array_char_p[0];
    char* body    = raw_data_string_array_obj.str_array_char_p[1];
    Error ret_res = http_header_init(header, &out_http_req_obj->header);
    if (ret_res != ERR_ALL_GOOD)
    {
        LOG_ERROR("Failed to initialize header");
    }
    else
    {
        out_http_req_obj->body_string_obj = String_new(body);
    }
    StringArray_destroy(&raw_data_string_array_obj);
    return ERR_ALL_GOOD;
}

Error HttpReqObj_destroy(HttpReqObj* http_req_obj_p)
{
    if (http_req_obj_p == NULL)
    {
        return ERR_NULL;
    }
    String_destroy(&http_req_obj_p->body_string_obj);
    return ERR_ALL_GOOD;
}

Error HttpReqObj_handle(HttpReqObj* http_req_obj_p, int client_socket)
{
    switch (http_req_obj_p->header.method)
    {
    case METHOD_GET:
    {
        // TODO: use getcwd to make sure the requested path is not a parent of the assets folder
        // const char* path_slices[] = { ASSETS_DIR, http_req_obj_p->header.location, NULL };
        // char resolved_path[MAX_PATH_LENGTH];
        // realpath(assets_path_string_obj.str, resolved_path);
        // String assets_path_string_obj     = String_join(path_slices, "");
        // printf("Resolved `%s`\n\n", resolved_path);
        char assets_path[MAX_PATH_LENGTH] = {0};
        if (strcmp(http_req_obj_p->header.location, "/") == 0)
        {
            sprintf(assets_path, "%s/index.html", ASSETS_DIR);
        }
        else
        {
            sprintf(assets_path, "%s%s", ASSETS_DIR, http_req_obj_p->header.location);
        }
        FILE* assets_file = fopen(assets_path, "r");
        fseek(assets_file, 0L, SEEK_END);
        long file_size = ftell(assets_file);
        fclose(assets_file);
        char out_buff[1024];
        sprintf(out_buff, "HTTP/1.0 200 OK\r\n\r\n");
        write(client_socket, (char*)out_buff, strlen((char*)out_buff));
        LOG_INFO("Looking for resource at `%s`", assets_path);
        off_t len         = 0; // set to 0 will send all the origin file
        int resource_file = open(assets_path, O_RDONLY);
        if (resource_file == -1)
        {
            printf("Error opening file\n");
            return ERR_UNDEFINED;
        }
#ifdef __linux__
        int res = sendfile(resource_file, client_socket, &len, 0);
#else
        int res = sendfile(resource_file, client_socket, 0, &len, NULL, 0);
#endif
        if (res == -1)
        {
            perror("Failed to send file");
            return -1;
        }
        printf("Bytes sent: %ld\n", (long)len);
        printf("Bytes size: %ld\n", file_size);
        close(resource_file);
        break;
    }
    default:
        LOG_ERROR("Method not supported.");
        return ERR_INVALID;
    }
    return ERR_ALL_GOOD;
}

#if TEST == 1
void test_class_http()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Initialize request");
    {
        HttpReqObj req_obj;
        const char* req_raw
            = "POST /some/path PROTOCOL\r\ncontent-type: some_value\r\n\r\nsome body\r\n";
        ASSERT(HttpReqObj_new(req_raw, &req_obj) == ERR_ALL_GOOD, "Valid request.");
        ASSERT(req_obj.header.method == METHOD_POST, "Method POST correct.");
        ASSERT_EQ(req_obj.header.location, "/some/path", "Path stored properly.");
        ASSERT(req_obj.header.protocol == PROTOCOL_VALID, "Protocol correct.");
        ASSERT_EQ(req_obj.body_string_obj.str, "some body\r\n", "Body correct");
        HttpReqObj_destroy(&req_obj);
    }
}
#endif /* TEST == 1 */
