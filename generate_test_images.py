import numpy as np

# صورة 1: مربع
img1 = np.zeros((128, 128), dtype=np.uint8)
img1[40:90, 40:90] = 255
img1.tofile('input_square.raw')
print("Created: input_square.raw")

# صورة 2: دايرة
img2 = np.zeros((128, 128), dtype=np.uint8)
cy, cx = 64, 64
for y in range(128):
    for x in range(128):
        if (x - cx)**2 + (y - cy)**2 < 30**2:
            img2[y, x] = 255
img2.tofile('input_circle.raw')
print("Created: input_circle.raw")

# صورة 3: خطوط
img3 = np.zeros((128, 128), dtype=np.uint8)
img3[30:40, :] = 255   # خط أفقي
img3[70:80, :] = 255   # خط أفقي
img3[:, 60:70] = 255   # خط رأسي
img3.tofile('input_lines.raw')
print("Created: input_lines.raw")

