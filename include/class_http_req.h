#ifndef CLASS_HTTP_REQ_H
#define CLASS_HTTP_REQ_H
#include "class_string.h"
#include "common.h"
#include "http_req_header.h"

#define HTTP_REQ_TIMEOUT (5)

typedef struct
{
    HttpReqHeader header;
    String body_string_obj;
} HttpReqObj;

Error HttpReqObj_new(const char*, HttpReqObj*);
Error HttpReqObj_destroy(HttpReqObj*);

#if TEST == 1
void test_class_http_req();
#endif /* TEST == 1 */
#endif /* CLASS_HTTP_REQ_H */
