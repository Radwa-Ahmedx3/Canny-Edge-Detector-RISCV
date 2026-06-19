import numpy as np
import matplotlib.pyplot as plt

# تحميل الصورتين
blurred = np.fromfile('output/blurred.raw', dtype=np.uint8).reshape(128, 128)
magnitude = np.fromfile('output/magnitude.raw', dtype=np.uint8).reshape(128, 128)
# رسم الصورتين
fig, axes = plt.subplots(1, 2, figsize=(10, 5))

axes[0].imshow(blurred, cmap='gray')
axes[0].set_title('Gaussian Blur Output')

axes[1].imshow(magnitude, cmap='gray')
axes[1].set_title('Sobel Magnitude Output')

plt.tight_layout()
plt.savefig('output_visualization.png')
print("Saved: output_visualization.png")
