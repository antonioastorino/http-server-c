#ifndef HTTP_RESP_HEADER
#define HTTP_RESP_HEADER
#include "common.h"
#include "http_req_header.h"
#define REASON_PHRASE_LENGTH (64)

typedef enum
{
    RESP_STATUS_UNDEFINED,
    OK_200,
    FORBIDDEN_403,
    NOT_FOUND_404,
} HttpRespStatus;

typedef struct
{
    HttpRespStatus status;
    char reason_phrase[REASON_PHRASE_LENGTH];
    size_t content_length;
    char* content_type;
    char actual_location[PATH_MAX];
} HttpRespHeader;

void http_get_resp_header_init(const HttpReqHeader*, HttpRespHeader*);
void http_post_resp_header_init(const HttpReqHeader*, HttpRespHeader*);
Error http_resp_header_to_string(const HttpRespHeader*, String*);

#if TEST == 1
void test_http_resp_header();
#endif
#endif /* HTTP_RESP_HEADER */
