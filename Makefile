# Makefile auto generated using custom generator
FLAGS=-Wall -Wextra -std=c11
BD=/Users/antonio/my-projects/CWS/bin/..
OPT ?= 0
LIB=-lc++
INC= -Iinclude \

.PHONY: all setup

all: setup

setup:
	@/bin/rm -rf tcp-server.app
	@mkdir -p \
	build
	@if [ "$(MODE)" = "TEST" ]; then \
	[ `grep -c '^#define TEST 0' "$(BD)"/include/common.h` -eq 1 ] && \
	sed -i.bak 's/^#define TEST 0/#define TEST 1/g' "$(BD)"/include/common.h; \
	make -C "$(BD)" OPT=$(OPT) build/tcp-server.app; \
	else \
	[ `grep -c '^#define TEST 1' "$(BD)"/include/common.h` -eq 1 ] && \
	sed -i.bak 's/^#define TEST 1/#define TEST 0/g' "$(BD)"/include/common.h; \
	make -C "$(BD)" OPT=$(OPT) build/tcp-server.app; \
	fi

build/tcp-server.app:\
	build/json.o \
	build/common.o \
	build/string_array.o \
	build/my_memory.o \
	build/logger.o \
	build/my_string.o \
	build/main.o \
	build/error.o \
	build/http.o \
	build/fs_utils.o \
	build/converter.o 
	clang $(LIB) $(FLAGS) -O$(OPT) $(INC) $(FRAMEWORKS) $^ -o $@

build/json.o: src/json.c \
	include/common.h \
	build/common.o \
	include/converter.h \
	build/converter.o \
	include/json.h \
	include/logger.h \
	build/logger.o \
	include/my_memory.h \
	build/my_memory.o 
	clang $(INC) $(FLAGS) -O$(OPT) -c $< -o $@


build/common.o: src/common.c \
	include/common.h 
	clang $(INC) $(FLAGS) -O$(OPT) -c $< -o $@


build/string_array.o: src/string_array.c \
	include/string_array.h \
	include/common.h \
	build/common.o \
	include/logger.h \
	build/logger.o \
	include/my_memory.h \
	build/my_memory.o 
	clang $(INC) $(FLAGS) -O$(OPT) -c $< -o $@


build/my_memory.o: src/my_memory.c \
	include/common.h \
	build/common.o \
	include/my_memory.h 
	clang $(INC) $(FLAGS) -O$(OPT) -c $< -o $@


build/logger.o: src/logger.c \
	include/logger.h 
	clang $(INC) $(FLAGS) -O$(OPT) -c $< -o $@


build/my_string.o: src/my_string.c \
	include/logger.h \
	build/logger.o \
	include/my_memory.h \
	build/my_memory.o \
	include/my_string.h 
	clang $(INC) $(FLAGS) -O$(OPT) -c $< -o $@


build/main.o: src/main.c \
	include/common.h \
	build/common.o \
	include/http.h \
	build/http.o 
	clang $(INC) $(FLAGS) -O$(OPT) -c $< -o $@


build/error.o: src/error.c \
	include/error.h 
	clang $(INC) $(FLAGS) -O$(OPT) -c $< -o $@


build/http.o: src/http.c \
	include/http.h 
	clang $(INC) $(FLAGS) -O$(OPT) -c $< -o $@


build/fs_utils.o: src/fs_utils.c \
	include/common.h \
	build/common.o \
	include/fs_utils.h \
	include/logger.h \
	build/logger.o \
	include/my_memory.h \
	build/my_memory.o \
	include/my_string.h \
	build/my_string.o 
	clang $(INC) $(FLAGS) -O$(OPT) -c $< -o $@


build/converter.o: src/converter.c \
	include/common.h \
	build/common.o \
	include/converter.h \
	include/logger.h \
	build/logger.o 
	clang $(INC) $(FLAGS) -O$(OPT) -c $< -o $@

