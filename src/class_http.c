#include "class_http.h"
#include "class_string_array.h"

#define compare_method(method_char_p, method_name)                                                 \
    if (0 == strncmp(method_char_p, #method_name, strlen(#method_name)))                           \
    {                                                                                              \
        LOG_TRACE(#method_name " method found");                                                   \
        return METHOD_##method_name;                                                               \
    }

HttpMethod http_parse_method(const char* method_char_p)
{
    LOG_TRACE("Trying to parse method `%s`", method_char_p);
    compare_method(method_char_p, GET);
    compare_method(method_char_p, POST);
    compare_method(method_char_p, PUT);
    compare_method(method_char_p, DELETE);
    LOG_ERROR("Received undefined method `%s`.", method_char_p);
    return METHOD_UNKNOWN;
}

Error http_parse_path(const char* input_path_char_p, char* out_path_char_p)
{
    // TODO: detect errors in path
    LOG_TRACE("Trying to parse path `%s`", input_path_char_p);
    strncpy(out_path_char_p, input_path_char_p, strlen(input_path_char_p));
    return ERR_ALL_GOOD;
}

HttpProtocol http_parse_protocol(const char* input_protocol_char_p)
{
    // TODO: detect protocol
    UNUSED(input_protocol_char_p);
    return PROTOCOL_VALID;
}

Error HttpHeader_init(const char* header_str_p, HttpReqHeader* out_http_header)
{
    Error ret_res                 = ERR_ALL_GOOD;
    out_http_header->req_method   = METHOD_UNKNOWN;
    out_http_header->req_protocol = PROTOCOL_UNKNOWN;
    memset(out_http_header->req_path, 0, MAX_PATH_LENGTH);
    // Split the header into lines.
    StringArray header_lines_string_array_obj = StringArray_new(header_str_p, "\r\n\r\n");
    LOG_INFO("Elements in the array: `%lu`", header_lines_string_array_obj.num_of_elements);
    if (header_lines_string_array_obj.num_of_elements < 2)
    {
        // StringArray_new() returns 0 elements if the string is empty, one element (the original
        // string) if the pattern is not found. Both cases are unacceptable.
        LOG_ERROR("Invalid header");
        StringArray_destroy(&header_lines_string_array_obj);
        out_http_header->req_method = METHOD_UNKNOWN;
        return ERR_INVALID;
    }
    // Split the first line into its component.
    // The result should be:
    // str_array_char_p[0] = method
    // str_array_char_p[1] = /path/to/follow
    // str_array_char_p[2] = protocol (optional)
    StringArray method_path_protocol_string_array_obj
        = StringArray_new(header_lines_string_array_obj.str_array_char_p[0], " ");
    LOG_INFO("Elements in the array: `%lu`", method_path_protocol_string_array_obj.num_of_elements);

    switch (method_path_protocol_string_array_obj.num_of_elements)
    {
    case 0:
    case 1:
        ret_res = ERR_INVALID;
        break;
    case 2:
    case 3:
        // try to parse method and path
        out_http_header->req_method
            = http_parse_method(method_path_protocol_string_array_obj.str_array_char_p[0]);

        if (out_http_header->req_method == METHOD_UNKNOWN)
        {
            LOG_ERROR("Invalid method detected");
            ret_res = ERR_INVALID;
            break;
        }
        if (http_parse_path(
                method_path_protocol_string_array_obj.str_array_char_p[1],
                out_http_header->req_path)
            != ERR_ALL_GOOD)
        {
            ret_res = ERR_INVALID;
            break;
        }
        LOG_TRACE("Path: `%s`", out_http_header->req_path);
        // try to parse protocol as well
        if (method_path_protocol_string_array_obj.num_of_elements == 3)
        {
            out_http_header->req_protocol
                = http_parse_protocol(method_path_protocol_string_array_obj.str_array_char_p[2]);
        }
        break;
    default:
        break;
    }
    StringArray_destroy(&method_path_protocol_string_array_obj);
    // TODO: parse the other lines as a JsonObj.
    StringArray_destroy(&header_lines_string_array_obj);
    return ERR_ALL_GOOD;
}

Error HttpReqObj_destroy(HttpReqObj* http_req_obj_p)
{
    if (http_req_obj_p == NULL)
    {
        return ERR_NULL;
    }
    String_destroy(&http_req_obj_p->req_body_string_obj);
    return ERR_ALL_GOOD;
}

#if TEST == 1
void test_http()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Parsing method - pass");
    {
        HttpMethod rqv;

        const char* get_char_p = "GET";
        rqv                    = http_parse_method(get_char_p);
        ASSERT(rqv == METHOD_GET, "GET request understood");

        const char* post_char_p = "POST";
        rqv                     = http_parse_method(post_char_p);
        ASSERT(rqv == METHOD_POST, "POST request understood");

        const char* put_char_p = "PUT";
        rqv                    = http_parse_method(put_char_p);
        ASSERT(rqv == METHOD_PUT, "PUT request understood");

        const char* delete_char_p = "DELETE";
        rqv                       = http_parse_method(delete_char_p);
        ASSERT(rqv == METHOD_DELETE, "DELETE request understood");

        const char* invalid_char_p = "something";
        rqv                        = http_parse_method(invalid_char_p);
        ASSERT(rqv == METHOD_UNKNOWN, "Invalid method or syntax error.");
    }

    PRINT_TEST_TITLE("Populate header - full");
    {
        HttpReqHeader http_req_header_obj;
        Error ret_res
            = HttpHeader_init("GET /requested/path HTTP/1.1\r\n\r\n", &http_req_header_obj);
        ASSERT(ret_res == ERR_ALL_GOOD, "Initialization successful.");
        ASSERT(http_req_header_obj.req_method == METHOD_GET, "Method found.");
        ASSERT(strcmp(http_req_header_obj.req_path, "/requested/path") == 0, "Path saved.");
        ASSERT(http_req_header_obj.req_protocol == PROTOCOL_VALID, "Protocol saved.");
    }
    PRINT_TEST_TITLE("Populate header - no protocol");
    {
        HttpReqHeader http_req_header_obj;
        Error ret_res = HttpHeader_init("GET /requested/path\r\n\r\n", &http_req_header_obj);
        ASSERT(ret_res == ERR_ALL_GOOD, "Initialization successful.");
        ASSERT(http_req_header_obj.req_method == METHOD_GET, "Method found.");
        printf("Path: %d\n", strcmp(http_req_header_obj.req_path, "/requested/path"));
        ASSERT(strcmp(http_req_header_obj.req_path, "/requested/path") == 0, "Path saved.");
        ASSERT(http_req_header_obj.req_protocol == PROTOCOL_UNKNOWN, "Protocol not found.");
    }
    PRINT_TEST_TITLE("Populate header - fail");
    {
        HttpReqHeader http_req_header_obj;
        ASSERT(HttpHeader_init("GET", &http_req_header_obj) == ERR_INVALID, "Missing path.");
        ASSERT(http_req_header_obj.req_method == METHOD_UNKNOWN, "Unknown method set.");
        ASSERT(strcmp(http_req_header_obj.req_path, "\0") == 0, "Path not set.");
        ASSERT(http_req_header_obj.req_protocol == PROTOCOL_UNKNOWN, "Protocol set to UNKNOWN.");

        ASSERT(HttpHeader_init("", &http_req_header_obj) == ERR_INVALID, "Empty header handled.");
        ASSERT(http_req_header_obj.req_method == METHOD_UNKNOWN, "Unknown method set.");
        ASSERT(strcmp(http_req_header_obj.req_path, "\0") == 0, "Path not set.");
        ASSERT(http_req_header_obj.req_protocol == PROTOCOL_UNKNOWN, "Protocol set to UNKNOWN.");
    }
}
#endif /* TEST == 1 */
