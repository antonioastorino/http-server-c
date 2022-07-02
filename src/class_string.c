#include "class_string.h"
#include "my_memory.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_FACTOR 1.5

bool String_is_null(const String* string_obj_p)
{
    if (string_obj_p == NULL)
        return true;
    if (string_obj_p->str == NULL)
        return true;
    return false;
}

String* String_new(const char* format, ...)
{
    va_list args;
    char* tmp_str_p = NULL;
    String* out_string_obj_p;
    va_start(args, format);
    // Calculate how many bytes are needed (excluding the terminating '\0').
    if (VASPRINTF(&tmp_str_p, format, args) == -1)
    {
        LOG_ERROR("Out of memory", errno)
        exit(ERR_NULL);
    }
    size_t actual_size    = strlen(tmp_str_p);
    // Allocate twice the required length
    size_t allocated_size = (size_t)(actual_size * SIZE_FACTOR);
    // printf("Allocated size: %zu\n", allocated_size);
    tmp_str_p             = (char*)REALLOCF(tmp_str_p, sizeof(char) * allocated_size);
    LOG_TRACE("Created string.")
    va_end(args);
    // Set the `.len` parameter as the length of the string, excluding the terminating '\0'.
    out_string_obj_p         = (String*)MALLOC(sizeof(String));
    out_string_obj_p->str    = tmp_str_p;
    out_string_obj_p->length = actual_size;
    out_string_obj_p->size   = allocated_size;
    return out_string_obj_p;
}

// The array must end with a NULL value.
String* String_join(const char** char_array, const char* joint)
{
    if (!char_array || !char_array[0])
    {
        LOG_ERROR("Please provide a valid input array.");
        return (String*)NULL;
    }
    const char** curr_element = char_array;
    String* out_string_obj_p  = String_new(*curr_element);
    while (*(curr_element + 1) != NULL)
    {
        // TODO: Use String_renew() instead.
        String* new_ret_string_p
            = String_new("%s%s%s", out_string_obj_p->str, joint, *(++curr_element));
        String_destroy(out_string_obj_p);
        out_string_obj_p = new_ret_string_p;
    }
    return out_string_obj_p;
}

String* String_clone(const String* origin) { return String_new(origin->str); }

void String_destroy(String* string_obj_p)
{
    FREE(string_obj_p->str);
    string_obj_p->str    = 0;
    string_obj_p->length = -1;
    string_obj_p->size   = -1;

    FREE(string_obj_p);
}

Error _String_print(const String* string_obj_p)
{
    if (String_is_null(string_obj_p))
    {
        LOG_ERROR("Uninitialized string.");
        return ERR_NULL;
    }
    for (size_t i = 0; i < string_obj_p->length; i++)
    {
        printf("%c", string_obj_p->str[i]);
    }
    fflush(stdout);
    return ERR_ALL_GOOD;
}

Error _String_println(const String* string_obj_p)
{
    Error ret_res = _String_print(string_obj_p);
    return_on_err(ret_res);
    printf("\n");
    return ret_res;
}

