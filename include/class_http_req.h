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

Error _HttpReqObj_new(const char* file, const int line, const char*, HttpReqObj*);
Error HttpReqObj_destroy(HttpReqObj*);

#define HttpReqObj_new(param1, param2) _HttpReqObj_new(__FILE__, __LINE__, param1, param2)
#define HttpReqObj_empty(var_name)                                                                 \
    String_empty(var_name##body);                                                                  \
    HttpReqObj var_name = {.body_string_obj = var_name##body}
#if TEST == 1
void test_class_http_req(void);
#endif /* TEST == 1 */
#endif /* CLASS_HTTP_REQ_H */
