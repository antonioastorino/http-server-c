#include "class_http.h"
#include "class_string_array.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define HTTP_VERSION "HTTP/1.0"

#ifdef __linux__
#include <sys/sendfile.h>
#endif /* __linux__ */

#define ASSETS_DIR "assets"

typedef struct {
    
    HttpVersion version;
} HttpRespHeader;

Error HttpReqObj_new(const char* raw_request, HttpReqObj* out_http_req_obj)
{
    // Split the raw data into header and body.
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
    Error ret_res = http_req_header_init(header, &out_http_req_obj->header);
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
    char response_header_char_p[1024];
    switch (http_req_obj_p->header.method)
    {
    case METHOD_GET:
    {
        // TODO: use getcwd to make sure the requested path is not a parent of the assets folder
        // const char* path_slices[] = { ASSETS_DIR, http_req_obj_p->header.location, NULL };
        // String assets_path_string_obj     = String_join(path_slices, "");
        // printf("Resolved `%s`\n\n", resolved_path);
        char assets_path[PATH_MAX] = {0};
        if (strcmp(http_req_obj_p->header.location, "/") == 0)
        {
            sprintf(assets_path, "%s/index.html", ASSETS_DIR);
        }
        else
        {
            sprintf(assets_path, "%s%s", ASSETS_DIR, http_req_obj_p->header.location);
        }
        char resolved_path[PATH_MAX];
        if (realpath(assets_path, resolved_path) == NULL)
        {
            LOG_ERROR("Invalid path");
            sprintf(response_header_char_p, HTTP_VERSION " 404 Not Found\r\n\r\n");
            write(
                client_socket,
                (char*)response_header_char_p,
                strlen((char*)response_header_char_p));
            return ERR_ALL_GOOD;
        }
        sprintf(response_header_char_p, HTTP_VERSION " 200 OK\r\n\r\n");
        write(client_socket, (char*)response_header_char_p, strlen((char*)response_header_char_p));
        LOG_INFO("Looking for resource at `%s`", resolved_path);
        int resource_file = open(resolved_path, O_RDONLY);
        if (resource_file == -1)
        {
            LOG_PERROR("Error opening file");
            return ERR_UNDEFINED;
        }
#ifdef __linux__
        FILE* assets_file = fopen(resolved_path, "r");
        if (fseek(assets_file, 0L, SEEK_END) == -1)
        {
            LOG_PERROR("fseek failed");
            return ERR_HTTP_INTERNAL;
        }
        long file_size = ftell(assets_file);
        fclose(assets_file);
        ssize_t bytes_sent = sendfile(client_socket, resource_file, NULL, file_size);
        LOG_INFO("Size `%ld` bytes.", file_size);
        LOG_INFO("Sent `%ld` bytes.", bytes_sent);
        if (bytes_sent == -1)
#else
        off_t len = 0; // set to 0 will send all the origin file
        int res   = sendfile(resource_file, client_socket, 0, &len, NULL, 0);
        LOG_INFO("Sent `%lld` bytes.", len);
        if (res == -1)
#endif
        {
            LOG_PERROR("Failed to send file");
            return -1;
        }
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
            = "POST /some/path VERSION\r\ncontent-type: some_value\r\n\r\nsome body\r\n";
        ASSERT(HttpReqObj_new(req_raw, &req_obj) == ERR_ALL_GOOD, "Valid request.");
        ASSERT(req_obj.header.method == METHOD_POST, "Method POST correct.");
        ASSERT_EQ(req_obj.header.location, "/some/path", "Path stored properly.");
        ASSERT(req_obj.header.version == VERSION_VALID, "Version correct.");
        ASSERT_EQ(req_obj.body_string_obj.str, "some body\r\n", "Body correct");
        HttpReqObj_destroy(&req_obj);
    }
}
#endif /* TEST == 1 */
