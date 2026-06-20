#!/usr/bin/env python3
"""
Convert an image into the headerless 128x128 8-bit grayscale .raw
format expected by Image::load() in src/image.cpp.

Usage:
    python3 scripts/img_to_raw.py <input_image> <output.raw>
"""
import sys
from PIL import Image

WIDTH, HEIGHT = 128, 128

def convert(input_path, output_path):
    img = Image.open(input_path).convert("L")        # grayscale
    img = img.resize((WIDTH, HEIGHT))                 # fixed size
    img.save("preview_" + output_path.replace(".raw", ".png"))  # optional sanity check
    with open(output_path, "wb") as f:
        f.write(img.tobytes())
    print(f"Wrote {output_path}: {WIDTH*HEIGHT} bytes ({WIDTH}x{HEIGHT} grayscale)")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input_image> <output.raw>")
        sys.exit(1)
    convert(sys.argv[1], sys.argv[2])
