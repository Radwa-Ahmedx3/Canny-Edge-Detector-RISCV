# RISC-V GTest
GTEST_DIR = /home/radwa-ahmed/googletest/install
TOOLCHAIN = /home/radwa-ahmed/riscv-toolchain/bin/riscv64-unknown-elf-

CXX = $(TOOLCHAIN)g++
CC  = $(TOOLCHAIN)gcc

# Flags
CXXFLAGS = -march=rv64gcv -mabi=lp64d -static -I./include -I$(GTEST_DIR)/include


LDFLAGS = -L$(GTEST_DIR)/lib -lgtest -static
