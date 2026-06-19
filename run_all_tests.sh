#!/bin/bash
echo "==============================="
echo "Testing with input_square.raw"
echo "==============================="
python3 -c "
import numpy as np
img = np.zeros((128,128), dtype=np.uint8)
img[30:90, 30:90] = 255
data = img.tobytes()
print('unsigned char test_image[] = {' + ','.join(str(b) for b in data) + '};')
print('int test_w = 128, test_h = 128;')
" > src/test_data.hpp
riscv64-unknown-elf-g++ -march=rv64gcv -O2 src/image.cpp src/gaussian.cpp src/sobel.cpp src/test_main.cpp -Iinclude -Isrc -o canny_test_rv
echo "--- vlen=128 ---"
qemu-riscv64 -cpu rv64,v=true,vlen=128 ./canny_test_rv
echo "--- vlen=256 ---"
qemu-riscv64 -cpu rv64,v=true,vlen=256 ./canny_test_rv
echo "--- vlen=512 ---"
qemu-riscv64 -cpu rv64,v=true,vlen=512 ./canny_test_rv

echo ""
echo "==============================="
echo "Testing with input_lines.raw"
echo "==============================="
python3 -c "
import numpy as np
img = np.zeros((128,128), dtype=np.uint8)
img[60:68, :] = 255
img[:, 60:68] = 255
data = img.tobytes()
print('unsigned char test_image[] = {' + ','.join(str(b) for b in data) + '};')
print('int test_w = 128, test_h = 128;')
" > src/test_data.hpp
riscv64-unknown-elf-g++ -march=rv64gcv -O2 src/image.cpp src/gaussian.cpp src/sobel.cpp src/test_main.cpp -Iinclude -Isrc -o canny_test_rv
echo "--- vlen=128 ---"
qemu-riscv64 -cpu rv64,v=true,vlen=128 ./canny_test_rv
echo "--- vlen=256 ---"
qemu-riscv64 -cpu rv64,v=true,vlen=256 ./canny_test_rv
echo "--- vlen=512 ---"
qemu-riscv64 -cpu rv64,v=true,vlen=512 ./canny_test_rv

echo ""
echo "==============================="
echo "Testing with input_circle.raw"
echo "==============================="
python3 -c "
import numpy as np
img = np.zeros((128,128), dtype=np.uint8)
for y in range(128):
    for x in range(128):
        if (x-64)**2 + (y-64)**2 < 40**2:
            img[y,x] = 255
data = img.tobytes()
print('unsigned char test_image[] = {' + ','.join(str(b) for b in data) + '};')
print('int test_w = 128, test_h = 128;')
" > src/test_data.hpp
riscv64-unknown-elf-g++ -march=rv64gcv -O2 src/image.cpp src/gaussian.cpp src/sobel.cpp src/test_main.cpp -Iinclude -Isrc -o canny_test_rv
echo "--- vlen=128 ---"
qemu-riscv64 -cpu rv64,v=true,vlen=128 ./canny_test_rv
echo "--- vlen=256 ---"
qemu-riscv64 -cpu rv64,v=true,vlen=256 ./canny_test_rv
echo "--- vlen=512 ---"
qemu-riscv64 -cpu rv64,v=true,vlen=512 ./canny_test_rv
