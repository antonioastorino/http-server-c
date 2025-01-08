#ifndef HTTP_RESP_HEADER
#define HTTP_RESP_HEADER
#include "common.h"
#include "http_req_header.h"
#include "http_resp_status.h"
#define REASON_PHRASE_LENGTH (64)

typedef struct
{
    HttpRespStatus status;
    char reason_phrase[REASON_PHRASE_LENGTH];
    size_t content_length;
    char* content_type;
    char actual_location[PATH_MAX];
} HttpRespHeader;

void http_resp_header_init_GET(const HttpReqHeader*, HttpRespHeader*);
void http_resp_header_init_POST(const HttpReqHeader*, const String*, HttpRespHeader*);
Error http_resp_header_to_string(const HttpRespHeader*, String*);

#if TEST == 1
void test_http_resp_header(void);
#endif
#endif /* HTTP_RESP_HEADER */
