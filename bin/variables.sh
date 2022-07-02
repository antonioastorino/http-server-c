#!/bin/bash
APP_NAME="tcp-server.app"
ARTIFACT_FOLDER="artifacts"
COMMON_HEADER="include/common.h"
FLAGS="-Wall -Wextra -std=c11"
COMPILER="clang"
LIB="-lc++"
BUILD_DIR="build"
MAKE_FILE="Makefile"
MAIN="main"
SRC_EXTENSIONS=("c")
INC_EXTENSIONS=("h")
FRAMEWORKS=""

HEADER_PATHS=("include")
SRC_PATHS=("src")
