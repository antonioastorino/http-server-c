#ifndef CLASS_HTTP_H
#define CLASS_HTTP_H
#include "class_string.h"
#include "common.h"
#include "http_req_header.h"
#include "http_resp_header.h"

#define HTTP_REQ_TIMEOUT (5)

typedef struct
{
    HttpReqHeader header;
    String body_string_obj;
} HttpReqObj;

typedef struct
{
    HttpRespHeader header;
    char* path_to_data;
} HttpRespObj;

Error HttpReqObj_new(const char*, HttpReqObj*);
Error HttpReqObj_destroy(HttpReqObj*);
Error HttpReqObj_handle(HttpReqObj*, int);

#if TEST == 1
void test_class_http();
#endif /* TEST == 1 */
#endif /* HTTP_H */