bool String_starts_with(const String* string_p, const char* prefix)
{
    if (String_is_null(string_p))
    {
        LOG_ERROR("Trying to check the start of an empty string");
        return false;
    }
    if (strstr(string_p->str, prefix) == string_p->str)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool String_match(const String* a_string_p, const String* b_string_p)
{
    if (a_string_p == NULL || b_string_p == NULL)
    {
        LOG_WARNING("Input strings should not be NULL");
        return false;
    }
    if (strcmp(a_string_p->str, b_string_p->str))
    {
        return false;
    }
    return true;
}

Error String_replace_char(
    String* haystack_string_p,
    const char needle,
    const char replace,
    size_t* out_count)
{
    return_on_err(String_is_null(haystack_string_p)) char tmp_str[haystack_string_p->length + 1];
    size_t i = 0, j = 0, cnt = 0;
    while (i < haystack_string_p->length)
    {
        if (haystack_string_p->str[i] == needle)
        {
            cnt++;
            if (replace != '\0')
            {
                // Replace the current char with that provided.
                tmp_str[j++] = replace;
            }
        }
        else
        {
            tmp_str[j++] = haystack_string_p->str[i];
        }
        i++;
    }
    // Terminate.
    tmp_str[j]                = '\0';
    // Update the string length in case some chars were removed.
    haystack_string_p->length = j;
    strncpy(haystack_string_p->str, tmp_str, j + 1);
    *out_count = cnt;
    return ERR_ALL_GOOD;
}

#define String_replace_pattern_c(suffix)                                                           \
    Error String_replace_pattern_##suffix(                                                         \
        String* haystack_string_p,                                                                 \
        const char* needle,                                                                        \
        const char* format,                                                                        \
        const suffix replacement,                                                                  \
        size_t* out_count)                                                                         \
    {                                                                                              \
        String* replacement_string_p = String_new(format, replacement);                            \
        Error res_replace            = String_replace_pattern(                                     \
            haystack_string_p, needle, replacement_string_p->str, out_count);           \
        String_destroy(replacement_string_p);                                                      \
        return res_replace;                                                                        \
    }

String_replace_pattern_c(size_t);
String_replace_pattern_c(float);
String_replace_pattern_c(int);

