# =========================
# Compilers
# =========================

HOST_CXX = g++
RV_CXX = riscv64-unknown-linux-gnu-g++

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
	src/main.cpp \
	src/image.cpp \
	src/gaussian.cpp \
	src/sobel.cpp \
	src/nms.cpp \
	src/hysteresis.cpp \
	-I include \
	-o canny_pipeline

run:
	qemu-riscv64 ./canny_pipeline
# ---------- Clean ----------

clean:
	rm -f host_test my_rvv_test canny_pipeline canny_O0 canny_O2 canny_O3 canny_Os canny_Ofast

# ---------- Phase 4: Optimization Sweep ----------
canny_O0:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -O0 \
	src/main.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp \
	-I include -o canny_O0

canny_O2:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -O2 \
	src/main.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp \
	-I include -o canny_O2

canny_O3:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -O3 \
	src/main.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp \
	-I include -o canny_O3

canny_Os:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -Os \
	src/main.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp \
	-I include -o canny_Os

canny_Ofast:
	$(RV_CXX) -march=rv64gcv -mabi=lp64d -static -Ofast \
	src/main.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp \
	-I include -o canny_Ofast
