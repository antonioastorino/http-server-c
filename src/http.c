#include "http.h"

HttpMethod http_parse_method(const String* method_string_obj_p)
{
    if (0 == strncmp(method_string_obj_p->str, "GET", method_string_obj_p->length))
    {
        return VERB_GET;
    }
    if (0 == strncmp(method_string_obj_p->str, "POST", method_string_obj_p->length))
    {
        return VERB_POST;
    }
    if (0 == strncmp(method_string_obj_p->str, "PUT", method_string_obj_p->length))
    {
        return VERB_PUT;
    }
    if (0 == strncmp(method_string_obj_p->str, "DELETE", method_string_obj_p->length))
    {
        return VERB_DELETE;
    }

    return VERB_UNKNOWN;
}

#if TEST == 1
void test_http()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Parsing method - pass");
    {
        HttpMethod rqv;
        String* get_string_obj_p     = String_new("GET");
        String* post_string_obj_p    = String_new("POST");
        String* put_string_obj_p     = String_new("PUT");
        String* delete_string_obj_p  = String_new("DELETE");
        String* invalid_string_obj_p = String_new("something");

        rqv                          = http_parse_method(get_string_obj_p);
        ASSERT(rqv == VERB_GET, "GET request understood");
        rqv = http_parse_method(post_string_obj_p);
        ASSERT(rqv == VERB_POST, "POST request understood");
        rqv = http_parse_method(put_string_obj_p);
        ASSERT(rqv == VERB_PUT, "PUT request understood");
        rqv = http_parse_method(delete_string_obj_p);
        ASSERT(rqv == VERB_DELETE, "DELETE request understood");
        rqv = http_parse_method(invalid_string_obj_p);
        ASSERT(rqv == VERB_UNKNOWN, "Invalid method or syntax error.");
        String_destroy(get_string_obj_p);
        String_destroy(post_string_obj_p);
        String_destroy(put_string_obj_p);
        String_destroy(delete_string_obj_p);
        String_destroy(invalid_string_obj_p);
    }
    /*
        }*/
}
#endif /* TEST == 1 */
