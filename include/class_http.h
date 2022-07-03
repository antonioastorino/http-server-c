#ifndef HTTP_H
#define HTTP_H
#include "class_string.h"
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
    HttpMethod req_method;
    char req_path[MAX_PATH_LENGTH];
    HttpProtocol req_protocol;
} HttpReqHeader;

typedef struct
{
    HttpReqHeader req_header;
    String req_body_string_obj;
} HttpReqObj;

#if TEST == 1
void test_http();
#endif /* TEST == 1 */
#endif /* HTTP_H */
