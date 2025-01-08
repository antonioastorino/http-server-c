#include "class_http_req.h"
#include "class_string_array.h"
#include <errno.h>
#include <unistd.h>

Error _HttpReqObj_new(
    const char* file,
    const int line,
    const char* raw_request,
    HttpReqObj* out_http_req_obj)
{
    // Split the raw data into header and body.
    out_http_req_obj->header.method       = METHOD_UNKNOWN;
    out_http_req_obj->header.version      = VERSION_UNKNOWN;
    StringArray raw_data_string_array_obj = _StringArray_new(file, line, raw_request, "\r\n\r\n");
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
        out_http_req_obj->body_string_obj = _String_new(file, line, body);
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

#if TEST == 1
void test_class_http_req(void)
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Initialize request");
    {
        HttpReqObj_empty(req_obj);
        const char* req_raw
            = "POST /some/path VERSION\r\ncontent-type: some_value\r\n\r\nsome body\r\n";
        ASSERT(HttpReqObj_new(req_raw, &req_obj) == ERR_ALL_GOOD, "Valid request.");
        ASSERT(req_obj.header.method == METHOD_POST, "Method POST correct.");
        ASSERT_EQ(req_obj.header.location, "/some/path", "Path stored properly.");
        ASSERT(req_obj.header.version == VERSION_VALID, "Version correct.");
        ASSERT_EQ(req_obj.body_string_obj.str, "some body\r\n", "Body correct");
        HttpReqObj_destroy(&req_obj);
        HttpReqObj_destroy(&req_obj);
    }
}
#endif /* TEST == 1 */
