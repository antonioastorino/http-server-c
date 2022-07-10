#ifndef CLASS_HTTP_RESP_H
#define CLASS_HTTP_RESP_H
#include "class_http_req.h"
#include "common.h"
#include "http_resp_header.h"

typedef struct
{
    HttpRespHeader header;
    char* path_to_data[PATH_MAX];
} HttpRespObj;

Error HttpRespObj_new(HttpReqObj*, HttpRespObj*);
Error HttpRespObj_destroy(HttpRespObj*);

#if TEST == 1
void test_class_http_resp();
#endif
#endif /* CLASS_HTTP_RESP_H */
