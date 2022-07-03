#ifndef HTTP_H
#define HTTP_H
#include "class_string.h"
#include "common.h"

typedef enum
{
    METHOD_GET,
    METHOD_POST,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_UNKNOWN,
} HttpMethod;

typedef struct
{
    HttpMethod req_method;
    char* req_path;
} HttpReqHeader;

typedef struct
{
    HttpReqHeader req_header;
    String* req_body_string_obj_p;
} HttpReq;

#if TEST == 1
void test_http();
#endif /* TEST == 1 */
#endif /* HTTP_H */
