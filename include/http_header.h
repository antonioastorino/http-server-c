#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H
#include "common.h"
#include <limits.h>

typedef enum
{
    METHOD_GET,
    METHOD_POST,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_UNKNOWN,
} HttpMethod;

typedef enum
{
    PROTOCOL_VALID,
    PROTOCOL_UNKNOWN,
} HttpProtocol;

typedef struct
{
    HttpMethod method;
    char location[PATH_MAX];
    HttpProtocol protocol;
} HttpReqHeader;

Error http_header_init(const char*, HttpReqHeader*);

#if TEST == 1
void test_http_header();
#endif

#endif /* HTTP_HEADER_H */
