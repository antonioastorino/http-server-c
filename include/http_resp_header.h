#ifndef HTTP_RESP_HEADER
#define HTTP_RESP_HEADER
#include "common.h"

typedef enum
{
    OK_200,
    NOT_FOUND_404,
    UNKNOWN,
} HttpRespStatus;

typedef struct {
    HttpRespStatus status;
    char* reason_phrase;
 } HttpRespHeader;


#if TEST == 1
void test_http_resp_header();
#endif
#endif /* HTTP_RESP_HEADER */
