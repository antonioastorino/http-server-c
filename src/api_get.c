#include "api_get.h"
#include "class_string_array.h"
#include "converter.h"
#include "fs_utils.h"
#include <fcntl.h>
#include <stdlib.h> /* for getenv()  and exit() */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h> /* contains waitpid() */

typedef HttpRespStatus(api_get_function)(const StringArray*, char*);

typedef struct
{
    const char* uri_char_p;
    api_get_function* function_p;
} ApiGetValidUri;

api_get_function _api_get_example_with_content;
api_get_function _api_get_example_with_no_content;

static const ApiGetValidUri valid_get_uri_vec[] = {
    {
        .uri_char_p = "/api/example-with-content",
        .function_p = _api_get_example_with_content,
    },
    {
        .uri_char_p = "/api/example-with-no-content",
        .function_p = _api_get_example_with_no_content,
    },
};

api_get_function* _api_get_is_valid_uri(const char* uri)
{
    for (size_t i = 0; i < sizeof(valid_get_uri_vec) / sizeof(ApiGetValidUri); i++)
    {
        if (strcmp(uri, valid_get_uri_vec[i].uri_char_p) == 0)
        {
            return valid_get_uri_vec[i].function_p;
        }
    }
    return NULL;
}

HttpRespStatus api_get_handle_request(
    const char* uri_char_p,
    const StringArray* req_params_string_array_p,
    char* out_file_path_char_p)
{
    HttpRespStatus (*request_handler)(const StringArray*, char*);
    if (!(request_handler = _api_get_is_valid_uri(uri_char_p)))
    {
        LOG_ERROR("Invalid GET URI `%s`", uri_char_p);
        return NOT_FOUND_404;
    }
    return request_handler(req_params_string_array_p, out_file_path_char_p);
}

HttpRespStatus _api_get_example_with_content(
    const StringArray* req_params_string_array_p,
    char* out_file_path_char_p)
{
    LOG_TRACE("Executing GET API example")
    UNUSED(req_params_string_array_p);
    strcpy(out_file_path_char_p, "assets/example.txt");
    return OK_200;
}

HttpRespStatus _api_get_example_with_no_content(
    const StringArray* req_params_string_array_p,
    char* out_file_path_char_p)
{
    LOG_TRACE("Reading mapping file.")
    UNUSED(req_params_string_array_p);
    UNUSED(out_file_path_char_p);
    return NO_CONTENT_204;
}

#if TEST == 1
void test_api_get(void)
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Test api GET");
    {
        ASSERT(_api_get_is_valid_uri("/invalid-uri") == NULL, "Invalid URI found.");
        ASSERT(
            _api_get_is_valid_uri("/api/example-with-no-content") == _api_get_example_with_no_content,
            "Valid URI found.");
    }
    PRINT_TEST_TITLE("_api_get_example_with_content");
    {
        char* expected_file_path = "assets/example.txt";
        char file_path[PATH_MAX] = {0};
        ASSERT(
            _api_get_is_valid_uri("/api/example-with-content") == _api_get_example_with_content,
            "Valid URI found.");
        ASSERT(_api_get_example_with_content(NULL, file_path) == OK_200, "OK 200 received");
        ASSERT_EQ(expected_file_path, file_path, "File path retrieved correctly");
    }
}
#endif /* TEST == 1 */
