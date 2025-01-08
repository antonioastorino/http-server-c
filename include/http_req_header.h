#ifndef HTTP_REQ_HEADER_H
#define HTTP_REQ_HEADER_H
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
    VERSION_VALID,
    VERSION_UNKNOWN,
} HttpVersion;

typedef struct
{
    HttpMethod method;
    char location[PATH_MAX];
    HttpVersion version;
} HttpReqHeader;

Error http_req_header_init(const char*, HttpReqHeader*);

#if TEST == 1
void test_http_req_header(void);
#endif

#endif /* HTTP_REQ_HEADER_H */
