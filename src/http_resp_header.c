#include "fs_utils.h"
#include "http_resp_header.h"

#define HTTP_RESP_VERSION "HTTP/1.0"
#define ASSETS_DIR "assets"

static HttpRespStatus g_http_resp_status;

Error http_get_resp_header_init(
    HttpReqHeader* http_req_header_p,
    HttpRespHeader* out_http_resp_header_p)
{
    char assets_path[PATH_MAX]           = {0};
    // Set the file size to zero. This is used in case there is no file to send because not
    // requested or because there was an error.
    out_http_resp_header_p->content_size = 0;
    if (strcmp(http_req_header_p->location, "/") == 0)
    {
        sprintf(assets_path, "%s/index.html", ASSETS_DIR);
    }
    else
    {
        sprintf(assets_path, "%s%s", ASSETS_DIR, http_req_header_p->location);
    }
    char resolved_path[PATH_MAX];
    if (realpath(assets_path, resolved_path) == NULL)
    {
        LOG_ERROR("Invalid path");
        out_http_resp_header_p->status = FORBIDDEN_403;
        return ERR_INVALID;
    }
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

const char* http_resp_reason_phrase()
{
    switch (g_http_resp_status)
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
        return "INVALID RESPONSE";
    }
    return "";
}

Error http_resp_header_to_string(HttpRespHeader* http_resp_header_p, char out_resp_header_char_p[])
{
    if (http_resp_header_p->status == RESP_STATUS_UNDEFINED)
    {
        LOG_ERROR("Unknown response status");
        out_resp_header_char_p[0] = '\0';
        return ERR_UNDEFINED;
    }
    sprintf(
        out_resp_header_char_p,
        "%s %s\r\nContent-Size: %lu\r\n\r\n",
        HTTP_RESP_VERSION,
        http_resp_reason_phrase(),
        http_resp_header_p->content_size);
    return ERR_ALL_GOOD;
}

#if TEST == 1
void test_http_resp_header()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Response string");
    {
        g_http_resp_status = OK_200;
        ASSERT_EQ("200 OK", http_resp_reason_phrase(), "Correct string");
    }
}
#endif
