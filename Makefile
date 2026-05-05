HOST_CXX   = g++
RV_CXX     = riscv64-unknown-elf-g++
RV_FLAGS   = -march=rv64gcv -static
GTEST_INC  = $(HOME)/gtest-install/include
GTEST_LIB  = $(HOME)/gtest-install/lib

canny_rv: src/main.cpp
	$(RV_CXX) $(RV_FLAGS) -O2 -o build-rv/canny_rv src/main.cpp

run: canny_rv
	qemu-riscv64 -cpu rv64,v=true,vlen=128 ./build-rv/canny_rv

test: tests/test_pipeline.cpp
	$(HOST_CXX) -O2 -I$(GTEST_INC) -L$(GTEST_LIB) \
	  tests/test_pipeline.cpp -lgtest -lgtest_main -lpthread \
	  -o build-host/test_runner
	./build-host/test_runner

clean:
	rm -rf build-host/* build-rv/*
