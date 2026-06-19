# =========================
# Compilers
# =========================
HOST_CXX = g++
RV_CXX   = riscv64-unknown-elf-g++

# =========================
# Flags
# =========================
HOST_FLAGS = -I include
RV_FLAGS   = -march=rv64gcv -mabi=lp64d -static -I include

# =========================
# Source Files
# =========================
SRCS = syscalls.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp
RVV_SRCS = src/gaussian_rvv.cpp src/sobel_rvv.cpp

# =========================
# GoogleTest
# =========================
GTEST_INC = $(HOME)/gtest-install/include
GTEST_LIB = $(HOME)/gtest-install/lib

# =========================
# Default Target
# =========================
all: host canny_rv

# ---------- Host Pipeline ----------
host:
	$(HOST_CXX) $(HOST_FLAGS) src/main.cpp $(SRCS) -o build-host/canny_host

# ---------- Host GoogleTest ----------
test:
	$(HOST_CXX) $(HOST_FLAGS) -I$(GTEST_INC) \
		tests/pipeline_test.cpp $(SRCS) \
		-L$(GTEST_LIB) -lgtest -lgtest_main -lpthread \
		-o build-host/pipeline_tests
	./build-host/pipeline_tests

# ---------- RISC-V Equivalence Tests ----------
	test_rvv_equiv:
	$(RV_CXX) $(RV_FLAGS) src/test_equivalence.cpp $(SRCS) $(RVV_SRCS) -o build-rv/test_equiv
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./build-rv/test_equiv

# ---------- RISC-V Pipeline ----------
canny_rv:
	$(RV_CXX) $(RV_FLAGS) src/main_qemu.cpp $(SRCS) -o build-rv/canny_rv

# ---------- Run on QEMU ----------
run:
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./build-rv/canny_rv

# ---------- Compiler Optimization Sweep ----------
canny_O0:
	$(RV_CXX) $(RV_FLAGS) -O0 src/main_qemu.cpp $(SRCS) -o build-rv/canny_O0

canny_O2:
	$(RV_CXX) $(RV_FLAGS) -O2 src/main_qemu.cpp $(SRCS) -o build-rv/canny_O2

canny_O3:
	$(RV_CXX) $(RV_FLAGS) -O3 src/main_qemu.cpp $(SRCS) -o build-rv/canny_O3

canny_Os:
	$(RV_CXX) $(RV_FLAGS) -Os src/main_qemu.cpp $(SRCS) -o build-rv/canny_Os

canny_Ofast:
	$(RV_CXX) $(RV_FLAGS) -Ofast src/main_qemu.cpp $(SRCS) -o build-rv/canny_Ofast

sweep: canny_O0 canny_O2 canny_O3 canny_Os canny_Ofast

# ---------- RVV Intrinsics ----------
rvv_timing:
	$(RV_CXX) $(RV_FLAGS) -O3 src/main_rvv_timing.cpp $(SRCS) $(RVV_SRCS) \
		-o build-rv/timing_rvv

rvv_test:
	$(RV_CXX) $(RV_FLAGS) -O3 src/test_rvv_equivalence.cpp $(SRCS) $(RVV_SRCS) \
		-o build-rv/test_rvv

# ---------- Clean ----------
clean:
	rm -f build-host/* build-rv/* *.raw *.png

# ---------- RISC-V Equivalence Tests ----------
test_rvv_equiv:
	$(RV_CXX) $(RV_FLAGS) src/test_equivalence.cpp $(SRCS) $(RVV_SRCS) -o build-rv/test_equiv
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./build-rv/test_equiv

# Test with RISC-V syscalls
test_risc_v:
	$(RV_CXX) $(RV_FLAGS) src/main_risc_v_test.cpp $(SRCS) $(RVV_SRCS) -o build-rv/test_risc_v
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./build-rv/test_risc_v
