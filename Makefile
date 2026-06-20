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

.PHONY: phase6-lmul-build
phase6-lmul-build:
	$(RV_CXX) $(RV_FLAGS) -O3 src/phase6_lmul_sweep.cpp src/image.cpp src/gaussian_rvv_lmul1.cpp src/gaussian_rvv_lmul2.cpp src/gaussian_rvv_lmul4.cpp -o phase6_lmul_sweep

.PHONY: phase6-lmul-run
phase6-lmul-run: phase6-lmul-build
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase6_lmul_sweep

.PHONY: phase6-lmul
phase6-lmul: phase6-lmul-run

.PHONY: phase6-rvv-build
phase6-rvv-build:
	$(RV_CXX) $(RV_FLAGS) -O3 src/phase6_rvv_timing.cpp src/image.cpp src/gaussian_rvv.cpp -o phase6_rvv_timing

.PHONY: phase6-rvv-run
phase6-rvv-run: phase6-rvv-build
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase6_rvv_timing

.PHONY: phase6
phase6: phase6-rvv-run

.PHONY: phase6-lmul-timing
phase6-lmul-timing: phase6-rvv-build
	@echo "=== PHASE 6: LMUL SWEEP (LMUL=2 chosen) ===" && \
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase6_rvv_timing

.PHONY: phase6-vlen-sweep
phase6-vlen-sweep: phase6-rvv-build
	@echo "=== PHASE 6: VLEN SWEEP ===" && \
	echo "VLEN=128:" && qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase6_rvv_timing && \
	echo "VLEN=256:" && qemu-riscv64 -cpu rv64,v=true,vlen=256 ./phase6_rvv_timing && \
	echo "VLEN=512:" && qemu-riscv64 -cpu rv64,v=true,vlen=512 ./phase6_rvv_timing

.PHONY: phase6-all
phase6-all: phase6-lmul-timing phase6-vlen-sweep

.PHONY: phase6-pipeline-build
phase6-pipeline-build:
	$(RV_CXX) $(RV_FLAGS) -O3 src/phase6_pipeline.cpp src/image.cpp src/gaussian.cpp src/gaussian_rvv.cpp src/sobel.cpp src/sobel_rvv.cpp src/nms.cpp src/hysteresis.cpp -o phase6_pipeline

.PHONY: phase6-pipeline-run
phase6-pipeline-run: phase6-pipeline-build
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase6_pipeline

.PHONY: phase6-complete
phase6-complete: phase6-pipeline-run phase6-vlen-sweep

.PHONY: phase6-lmul-sweep
phase6-lmul-sweep:
	$(RV_CXX) $(RV_FLAGS) -O3 src/phase6_lmul_sweep_final.cpp src/image.cpp src/gaussian_rvv.cpp -o phase6_lmul_sweep
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase6_lmul_sweep

.PHONY: phase6-2-build
phase6-2-build:
	$(RV_CXX) $(RV_FLAGS) -O3 src/phase6_lmul_sweep_final.cpp src/image.cpp src/gaussian_rvv.cpp -o phase6_lmul_sweep

.PHONY: phase6-2-run
phase6-2-run: phase6-2-build
	@echo "=== PHASE 6.2: LMUL SWEEP ===" && \
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase6_lmul_sweep

.PHONY: phase6-2-vlen-128
phase6-2-vlen-128: phase6-2-build
	@echo "Testing at VLEN=128:" && \
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase6_lmul_sweep

.PHONY: phase6-2-vlen-256
phase6-2-vlen-256: phase6-2-build
	@echo "Testing at VLEN=256:" && \
	qemu-riscv64 -cpu rv64,v=true,vlen=256 ./phase6_lmul_sweep

.PHONY: phase6-2-vlen-512
phase6-2-vlen-512: phase6-2-build
	@echo "Testing at VLEN=512:" && \
	qemu-riscv64 -cpu rv64,v=true,vlen=512 ./phase6_lmul_sweep

.PHONY: phase6-2-all
phase6-2-all: phase6-2-vlen-128 phase6-2-vlen-256 phase6-2-vlen-512

.PHONY: phase6-2
phase6-2: phase6-2-all

.PHONY: phase6-3-build
phase6-3-build:
	$(RV_CXX) $(RV_FLAGS) -O3 src/phase6_sobel_rvv.cpp src/image.cpp src/sobel.cpp src/sobel_rvv.cpp -o phase6_sobel_rvv

.PHONY: phase6-3-run
phase6-3-run: phase6-3-build
	@echo "=== PHASE 6.3: SOBEL MAGNITUDE (L1 NORM) ===" && \
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase6_sobel_rvv

.PHONY: phase6-3-vlen-128
phase6-3-vlen-128: phase6-3-build
	@echo "VLEN=128:" && qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase6_sobel_rvv

.PHONY: phase6-3-vlen-256
phase6-3-vlen-256: phase6-3-build
	@echo "VLEN=256:" && qemu-riscv64 -cpu rv64,v=true,vlen=256 ./phase6_sobel_rvv

.PHONY: phase6-3-vlen-512
phase6-3-vlen-512: phase6-3-build
	@echo "VLEN=512:" && qemu-riscv64 -cpu rv64,v=true,vlen=512 ./phase6_sobel_rvv

.PHONY: phase6-3-all
phase6-3-all: phase6-3-vlen-128 phase6-3-vlen-256 phase6-3-vlen-512

.PHONY: phase6-3
phase6-3: phase6-3-all
