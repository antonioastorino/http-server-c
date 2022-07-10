#include "class_string_array.h"
#include "fs_utils.h"
#include "http_resp_header.h"

#define HTTP_RESP_VERSION "HTTP/1.0"
#define ASSETS_DIR "assets"

Error http_get_resp_header_init(
    HttpReqHeader* http_req_header_p,
    HttpRespHeader* out_http_resp_header_p)
{
    char assets_path[PATH_MAX]               = {0};
    out_http_resp_header_p->status           = RESP_STATUS_UNDEFINED;
    // Set the file size to zero. This is used in case there is no file to send because not
    // requested or because there was an error.
    out_http_resp_header_p->content_size     = 0;
    StringArray parsed_path_string_array_obj = StringArray_new(http_req_header_p->location, "?");
    char* location                           = parsed_path_string_array_obj.str_array_char_p[0];
    if (strcmp(location, "/") == 0)
    {
        sprintf(assets_path, "%s/index.html", ASSETS_DIR);
    }
    else
    {
        sprintf(assets_path, "%s%s", ASSETS_DIR, location);
    }
    char resolved_path[PATH_MAX];
    if (realpath(assets_path, resolved_path) == NULL)
    {
        LOG_ERROR("Invalid path `%s`", assets_path);
        out_http_resp_header_p->status = FORBIDDEN_403;
        StringArray_destroy(&parsed_path_string_array_obj);
        return ERR_INVALID;
    }
    StringArray_destroy(&parsed_path_string_array_obj);
    off_t file_size;
    if (is_err(fs_utils_get_file_size(resolved_path, &file_size)))
    {
        LOG_ERROR("Could not retrieve file size");
        out_http_resp_header_p->status = FORBIDDEN_403;
        return ERR_UNEXPECTED;
    }
    memcpy(out_http_resp_header_p->actual_location, resolved_path, PATH_MAX);
    out_http_resp_header_p->content_size = file_size;
    out_http_resp_header_p->status       = OK_200;
    return ERR_ALL_GOOD;
}

const char* http_resp_reason_phrase(HttpRespStatus http_resp_status)
{
    switch (http_resp_status)
    {
    case OK_200:
        return "200 OK";
    case FORBIDDEN_403:
        return "403 Forbidden";
    case NOT_FOUND_404:
        return "404 Not Found";
    case RESP_STATUS_UNDEFINED:
        return "UNDEFINED";
    default:
        return "INVALID STATUS";
    }
    return "";
}

Error http_resp_header_to_string(
    HttpRespHeader* http_resp_header_p,
    String* out_resp_header_string_obj_p)
{
    if (http_resp_header_p->status == RESP_STATUS_UNDEFINED)
    {
        LOG_ERROR("Unknown response status");
        out_resp_header_string_obj_p = NULL;
        return ERR_UNDEFINED;
    }
    *out_resp_header_string_obj_p = String_new(
        "%s %s\r\nContent-Size: %lu\r\n\r\n",
        HTTP_RESP_VERSION,
        http_resp_reason_phrase(http_resp_header_p->status),
        http_resp_header_p->content_size);
    return ERR_ALL_GOOD;
}

#if TEST == 1
void test_http_resp_header()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Response string");
    {
        HttpRespStatus http_resp_status = OK_200;
        ASSERT_EQ("200 OK", http_resp_reason_phrase(http_resp_status), "Correct string");
        http_resp_status = FORBIDDEN_403;
        ASSERT_EQ("403 Forbidden", http_resp_reason_phrase(http_resp_status), "Correct string");
        http_resp_status = NOT_FOUND_404;
        ASSERT_EQ("404 Not Found", http_resp_reason_phrase(http_resp_status), "Correct string");
        http_resp_status = RESP_STATUS_UNDEFINED;
        ASSERT_EQ("UNDEFINED", http_resp_reason_phrase(http_resp_status), "Correct string");
        http_resp_status = 10;
        ASSERT_EQ("INVALID STATUS", http_resp_reason_phrase(http_resp_status), "Correct string");
    }
}
#endif
