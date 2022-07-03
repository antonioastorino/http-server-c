#ifndef HTTP_H
#define HTTP_H
#include "common.h"
#include "class_string.h"

typedef enum
{
    VERB_GET,
    VERB_POST,
    VERB_PUT,
    VERB_DELETE,
    VERB_UNKNOWN,
} HttpMethod;

#if TEST == 1
void test_http();
#endif /* TEST == 1 */
#endif /* HTTP_H */
