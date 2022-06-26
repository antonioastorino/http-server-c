#!/bin/bash
APP_NAME="tcp-server"
COMMON_HEADER="include/common.h"
CFLAGS="-Wall -Wextra -std=c11"
MAINFLAGS="-Wall -Wextra -std=c++1z"
GLOBAL_COMPILER="clang"
LIB="-lc++"
BUILD_DIR="build"
MAKE_FILE="Makefile"
MAIN="main"
SRC_EXTENSIONS=("c")
INC_EXTENSIONS=("h")
FRAMEWORKS=""

HEADER_PATHS=("include")
SRC_PATHS=("src")
