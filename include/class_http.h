#ifndef CLASS_HTTP_H
#define CLASS_HTTP_H
#include "class_string.h"
#include "common.h"
#include "http_header.h"

typedef struct
{
    HttpReqHeader req_header;
    String req_body_string_obj;
} HttpReqObj;

Error HttpReqObj_new(const char*, HttpReqObj*);
Error HttpReqObj_destroy(HttpReqObj*);
#if TEST == 1
void test_class_http();
#endif /* TEST == 1 */
#endif /* HTTP_H */
