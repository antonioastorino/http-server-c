#include "http.h"

request_verb parse_verb(const char* str_p) { return VERB_GET; }

#if TEST == 1
void test_http()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Parsing verb");
    {
//        request_verb rqv = parse_verb("GET /");
    }
}
#endif /* TEST == 1 */
