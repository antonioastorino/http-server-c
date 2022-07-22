#!/bin/bash
APP_NAME="http-server.app"
ARTIFACT_FOLDER="test/artifacts"
LOG_FILE_ERR="${ARTIFACT_FOLDER}/err.log"
COMMON_HEADER="mylibc/include/common.h"
COMMON_ERROR="mylibc/include/error.h"
APP_ERROR="include/app_error.h"
FLAGS="-Wall -Wextra -std=c11 -include ${APP_ERROR}"
COMPILER="clang"
LIB=""
BUILD_DIR="build"
MAKE_FILE="Makefile"
MAIN="main"
SRC_EXTENSIONS=("c")
INC_EXTENSIONS=("h")
FRAMEWORKS=""
SKIP_SRC="mylibc/src/test.c"
HEADER_PATHS=("include" "mylibc/include")
SRC_PATHS=("src" "mylibc/src")
if [ "$(uname -s)" = "Linux" ]; then
    FLAGS="${FLAGS} -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_GNU_SOURCE"
fi
