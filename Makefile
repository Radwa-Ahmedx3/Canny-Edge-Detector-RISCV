# ==========================================
# 1. Compilers & Toolchain
# ==========================================
TOOLCHAIN := /home/radwa-ahmed/riscv-toolchain/bin/riscv64-unknown-elf-
CXX       := $(TOOLCHAIN)g++

# ==========================================
# 2. Paths & Flags
# ==========================================
GTEST_DIR  := ./third_party/googletest/install
GTEST_INC  := $(GTEST_DIR)/include
GTEST_LIB  := $(GTEST_DIR)/lib/libgtest.a
BUILD_RV   := build/rv

RV_FLAGS   := -march=rv64gcv -mabi=lp64d -static -fno-exceptions -fno-rtti -I./include -I$(GTEST_INC)
RV_DEFINES := -DGTEST_HAS_PTHREAD=0 -DGTEST_HAS_POSIX_RE=0

# ==========================================
# 3. Targets
# ==========================================

.PHONY: all clean run_all

all: hello_test rvv_test

hello_test:
	mkdir -p $(BUILD_RV)
	$(CXX) $(RV_FLAGS) $(RV_DEFINES) tests/test_hello.cpp $(GTEST_LIB) -o $(BUILD_RV)/test_hello

rvv_test:
	mkdir -p $(BUILD_RV)
	$(CXX) $(RV_FLAGS) $(RV_DEFINES) tests/rvv_test.cpp $(GTEST_LIB) -o $(BUILD_RV)/rvv_test

run_all: hello_test rvv_test
	@echo "--- Step 1 & 2: Running Infrastructure Tests ---"
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./$(BUILD_RV)/test_hello
	@echo "--- Step 3 & 4: Running Vector (RVV) Logic Tests ---"
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./$(BUILD_RV)/rvv_test

clean:
	rm -rf build/
