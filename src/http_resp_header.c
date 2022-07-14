#include "class_string_array.h"
#include "fs_utils.h"
#include "http_resp_header.h"

#define HTTP_RESP_VERSION "HTTP/1.0"
#define ASSETS_DIR "assets"
#define WWW_DIR "www"
#define WWW_NOT_FOUND WWW_DIR "/not_found.html"
#define WWW_FORBIDDEN WWW_DIR "/forbidden.html"

typedef enum
{
    dot_html,
    dot_css,
    dot_js,
    dot_jpg,
    dot_ico,
    dot_unrecognized,
    no_extension,
} FileExtension;

typedef struct
{
    const char* extension_char_p;
    char* content_type_char_p;
    FileExtension extension_enum;
} ContentTypeMapItem;

static ContentTypeMapItem content_type_map[] = {
    {
        .extension_char_p    = "html",
        .content_type_char_p = "text/html",
        .extension_enum      = dot_html,
    },
    {
        .extension_char_p    = "css",
        .content_type_char_p = "text/css",
        .extension_enum      = dot_css,
    },
    {
        .extension_char_p    = "jpeg",
        .content_type_char_p = "image/jpeg",
        .extension_enum      = dot_jpg,
    },
    {
        .extension_char_p    = "js",
        .content_type_char_p = "application/javascript",
        .extension_enum      = dot_js,
    },
    {
        .extension_char_p    = "jpg",
        .content_type_char_p = "image/jpeg",
        .extension_enum      = dot_jpg,
    },
    {
        .extension_char_p    = "ico",
        .content_type_char_p = "image/x-icon",
        .extension_enum      = dot_ico,
    },
};

FileExtension match_content_type(const char* path_char_p, char** out_content_type_str_p)
{
    FileExtension ret_file_extension;
    char* dot_char_p = strrchr(path_char_p, '.');
    if (dot_char_p == NULL)
    {
        LOG_TRACE("File without extension detected.");
        *out_content_type_str_p = "";
        return no_extension;
    }
    dot_char_p++; // Skip the dot
    // TODO: convert to lowercase before comparing.
    bool file_extension_matched = false;
    for (size_t i = 0; i < sizeof(content_type_map) / sizeof(ContentTypeMapItem); i++)
    {
        ContentTypeMapItem content_type_item = content_type_map[i];
        if (strcmp(dot_char_p, content_type_item.extension_char_p) == 0)
        {
            *out_content_type_str_p = content_type_item.content_type_char_p;
            ret_file_extension      = content_type_item.extension_enum;
            file_extension_matched  = true;
            break;
        }
    }
    if (!file_extension_matched)
    {
        LOG_WARNING("File extension not recognized.");
        *out_content_type_str_p = "";
        ret_file_extension      = dot_unrecognized;
    }
    LOG_TRACE("Detected content type: `%s`.", *out_content_type_str_p);
    return ret_file_extension;
}

bool make_local_path(const char* location_char_p, char out_local_path_char_p[])
{
    if (strcmp(location_char_p, "/") == 0)
    {
        LOG_TRACE("Homepage requested.");
        sprintf(out_local_path_char_p, "%s/index.html", WWW_DIR);
        return true;
    }
    if (strncmp(location_char_p, "/api", 4) == 0)
    {
        LOG_TRACE("Possible API resource identified");
        sprintf(out_local_path_char_p, "%s", location_char_p);
        return false;
    }
    sprintf(out_local_path_char_p, "%s%s", WWW_DIR, location_char_p);
    return true;
}

void http_get_resp_header_init(
    const HttpReqHeader* http_req_header_p,
    HttpRespHeader* out_http_resp_header_p)
{
    char local_path[PATH_MAX]      = {0};
    out_http_resp_header_p->status = RESP_STATUS_UNDEFINED;
    // Set the file size to zero. This is used in case there is no file to send because not
    // requested or because there was an error.
    out_http_resp_header_p->content_length   = 0;
    StringArray parsed_path_string_array_obj = StringArray_new(http_req_header_p->location, "?");
    const char* location                     = parsed_path_string_array_obj.str_array_char_p[0];
    char resolved_path[PATH_MAX];
    if (make_local_path(location, local_path))
    {
        // It's a path to a file.
        if (realpath(local_path, resolved_path) == NULL)
        {
            LOG_ERROR("Invalid path `%s`", local_path);
            out_http_resp_header_p->status = NOT_FOUND_404;
            memcpy(resolved_path, WWW_NOT_FOUND, sizeof(WWW_NOT_FOUND));
        }
        off_t file_size;
        if (is_err(fs_utils_get_file_size(resolved_path, &file_size)))
        {
            LOG_ERROR("Could not retrieve file size");
            out_http_resp_header_p->status         = FORBIDDEN_403;
            out_http_resp_header_p->content_length = 0;
            memcpy(resolved_path, WWW_FORBIDDEN, sizeof(WWW_FORBIDDEN));
        }
        memcpy(out_http_resp_header_p->actual_location, resolved_path, PATH_MAX);
        out_http_resp_header_p->content_length = file_size;
        match_content_type(resolved_path, &(out_http_resp_header_p->content_type));
        out_http_resp_header_p->status = OK_200;
    }
    // TODO: refactor to destroy in one location.
    StringArray_destroy(&parsed_path_string_array_obj);
    // It could be an API
}