Error String_replace_pattern(
    String* haystack_string_p,
    const char* needle,
    const char* replacement,
    size_t* out_count)
{
    if (String_is_null(haystack_string_p))
    {
        LOG_ERROR("Uninitialized string.");
        return ERR_NULL;
    }
    if (strlen(needle) == 0)
    {
        LOG_ERROR("Empty needle not allowed.");
        return ERR_EMPTY_STRING;
    }
    int oldWlen = strlen(needle);
    int newWlen = strlen(replacement);
    size_t i;
    size_t cnt    = 0;

    // Counting the number of times old word occur in the string
    const char* s = haystack_string_p->str;
    for (i = 0; i < haystack_string_p->length; i++)
    {
        if (strstr(&s[i], needle) == &s[i])
        {
            cnt++;

            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }
    if (cnt == 0)
    {
        LOG_DEBUG("Pattern %s not found", needle);
        *out_count = 0;
        return ERR_ALL_GOOD;
    }

    // Making new string of enough length
    size_t new_string_length         = i + cnt * (newWlen - oldWlen);
    char* result_char_p              = (char*)MALLOC(new_string_length + 1);
    result_char_p[new_string_length] = 0;

    i                                = 0;
    while (*s)
    {
        // compare the substring with the result
        if (strstr(s, needle) == s)
        {
            strcpy(&result_char_p[i], replacement);
            i += newWlen;
            s += oldWlen;
        }
        else
            result_char_p[i++] = *s++;
    }

    // Update string.
    if (new_string_length >= haystack_string_p->size)
    {
        // Increase the allocated size.
        haystack_string_p->size = (size_t)(new_string_length * SIZE_FACTOR);
        haystack_string_p->str  = (char*)REALLOCF(haystack_string_p->str, haystack_string_p->size);
    }
    haystack_string_p->length = new_string_length;
    // Copy an extra byte for the NULL characther.
    strncpy(haystack_string_p->str, result_char_p, new_string_length + 1);
    haystack_string_p->str[new_string_length] = 0;
    FREE(result_char_p);
    result_char_p = NULL;

    *out_count    = cnt;
    return ERR_ALL_GOOD;
}

Error String_between_patterns_in_char_p(
    const char* in_char_p,
    const char* prefix,
    const char* suffix,
    String** out_string_obj_pp)
{
    if (in_char_p == NULL)
    {
        LOG_ERROR("Uninitialized input detected.");
        return ERR_NULL;
    }
    else if (strlen(in_char_p) < strlen(prefix) + strlen(suffix))
    {
        LOG_DEBUG("Input str length : %lu", strlen(in_char_p));
        LOG_DEBUG("Prefix length    : %lu", strlen(in_char_p));
        LOG_DEBUG("Suffix length    : %lu", strlen(in_char_p));

        LOG_ERROR("Input shorter than the input patterns.");
        return ERR_INVALID;
    }
    char* start = strstr(in_char_p, prefix);
    if (start == NULL)
    {
        LOG_ERROR("Prefix not found in input string");
        return ERR_NOT_FOUND;
    }
    start     = start + strlen(prefix);
    char* end = strstr(start, suffix);
    if (end == NULL)
    {
        LOG_ERROR("Suffix not found in input string");
        return ERR_NOT_FOUND;
    }
    char* tmp = (char*)MALLOC(end - start + 1);
    memcpy(tmp, start, end - start);
    tmp[end - start]   = '\0';

    *out_string_obj_pp = String_new(tmp);
    FREE(tmp);
    tmp = NULL;
    return ERR_ALL_GOOD;
}

Error String_between_patterns_in_string_p(
    String* in_string_p,
    const char* prefix,
    const char* suffix,
    String** out_string_obj_pp)
{
    return String_between_patterns_in_char_p(in_string_p->str, prefix, suffix, out_string_obj_pp);
}

#if TEST == 1
void test_class_string()
{
    PRINT_BANNER();

    PRINT_TEST_TITLE("New from string")
    {
        const char* str       = "Hello everybody";
        String* test_string_p = String_new(str);
        ASSERT_EQ(test_string_p->length, strlen(str), "Length correct.");
        ASSERT_EQ(test_string_p->size, (size_t)(strlen(str) * 1.5), "Size correct.");
        ASSERT_EQ((int)_String_println(test_string_p), ERR_ALL_GOOD, "Printing functions work.");
        String_destroy(test_string_p);
        ASSERT_EQ(String_is_null(test_string_p), true, "Already destroyed.");
        test_string_p = NULL;
        ASSERT_EQ(String_is_null(test_string_p), true, "String pointer set to NULL.");
    }
    PRINT_TEST_TITLE("New from formatter");
    {
        const char* format1   = "Old string content.";
        String* test_string_p = String_new(format1);
        size_t initial_length = strlen(test_string_p->str);
        size_t initial_size   = (size_t)(initial_length * 1.5);
        ASSERT_EQ(test_string_p->length, initial_length, "Length correct.");
        ASSERT_EQ(test_string_p->size, initial_size, "Size correct.");
    }
    PRINT_TEST_TITLE("starts_with() function");
    {
        String* test_string_p = String_new("Old string content.");
        ASSERT_EQ(
            String_starts_with(test_string_p, "Old string"), true, "starts_with() works when true");
        ASSERT_EQ(
            String_starts_with(test_string_p, "new"), false, "starts_with() works when false");
        ASSERT_EQ(
            String_starts_with(test_string_p, ""),
            true,
            "starts_with() works when needle is empty");
        String_destroy(test_string_p);
    }

    PRINT_TEST_TITLE("clone() function")
    {
        String* test_origin = String_new("Original");
        String* test_clone  = String_clone(test_origin);
        size_t length       = test_origin->length;
        size_t size         = test_origin->size;
        ASSERT_EQ(test_origin->str, test_clone->str, "Strings match.");
        String_destroy(test_origin); // The clone should still be alive.
        ASSERT_EQ(length, test_clone->length, "Size copied");
        ASSERT_EQ(size, test_clone->size, "Size copied");
        String_destroy(test_clone);
    }
    PRINT_TEST_TITLE("replace_char() function - replace with nothing.");
    {
        String* test_string_p = String_new("Some text to be modified.");
        size_t num_of_replacements;
        _String_println(test_string_p);
        ASSERT(
            String_replace_char(test_string_p, ' ', '\0', &num_of_replacements) == ERR_ALL_GOOD,
            "Replacement successful.");
        ASSERT_EQ(test_string_p->str, "Sometexttobemodified.", "Needles removed correctly.");
        ASSERT_EQ(num_of_replacements, 4, "Replacements counted correctly.");
        _String_println(test_string_p);

        ASSERT(
            String_replace_char(test_string_p, 't', '_', &num_of_replacements) == ERR_ALL_GOOD,
            "Replacement successful.");
        ASSERT_EQ(test_string_p->str, "Some_ex__obemodified.", "Needles replaced correctly.");
        _String_println(test_string_p);

        ASSERT(
            String_replace_char(test_string_p, '&', '^', &num_of_replacements) == ERR_ALL_GOOD,
            "Replacement successful");
        ASSERT_EQ(
            test_string_p->str, "Some_ex__obemodified.", "String unchanged - needle not found.");
        ASSERT_EQ(num_of_replacements, 0, "No replacements counted correctly.");
        _String_println(test_string_p);

        String_replace_char(test_string_p, 'S', '+', &num_of_replacements);
        String_replace_char(test_string_p, 'o', '+', &num_of_replacements);
        String_replace_char(test_string_p, 'm', '+', &num_of_replacements);
        String_replace_char(test_string_p, '_', '+', &num_of_replacements);
        String_replace_char(test_string_p, 'e', '+', &num_of_replacements);
        String_replace_char(test_string_p, 'x', '+', &num_of_replacements);
        String_replace_char(test_string_p, 'b', '+', &num_of_replacements);
        String_replace_char(test_string_p, 'd', '+', &num_of_replacements);
        String_replace_char(test_string_p, 'i', '+', &num_of_replacements);
        String_replace_char(test_string_p, 'f', '+', &num_of_replacements);
        String_replace_char(test_string_p, '.', '+', &num_of_replacements);
        ASSERT_EQ(
            test_string_p->str, "+++++++++++++++++++++", "Replaced all chars in string with '+'.");
        _String_println(test_string_p);

        size_t initial_length = test_string_p->length;
        String_replace_char(test_string_p, '+', '\0', &num_of_replacements);
        ASSERT_EQ(test_string_p->str, "", "Ths string is correctly emptied.");
        ASSERT_EQ(test_string_p->length, 0, "Ths length is correctly set to 0.");
        ASSERT_EQ(num_of_replacements, initial_length, "Number of replacements counted correctly.");
        _String_println(test_string_p);
        String_destroy(test_string_p);
    }

    PRINT_TEST_TITLE("Test String_between_patterns()");
    {
        const char* input_char_p = "This string contains a \":pattern:\" to be found";

        {
            String* pattern_string_p;
            ASSERT(
                String_between_patterns(input_char_p, "\":", ":\"", &pattern_string_p)
                    == ERR_ALL_GOOD,
                "String between pattern success.");
            ASSERT_EQ(pattern_string_p->str, "pattern", "Pattern found in C string");
            String_destroy(pattern_string_p);
        }
        {
            String* pattern_string_p;
            String* test_string_p = String_new(input_char_p);
            String_between_patterns(test_string_p, "\":", ":\"", &pattern_string_p);
            ASSERT_EQ(pattern_string_p->str, "pattern", "Pattern found String Object");
            String_destroy(pattern_string_p);
        }
        {
            String* pattern_string_p;
            String* test_string_p = String_new(input_char_p);
            Error res_pattern_not_found
                = String_between_patterns(test_string_p, "--", ":\"", &pattern_string_p);
            ASSERT(res_pattern_not_found == ERR_NOT_FOUND, "Prefix not found");

            res_pattern_not_found
                = String_between_patterns(test_string_p, "\":", "--", &pattern_string_p);
            ASSERT(res_pattern_not_found == ERR_NOT_FOUND, "Suffix not found");

            String_destroy(test_string_p);
        }
    }
    PRINT_TEST_TITLE("Pattern replacement with empty string");
    {
        size_t num_of_replacements;
        String* test_string_p = String_new("This pattern contains \\r\\n to be removed");
        String_replace_pattern(test_string_p, "\\r\\n ", "", &num_of_replacements);
        ASSERT_EQ("This pattern contains to be removed", test_string_p->str, "Pattern deleted");
        ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
        String_destroy(test_string_p);
    }
    PRINT_TEST_TITLE("Pattern replacement with short string");
    {
        size_t num_of_replacements;
        String* test_string_p = String_new("This pattern contains \\r\\n to be replaced");
        String_replace_pattern(test_string_p, "\\r\\n", "HELLO WORLD", &num_of_replacements);
        ASSERT_EQ(
            "This pattern contains HELLO WORLD to be replaced",
            test_string_p->str,
            "Pattern replaced");
        ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
        String_destroy(test_string_p);
    }
    PRINT_TEST_TITLE("Pattern replacement with float");
    {
        size_t num_of_replacements;
        String* test_string_p = String_new("This pattern contains \\r\\n to be replaced");
        String_replace_pattern_with_format(
            test_string_p, "\\r\\n", "%.4f", 10.3f, &num_of_replacements);
        ASSERT_EQ(
            "This pattern contains 10.3000 to be replaced", test_string_p->str, "Pattern replaced");
        ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
        String_destroy(test_string_p);
    }
    PRINT_TEST_TITLE("Pattern replacement with size_t");
    {
        size_t num_of_replacements;
        String* test_string_p = String_new("This pattern contains \\r\\n to be replaced");
        String_replace_pattern_with_format(
            test_string_p, "\\r\\n", "%lu", (size_t)1003, &num_of_replacements);
        ASSERT_EQ(
            "This pattern contains 1003 to be replaced", test_string_p->str, "Pattern replaced");
        ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
        String_destroy(test_string_p);
    }

    PRINT_TEST_TITLE("Pattern replacement with int");
    {
        size_t num_of_replacements;
        String* test_string_p = String_new("This pattern contains \\r\\n to be replaced");
        String_replace_pattern_with_format(
            test_string_p, "\\r\\n", "%d", -1003, &num_of_replacements);
        ASSERT_EQ(
            "This pattern contains -1003 to be replaced", test_string_p->str, "Pattern replaced");
        ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
        String_destroy(test_string_p);
    }
    PRINT_TEST_TITLE("Pattern replacement with long string");
    {
        size_t num_of_replacements;
        String* test_string_p = String_new("This pattern contains \\r\\n to be replaced");
        String_replace_pattern(
            test_string_p,
            "\\r\\n",
            "HELLO WORLD! This is the replacement pattern",
            &num_of_replacements);
        ASSERT_EQ(
            "This pattern contains HELLO WORLD! This is the replacement pattern to be replaced",
            test_string_p->str,
            "Pattern replaced");
        ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
        String_destroy(test_string_p);
    }
    PRINT_TEST_TITLE("Pattern not replaced because missing");
    {
        size_t num_of_replacements;
        String* test_string_p = String_new("This string does not contain a pattern to be replaced");
        num_of_replacements   = String_replace_pattern(
            test_string_p,
            "missing pattern",
            "HELLO WORLD! This is the replacement pattern",
            &num_of_replacements);
        ASSERT_EQ(
            "This string does not contain a pattern to be replaced",
            test_string_p->str,
            "Pattern not found");
        ASSERT_EQ(num_of_replacements, 0, "Number of replacements counted correctly.");
        String_destroy(test_string_p);
    }
    PRINT_TEST_TITLE("Trying to join an empty array");
    {
        const char* char_array[] = {NULL};
        String* test_string_p    = String_join(char_array, "hello");
        ASSERT(test_string_p == NULL, "No string created");
    }

    PRINT_TEST_TITLE("Join with 1 element");
    {
        const char* one_element_array[] = {"element 1", NULL};
        String* test_string_p           = String_join(one_element_array, "hello");
        ASSERT_EQ(test_string_p->str, "element 1", "No concatenation performed");
        String_destroy(test_string_p);
    }

    PRINT_TEST_TITLE("Join with 2 elements");
    {
        const char* two_element_array[] = {"element 1", "element 2", NULL};
        String* test_string_p           = String_join(two_element_array, "|||");
        ASSERT_EQ(test_string_p->str, "element 1|||element 2", "Concatenation correct");
        String_destroy(test_string_p);
    }
    PRINT_TEST_TITLE("Join with empty separator");
    {
        const char* foo_bar_element_array[] = {"F", "O", "O", "B", "A", "R", NULL};
        String* test_string_p               = String_join(foo_bar_element_array, "");
        ASSERT_EQ(test_string_p->str, "FOOBAR", "Concatenation correct");
        String_destroy(test_string_p);
    }
}
#endif
