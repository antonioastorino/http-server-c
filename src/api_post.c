#include "api_post.h"
#include "class_json.h"
#include "class_string_array.h"
#include "converter.h"
#include "fs_utils.h"
#include <stdlib.h>     /* for getenv()  and exit() */
#include <sys/fcntl.h>  /* for open() */
#include <sys/stat.h>   /* for stat() */
#include <sys/types.h>  /* for stat() */
#include <sys/unistd.h> /* for stat() */
#include <sys/wait.h>   /* contains waitpid() */

typedef HttpRespStatus(api_post_function)(const char*, char*);

typedef struct
{
    char* file_id;
    char* file_path;
} ApiPostLogFileMapping;

typedef struct
{
    const char* uri_char_p;
    const char* feature_file_name;
    api_post_function* function_p;
} ApiPostValidUri;

typedef enum
{
    TYPE_FILE,
    TYPE_FOLDER,
} ApiPostEntryType;

typedef struct
{
    const char* entry_name;
    const ApiPostEntryType entry_type;
} ApiPostValidationEntry;

api_post_function _api_post_example;

static const ApiPostValidUri valid_post_uri_vec[] = {
    {
        .uri_char_p        = "/api/example",
        .function_p        = _api_post_example,
    },
};

api_post_function* _api_post_is_valid_uri(const char* uri)
{
    for (size_t i = 0; i < sizeof(valid_post_uri_vec) / sizeof(ApiPostValidUri); i++)
    {
        if (strcmp(uri, valid_post_uri_vec[i].uri_char_p) == 0)
        {
            return valid_post_uri_vec[i].function_p;
        }
    }
    return NULL;
}

HttpRespStatus api_post_handle_request(
    const char* uri_char_p,
    const char* req_body_char_p,
    char* out_file_path_char_p)
{
    HttpRespStatus (*request_handler)(const char*, char*);
    if (!(request_handler = _api_post_is_valid_uri(uri_char_p)))
    {
        LOG_ERROR("Invalid POST URI `%s`", uri_char_p);
        return NOT_FOUND_404;
    }
    return request_handler(req_body_char_p, out_file_path_char_p);
}

HttpRespStatus _api_post_example(const char* file_content_char_p, char* out_file_path_char_p)
{
    LOG_TRACE("Executing POST API example.");
    UNUSED(file_content_char_p);
    UNUSED(out_file_path_char_p);
    return NO_CONTENT_204;
}

#if TEST == 1
void test_api_post()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Test POST api");
    {
        ASSERT(_api_post_is_valid_uri("/api/example") == _api_post_example, "Valid URI found.");
    }
}
#endif /* TEST == 1 */
