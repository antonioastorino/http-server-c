# Makefile auto generated using custom generator
CFLAGS=-Wall -Wextra -std=c11
MAINFLAGS=-Wall -Wextra -std=c++1z
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
	make -C "$(BD)" OPT=$(OPT) build/tcp-server-test; \
	else \
	[ `grep -c '^#define TEST 1' "$(BD)"/include/common.h` -eq 1 ] && \
	sed -i.bak 's/^#define TEST 1/#define TEST 0/g' "$(BD)"/include/common.h; \
	make -C "$(BD)" OPT=$(OPT) build/tcp-server; \
	fi

build/tcp-server:\
	build/common.o \
	build/main.o 
	clang $(LIB) $(MAINFLAGS) -O$(OPT) $(INC) $(FRAMEWORKS) $^ -o $@

build/common.o: src/common.c \
	include/common.h 
	gcc $(INC) $(CFLAGS) -O$(OPT) -c $< -o $@


build/main.o: src/main.c \
	include/common.h \
	build/common.o 
	gcc $(INC) $(CFLAGS) -O$(OPT) -c $< -o $@

