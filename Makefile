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
# ---------- Canny Pipeline ----------
canny_rv:
	$(RV_CXX) $(RV_FLAGS) \
	src/main_scalar.cpp \
	src/image.cpp \
	src/gaussian.cpp \
	src/sobel.cpp \
	src/nms.cpp \
	src/hysteresis.cpp \
	src/syscalls.cpp \
	-I include \
	-o canny_pipeline

run:
	qemu-riscv64 ./canny_pipeline
# ---------- Clean ----------

clean:
	rm -f host_test my_rvv_test canny_pipeline \
	canny_O0 canny_O2 canny_O3 canny_Os canny_Ofast \
	canny_O0_rvv canny_O2_rvv canny_O3_rvv canny_Os_rvv canny_Ofast_rvv \
	canny_O3_vecinfo canny_fp_test canny_lmul1 canny_lmul2 canny_lmul4 equivalence_test \
	phase3_test vec_report.txt

# ---------- Phase 4: Optimization Sweep ----------
canny_O0:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -O0 \
	src/main_scalar.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_O0

canny_O2:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -O2 \
	src/main_scalar.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_O2

canny_O3:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -O3 \
	src/main_scalar.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_O3

canny_Os:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -Os \
	src/main_scalar.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_Os

canny_Ofast:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -Ofast \
	src/main_scalar.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_Ofast

sweep: canny_O0 canny_O2 canny_O3 canny_Os canny_Ofast


# ---------- Phase 6: RVV Optimization Sweep ----------
canny_O0_rvv:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -O0 \
	src/main.cpp src/image.cpp src/gaussian_rvv.cpp src/sobel_rvv.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_O0_rvv

canny_O2_rvv:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -O2 \
	src/main.cpp src/image.cpp src/gaussian_rvv.cpp src/sobel_rvv.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_O2_rvv

canny_O3_rvv:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -O3 \
	src/main.cpp src/image.cpp src/gaussian_rvv.cpp src/sobel_rvv.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_O3_rvv

canny_Os_rvv:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -Os \
	src/main.cpp src/image.cpp src/gaussian_rvv.cpp src/sobel_rvv.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_Os_rvv

canny_Ofast_rvv:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -Ofast \
	src/main.cpp src/image.cpp src/gaussian_rvv.cpp src/sobel_rvv.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_Ofast_rvv

sweep_rvv: canny_O0_rvv canny_O2_rvv canny_O3_rvv canny_Os_rvv canny_Ofast_rvv

# ---------- VLEN sweep (uses the -Os RVV binary) ----------
vlen-sweep: canny_Os_rvv
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./canny_Os_rvv
	qemu-riscv64 -cpu rv64,v=true,vlen=256 ./canny_Os_rvv
	qemu-riscv64 -cpu rv64,v=true,vlen=512 ./canny_Os_rvv

# ---------- Test Input Generation ----------
# Usage: make test_input.raw IMG=path/to/your_image.jpg
IMG ?= real_image.jpg
test_input.raw:
	python3 scripts/img_to_raw.py $(IMG) test_input.raw

# ---------- Setup (for first-time newcomers) ----------
setup: test_input.raw
	@echo "Setup complete: test_input.raw generated from $(IMG)"

.PHONY: all host rvv run-host run-rvv canny_rv run clean sweep sweep_rvv vlen-sweep setup

# ---------- Auto-vectorization Report (-O3) ----------
canny_O3_vecinfo:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -O3 -fopt-info-vec-all \
	src/main_scalar.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_O3_vecinfo 2> vec_report.txt
	@echo "Auto-vectorization report written to vec_report.txt"

# ---------- Binary Size Comparison ----------
binary-sizes: sweep sweep_rvv canny_fp_test
	@echo "Binary sizes (scalar vs RVV, across optimization levels):"
	@ls -lh canny_O0 canny_O2 canny_O3 canny_Os canny_Ofast \
	        canny_O0_rvv canny_O2_rvv canny_O3_rvv canny_Os_rvv canny_Ofast_rvv \
	        canny_fp_test \
	        2>/dev/null | awk '{print $$5, $$NF}'

.PHONY: canny_O3_vecinfo binary-sizes

# ---------- Fixed-Point vs Exact Division Benchmark ----------
canny_fp_test:
	$(RV_CXX) $(RV_FLAGS) src/main_fp.cpp src/image.cpp src/gaussian_rvv.cpp src/syscalls.cpp \
	-I include -o canny_fp_test

run-fp-test: canny_fp_test
	qemu-riscv64 ./canny_fp_test

.PHONY: canny_fp_test run-fp-test

# ---------- LMUL Sweep (Gaussian only, -Os) ----------
canny_lmul1:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -Os \
	src/main_lmul1.cpp src/image.cpp src/gaussian_rvv.cpp src/sobel_rvv.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_lmul1

canny_lmul2: canny_Os_rvv
	cp canny_Os_rvv canny_lmul2

canny_lmul4:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -Os \
	src/main_lmul4.cpp src/image.cpp src/gaussian_rvv.cpp src/sobel_rvv.cpp src/nms.cpp src/hysteresis.cpp src/syscalls.cpp \
	-I include -o canny_lmul4

lmul-sweep: canny_lmul1 canny_lmul2 canny_lmul4
	@echo "=== LMUL=1 ==="
	qemu-riscv64 ./canny_lmul1
	@echo "=== LMUL=2 (default) ==="
	qemu-riscv64 ./canny_lmul2
	@echo "=== LMUL=4 ==="
	qemu-riscv64 ./canny_lmul4

.PHONY: canny_lmul1 canny_lmul2 canny_lmul4 lmul-sweep

# ---------- Scalar vs RVV Equivalence Test (non-power-of-two, all VLEN) ----------
equivalence_test:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static \
	tests/equivalence_test.cpp src/image.cpp src/gaussian.cpp src/gaussian_rvv.cpp src/sobel.cpp src/sobel_rvv.cpp src/syscalls.cpp \
	-I include -o equivalence_test

test-equivalence: equivalence_test
	@echo "=== VLEN=128 ==="
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./equivalence_test
	@echo "=== VLEN=256 ==="
	qemu-riscv64 -cpu rv64,v=true,vlen=256 ./equivalence_test
	@echo "=== VLEN=512 ==="
	qemu-riscv64 -cpu rv64,v=true,vlen=512 ./equivalence_test

.PHONY: equivalence_test test-equivalence
