#include "class_string_array.h"
#include "http.h"

HttpMethod http_parse_method(const String* method_string_obj_p)
{
    if (0 == strncmp(method_string_obj_p->str, "GET", method_string_obj_p->length))
    {
        return METHOD_GET;
    }
    if (0 == strncmp(method_string_obj_p->str, "POST", method_string_obj_p->length))
    {
        return METHOD_POST;
    }
    if (0 == strncmp(method_string_obj_p->str, "PUT", method_string_obj_p->length))
    {
        return METHOD_PUT;
    }
    if (0 == strncmp(method_string_obj_p->str, "DELETE", method_string_obj_p->length))
    {
        return METHOD_DELETE;
    }

    return METHOD_UNKNOWN;
}

Error HttpHeader_new(const char* header_str_p, HttpReqHeader* out_http_header)
{
    StringArray header_lines_string_array_obj = StringArray_new(header_str_p, "\r\n");
    if (header_lines_string_array_obj.num_of_elements == 0)
    {
        out_http_header->req_method = METHOD_UNKNOWN;
        out_http_header->req_path   = NULL;
        return ERR_INVALID;
    }
    StringArray_destroy(&header_lines_string_array_obj);
    return ERR_ALL_GOOD;
}

Error HttpHeader_destroy(HttpReqHeader* http_req_header_obj_p)
{
    if (http_req_header_obj_p == NULL)
    {
        return ERR_NULL;
    }
    if (http_req_header_obj_p->req_path == NULL)
    {
        return ERR_NULL;
    }
    FREE(http_req_header_obj_p->req_path);
    http_req_header_obj_p->req_path = NULL;
    return ERR_ALL_GOOD;
}

#if TEST == 1
void test_http()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Parsing method - pass");
    {
        HttpMethod rqv;

        String get_string_obj_p = String_new("GET");
        rqv                     = http_parse_method(&get_string_obj_p);
        ASSERT(rqv == METHOD_GET, "GET request understood");
        String_destroy(&get_string_obj_p);

        String post_string_obj_p = String_new("POST");
        rqv                      = http_parse_method(&post_string_obj_p);
        ASSERT(rqv == METHOD_POST, "POST request understood");
        String_destroy(&post_string_obj_p);

        String put_string_obj_p = String_new("PUT");
        rqv                     = http_parse_method(&put_string_obj_p);
        ASSERT(rqv == METHOD_PUT, "PUT request understood");
        String_destroy(&put_string_obj_p);

        String delete_string_obj_p  = String_new("DELETE");
        rqv = http_parse_method(&delete_string_obj_p);
        ASSERT(rqv == METHOD_DELETE, "DELETE request understood");
        String_destroy(&delete_string_obj_p);

        String invalid_string_obj_p = String_new("something");
        rqv                         = http_parse_method(&invalid_string_obj_p);
        ASSERT(rqv == METHOD_UNKNOWN, "Invalid method or syntax error.");
        String_destroy(&invalid_string_obj_p);
    }
    PRINT_TEST_TITLE("Populate header");
    {
        HttpReqHeader http_req_header_obj;
        Error ret_res = HttpHeader_new("GET / HTTP/1.1", &http_req_header_obj);
        ASSERT(ret_res == ERR_ALL_GOOD, "Ok.");

        ASSERT(HttpHeader_new("", &http_req_header_obj) == ERR_INVALID, "Empty header handled.");
        ASSERT(http_req_header_obj.req_method == METHOD_UNKNOWN, "Unknown method set.");
        ASSERT(http_req_header_obj.req_path == NULL, "Path not set.");
    }
}
#endif /* TEST == 1 */
