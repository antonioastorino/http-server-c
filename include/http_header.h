#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H
#include "common.h"
#define MAX_PATH_LENGTH (256)

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
    char location[MAX_PATH_LENGTH];
    HttpProtocol protocol;
} HttpReqHeader;

Error http_header_init(const char*, HttpReqHeader*);

#if TEST == 1
void test_http_header();
#endif

#endif /* HTTP_HEADER_H */
