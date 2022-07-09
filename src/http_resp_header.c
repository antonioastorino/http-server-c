#include "http_resp_header.h"

static HttpRespStatus g_http_resp_status;

const char* http_resp_status_string()
{
    switch (g_http_resp_status)
    {
    case OK_200:
        return "200 OK";
    case NOT_FOUND_404:
        return "404 Not Found";
    case UNKNOWN:
        return "UNKNOWN";
     default:
        return "INVALID RESPONSE";
    }
    return "";
}

#if TEST == 1
void test_http_resp_header()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Response string");
    {
        g_http_resp_status = OK_200;
        ASSERT_EQ("200 OK", http_resp_status_string(), "Correct string");
    }
}
#endif
