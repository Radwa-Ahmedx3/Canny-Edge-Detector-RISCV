# Compilers
HOST_CXX := g++
RV_CXX   := riscv64-unknown-elf-g++

# GoogleTest
GTEST_INC := $(HOME)/gtest-local/include
GTEST_LIB := $(HOME)/gtest-local/lib

# Dirs
BUILD_HOST := build/host
BUILD_RV   := build/rv

# Flags
HOST_FLAGS := -std=c++17 -I$(GTEST_INC)
HOST_LIBS  := -L$(GTEST_LIB) -lgtest -lgtest_main -pthread
RV_FLAGS   := -std=c++17 -march=rv64gcv -mabi=lp64d

# QEMU
VLEN ?= 256

.PHONY: all test canny_rv run clean

all: test canny_rv

test:
	$(HOST_CXX) $(HOST_FLAGS) tests/test_hello.cpp -o $(BUILD_HOST)/test_hello $(HOST_LIBS)
	./$(BUILD_HOST)/test_hello

canny_rv:
	@echo "src/ is empty for now - will build in Phase 2"

run:
	qemu-riscv64 -cpu rv64,v=true,vlen=$(VLEN) ./$(BUILD_RV)/canny_rv

clean:
	rm -rf build/host/* build/rv/*
