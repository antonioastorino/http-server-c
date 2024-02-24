#include "class_http_resp.h"

Error HttpRespObj_new(HttpReqObj* http_req_obj_p, HttpRespObj* out_http_resp_obj_p)
{
    switch (http_req_obj_p->header.method)
    {
    case METHOD_GET:
    {
        http_resp_header_init_GET(&http_req_obj_p->header, &out_http_resp_obj_p->header);
        break;
    }
    case METHOD_POST:
    {
        http_resp_header_init_POST(
            &http_req_obj_p->header,
            &http_req_obj_p->body_string_obj,
            &out_http_resp_obj_p->header);
        break;
    }
    default:
        LOG_ERROR("Method not supported.");
        return ERR_HTTP_UNSUPPORTED_METHOD;
    }
    return ERR_ALL_GOOD;
}

Error HttpRespObj_destroy(HttpRespObj* http_resp_obj_p)
{
    // Nothing to destroy, so far
    UNUSED(http_resp_obj_p);
    return ERR_ALL_GOOD;
}

#if TEST == 1
void test_class_http_resp() {}
#endif /* TEST == 1 */
