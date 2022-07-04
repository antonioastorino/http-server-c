#!/bin/bash
APP_NAME="tcp-server.app"
ARTIFACT_FOLDER="test/artifacts"
LOG_FILE_ERR="${ARTIFACT_FOLDER}/err.log"
COMMON_HEADER="include/common.h"
FLAGS="-Wall -Wextra -std=c11"
COMPILER="clang"
LIB=""
BUILD_DIR="build"
MAKE_FILE="Makefile"
MAIN="main"
SRC_EXTENSIONS=("c")
INC_EXTENSIONS=("h")
FRAMEWORKS=""

HEADER_PATHS=("include")
SRC_PATHS=("src")
