#include "class_http_resp.h"

Error HttpRespObj_new(HttpReqObj* http_req_obj_p, HttpRespObj* out_http_resp_obj_p)
{
    switch (http_req_obj_p->header.method)
    {
    case METHOD_GET:
    {
        return http_get_resp_header_init(&http_req_obj_p->header, &out_http_resp_obj_p->header);
        break;
    }
    default:
        LOG_ERROR("Method not supported.");
        return ERR_INVALID;
    }
    return ERR_ALL_GOOD;
}

Error HttpRespObj_destroy(HttpRespObj* http_resp_obj_p)
{
    // Nothing to destroy, so far
    UNUSED(http_resp_obj_p);
    return ERR_ALL_GOOD;
}
