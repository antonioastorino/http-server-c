#!/bin/bash
APP_NAME="http-server"
ARTIFACT_FOLDER="test/artifacts"
LOG_FOLDER="${ARTIFACT_FOLDER}/logs"
LOG_FILE_ERR="${LOG_FOLDER}/err.log"
COMMON_HEADER="mylibc/include/common.h"
COMMON_ERROR="mylibc/include/error.h"
APP_ERROR="include/app_error.h"
COMPILER="clang"
LIB=""
BUILD_DIR="build"
MAKE_FILE="Makefile"
MAIN="main"
SRC_EXTENSIONS=("c")
INC_EXTENSIONS=("h")
FRAMEWORKS=""
SKIP_SRC="mylibc/src/main-test.c"
HEADER_PATHS=("include" "mylibc/include")
SRC_PATHS=("src" "mylibc/src")
FLAGS=(
    -Wall
    -Wextra
    -std=c11
    -pedantic
    -include ${APP_ERROR}
    -DLOG_LEVEL=5
)
if [ "$(uname -s)" = "Linux" ]; then
    FLAGS+=(
        -D_BSD_SOURCE
        -D_DEFAULT_SOURCE
        -D_GNU_SOURCE
    )
fi
