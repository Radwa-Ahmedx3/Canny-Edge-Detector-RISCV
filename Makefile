# =========================
# Compilers
# =========================

HOST_CXX = g++
RV_CXX = riscv64-unknown-elf-g++

# =========================
# Flags
# =========================

HOST_FLAGS = -pthread

RV_FLAGS = -march=rv64gcv -mabi=lp64d -static

# =========================
# GoogleTest Paths
# =========================

GTEST_INC = ./third_party/googletest/googletest/include
GTEST_LIB = ./third_party/googletest/build_host/lib

# =========================
# Targets
# =========================

all: host rvv

# ---------- Host Test ----------

host:
	$(HOST_CXX) tests/host_test.cpp \
	-I$(GTEST_INC) \
	-L$(GTEST_LIB) \
	-lgtest -lgtest_main \
	$(HOST_FLAGS) \
	-o host_test

# ---------- RVV Test ----------

rvv:
	$(RV_CXX) $(RV_FLAGS) tests/my_rvv_test.cpp -o my_rvv_test

# ---------- Run Host ----------

run-host:
	./host_test

# ---------- Run RVV ----------

run-rvv:
	qemu-riscv64 ./my_rvv_test

# ---------- Clean ----------

clean:
	rm -f host_test my_rvv_test
