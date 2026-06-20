RV_CXX = riscv64-unknown-elf-g++
RV_FLAGS = -march=rv64gcv -mabi=lp64d -static -I include

.PHONY: phase4-O0
phase4-O0:
	$(RV_CXX) $(RV_FLAGS) -O0 -fopt-info-vec-all=vec_O0.txt src/phase4_timing.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp -o canny_O0

.PHONY: phase4-O2
phase4-O2:
	$(RV_CXX) $(RV_FLAGS) -O2 -fopt-info-vec-all=vec_O2.txt src/phase4_timing.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp -o canny_O2

.PHONY: phase4-O3
phase4-O3:
	$(RV_CXX) $(RV_FLAGS) -O3 -fopt-info-vec-all=vec_O3.txt src/phase4_timing.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp -o canny_O3

.PHONY: phase4-Os
phase4-Os:
	$(RV_CXX) $(RV_FLAGS) -Os -fopt-info-vec-all=vec_Os.txt src/phase4_timing.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp -o canny_Os

.PHONY: phase4-Ofast
phase4-Ofast:
	$(RV_CXX) $(RV_FLAGS) -Ofast -fopt-info-vec-all=vec_Ofast.txt src/phase4_timing.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp -o canny_Ofast

.PHONY: phase4-all
phase4-all: phase4-O0 phase4-O2 phase4-O3 phase4-Os phase4-Ofast

.PHONY: phase4-timing
phase4-timing: phase4-all
	@echo "=== PHASE 4 TIMING RESULTS ===" && \
	for opt in O0 O2 O3 Os Ofast; do qemu-riscv64 -cpu rv64,v=true,vlen=128 ./canny_$$opt; echo ""; done

.PHONY: phase4-sizes
phase4-sizes:
	@echo "=== BINARY SIZES (KB) ===" && \
	for opt in O0 O2 O3 Os Ofast; do size=$$(stat -c "%s" canny_$$opt); echo "canny_$$opt: $$((size / 1024)) KB"; done

.PHONY: phase4-vecinfo
phase4-vecinfo:
	@echo "=== AUTO-VECTORIZATION ===" && \
	echo "Successful vectorizations:" && \
	for opt in O0 O2 O3 Os Ofast; do count=$$(grep "note: vectorized" vec_O$$opt.txt 2>/dev/null | grep -v "vectorized 0" | wc -l); echo "  O$$opt: $$count"; done && \
	echo "" && \
	echo "Vector instructions (vset):" && \
	for opt in O0 O2 O3 Os Ofast; do count=$$(riscv64-unknown-elf-objdump -d canny_$$opt 2>/dev/null | grep -c "vset" || echo "0"); echo "  O$$opt: $$count"; done

.PHONY: clean
clean:
	rm -f canny_O* vec_O*.txt *.o

.PHONY: phase4-deeper-build
phase4-deeper-build:
	$(RV_CXX) $(RV_FLAGS) -O3 -fopt-info-vec-all=vec_O3_with_checks.txt src/phase4_timing.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp -o canny_O3_with_checks
	$(RV_CXX) $(RV_FLAGS) -O3 -fopt-info-vec-all=vec_O3_no_checks.txt src/phase4_timing.cpp src/image.cpp src/gaussian_padded.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp -o canny_O3_no_checks

.PHONY: phase4-deeper-timing
phase4-deeper-timing: phase4-deeper-build
	@echo "=== TIMING COMPARISON ===" && \
	echo "WITH boundary checks:" && qemu-riscv64 -cpu rv64,v=true,vlen=128 ./canny_O3_with_checks | grep Total && \
	echo "" && \
	echo "WITHOUT boundary checks:" && qemu-riscv64 -cpu rv64,v=true,vlen=128 ./canny_O3_no_checks | grep Total

.PHONY: phase4-deeper-vecinfo
phase4-deeper-vecinfo:
	@echo "=== VECTORIZATION COMPARISON ===" && \
	echo "WITH boundary checks:" && grep "gaussianBlur\|gaussianBlur" vec_O3_with_checks.txt | grep "note: vectorized" && \
	echo "" && \
	echo "WITHOUT boundary checks:" && grep "note: vectorized" vec_O3_no_checks.txt

.PHONY: phase4-deeper-vecinfo-fixed
phase4-deeper-vecinfo-fixed:
	@echo "=== VECTORIZATION COMPARISON ===" && \
	echo "WITH boundary checks:" && grep "note: vectorized" vec_O3_with_checks.txt | grep "gaussian.cpp:12" && \
	echo "" && \
	echo "WITHOUT boundary checks:" && grep "note: vectorized" vec_O3_no_checks.txt | head -5

.PHONY: phase5-build
phase5-build:
	$(RV_CXX) $(RV_FLAGS) -O3 src/phase5_profiling.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp -o phase5_profile

.PHONY: phase5-run
phase5-run: phase5-build
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase5_profile

.PHONY: phase5
phase5: phase5-run
