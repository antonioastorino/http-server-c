#ifndef HTTP_H
#define HTTP_H
#include "common.h"
 
typedef enum {
    VERB_GET,
    VERB_POST,
    VERB_UNDEFINED,
} request_verb;

#if TEST == 1
void test_http();
#endif /* TEST == 1 */
#endif /* HTTP_H */

