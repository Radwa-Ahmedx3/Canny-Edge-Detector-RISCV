CXX = riscv64-unknown-elf-g++
CXXFLAGS = -I./include -O2
LDFLAGS = -static

# الملفات الأساسية
SRC = src/image.cpp
TEST_SRC = tests/critical_infra_test.cpp
TARGET = infra_test

all:
	$(CXX) $(CXXFLAGS) $(SRC) $(TEST_SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