const char* http_resp_reason_phrase(HttpRespStatus http_resp_status)
{
    switch (http_resp_status)
    {
    case OK_200:
        return "200 OK";
    case FORBIDDEN_403:
        return "403 Forbidden";
    case NOT_FOUND_404:
        return "404 Not Found";
    case RESP_STATUS_UNDEFINED:
        return "UNDEFINED";
    default:
        return "INVALID STATUS";
    }
    return "";
}

Error http_resp_header_to_string(
    const HttpRespHeader* http_resp_header_p,
    String* out_resp_header_string_obj_p)
{
    if (http_resp_header_p->status == RESP_STATUS_UNDEFINED)
    {
        LOG_ERROR("Unknown response status");
        out_resp_header_string_obj_p = NULL;
        return ERR_UNDEFINED;
    }
    if (http_resp_header_p->content_length > 0)
    {
        *out_resp_header_string_obj_p = String_new(
            "%s %s\r\n"
            "Content-Length: %lu\r\n"
            "Content-Type: %s\r\n"
            "\r\n",
            HTTP_RESP_VERSION,
            http_resp_reason_phrase(http_resp_header_p->status),
            http_resp_header_p->content_length,
            http_resp_header_p->content_type);
    }
    else
    {
        *out_resp_header_string_obj_p = String_new(
            "%s %s\r\n"
            "\r\n",
            HTTP_RESP_VERSION,
            http_resp_reason_phrase(http_resp_header_p->status));
    }
    return ERR_ALL_GOOD;
}

#if TEST == 1
void test_http_resp_header()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Response string");
    {
        HttpRespStatus http_resp_status = OK_200;
        ASSERT_EQ("200 OK", http_resp_reason_phrase(http_resp_status), "Correct string");
        http_resp_status = FORBIDDEN_403;
        ASSERT_EQ("403 Forbidden", http_resp_reason_phrase(http_resp_status), "Correct string");
        http_resp_status = NOT_FOUND_404;
        ASSERT_EQ("404 Not Found", http_resp_reason_phrase(http_resp_status), "Correct string");
        http_resp_status = RESP_STATUS_UNDEFINED;
        ASSERT_EQ("UNDEFINED", http_resp_reason_phrase(http_resp_status), "Correct string");
        http_resp_status = 10;
        ASSERT_EQ("INVALID STATUS", http_resp_reason_phrase(http_resp_status), "Correct string");
    }
    PRINT_TEST_TITLE("Separate file paths from parameters in URL");
    {
        HttpReqHeader http_req_header;
        ASSERT(
            !is_err(http_req_header_init("GET /index.html?param=val\r\n\r\n", &http_req_header)),
            "HTTP request header created");
        HttpRespHeader http_resp_header;
        http_get_resp_header_init(&http_req_header, &http_resp_header);
        char resolved_path[PATH_MAX];
        realpath("www/index.html", resolved_path);
        ASSERT_EQ(resolved_path, http_resp_header.actual_location, "Actual location correct.");
        ASSERT_EQ(http_resp_header.content_type, "text/html", "Content-Type correct.");
        ASSERT(http_resp_header.content_length > 0, "Content-Length grater than zero.");
    }
    PRINT_TEST_TITLE("Mach file extension")
    {
        char* content_type_char_p;
        ASSERT(
            match_content_type("file_name.html", &content_type_char_p) == dot_html,
            "html matched.");
        ASSERT_EQ(content_type_char_p, "text/html", "Content-Type correct");
        ASSERT(
            match_content_type("file_name.css", &content_type_char_p) == dot_css, "css matched.");
        ASSERT_EQ(content_type_char_p, "text/css", "Content-Type correct");
        ASSERT(match_content_type("file_name.js", &content_type_char_p) == dot_js, "js matched.");
        ASSERT_EQ(content_type_char_p, "application/javascript", "Content-Type correct");
        ASSERT(
            match_content_type("file_name.jpg", &content_type_char_p) == dot_jpg, "jpg matched.");
        ASSERT_EQ(content_type_char_p, "image/jpeg", "Content-Type correct");
        ASSERT(
            match_content_type("file_name.jpeg", &content_type_char_p) == dot_jpg, "jpeg matched.");
        ASSERT_EQ(content_type_char_p, "image/jpeg", "Content-Type correct");
        ASSERT(
            match_content_type("file_name.ico", &content_type_char_p) == dot_ico, "jpeg matched.");
        ASSERT_EQ(content_type_char_p, "image/x-icon", "Content-Type correct");
        ASSERT(
            match_content_type("file_name.something", &content_type_char_p) == dot_unrecognized,
            "unrecognized matched.");
        ASSERT_EQ(content_type_char_p, "", "Content-Type empty.");
        ASSERT(
            match_content_type("file_name-dot-missing", &content_type_char_p) == no_extension,
            "no extension matched.");
        ASSERT_EQ(content_type_char_p, "", "Content-Type empty.");
    }
    PRINT_TEST_TITLE("Make local path")
    {
        char local_path[PATH_MAX];
        ASSERT(make_local_path("/", local_path), "Home path created.");
        ASSERT_EQ(local_path, "www/index.html", "Home path correct.");
        ASSERT(make_local_path("/index.html", local_path), "Home path created.");
        ASSERT_EQ(local_path, "www/index.html", "Home path correct.");
        ASSERT(make_local_path("/anything.something", local_path), "Local path created.");
        ASSERT_EQ(local_path, "www/anything.something", "Local path correct.");
    }
}
#endif
