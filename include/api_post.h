#ifndef API_POST_H
#define API_POST_H
#include "common.h"
#include "http_resp_status.h"

HttpRespStatus api_post_handle_request(const char*, const char*, char*);

#if TEST == 1
void test_api_post();
#endif /* TEST == 1 */
#endif /* API_POST_H */
