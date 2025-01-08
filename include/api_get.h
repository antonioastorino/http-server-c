#ifndef API_GET_H
#define API_GET_H
#include "class_string_array.h"
#include "common.h"
#include "http_resp_status.h"

HttpRespStatus api_get_handle_request(const char*, const StringArray*, char*);

#if TEST == 1
void test_api_get(void);
#endif /* TEST == 1 */
#endif /* API_GET_H */
//
