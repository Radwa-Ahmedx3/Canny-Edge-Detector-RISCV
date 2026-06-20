# Canny-Edge-Detector-RISCV
Canny-edge detection code method examination and enhancing results by testing other algorithms using Risc-V processor

## Prerequisites

- RISC-V cross-compiler: `riscv64-unknown-elf-g++` (bare-metal/newlib target — **not** `riscv64-unknown-linux-gnu-g++`)
- QEMU user-mode emulator: `qemu-riscv64`
- Python 3 with Pillow: `pip install pillow --break-system-packages`

Verify your environment:
```bash
riscv64-unknown-elf-g++ --version
qemu-riscv64 --version
```
> Note: when typing `--version`, make sure both dashes are real hyphens (`-`). Pasting from Word/Docs can silently convert `--` into a single en-dash (`–`), which causes a confusing "No such file or directory" error.

## Quick Start

```bash
git clone <repo-url>
cd Canny-Edge-Detector-RISCV
make setup              # generates test_input.raw from real_image.jpg (128x128 grayscale)
make canny_rv            # builds the scalar pipeline
make run                 # runs it under qemu-riscv64
```

To use your own image instead:
```bash
make setup IMG=path/to/your_image.jpg
```

## Project Structure

- `src/main.cpp` — RVV pipeline entry point (calls `gaussianBlurRVV` / `sobelGradientRVV`)
- `src/main_scalar.cpp` — scalar pipeline entry point (calls `gaussianBlur` / `sobelGradient`)
- `src/main_fp.cpp` — standalone benchmark comparing RVV exact-division vs fixed-point Gaussian
- `src/gaussian.cpp`, `src/sobel.cpp` — scalar implementations
- `src/gaussian_rvv.cpp`, `src/sobel_rvv.cpp` — RVV intrinsic implementations
- `src/syscalls.cpp` — required for file I/O on bare-metal builds (newlib has no real `open`/`read`/`write` without this; omitting it causes `Cannot open: ./test_input.raw` even when the file exists)
- `src/nms.cpp`, `src/hysteresis.cpp` — shared by both scalar and RVV pipelines
- `scripts/img_to_raw.py` — converts any image into the headerless 128x128 8-bit grayscale `.raw` format the pipeline expects

## All Makefile Targets

| Command | What it does |
|---|---|
| `make setup` | Generates `test_input.raw` from `real_image.jpg` (or `IMG=...`) |
| `make canny_rv` / `make run` | Build / run the scalar pipeline |
| `make sweep` | Build scalar pipeline at all optimization levels (`-O0/-O2/-O3/-Os/-Ofast`) |
| `make sweep_rvv` | Build RVV pipeline at all optimization levels |
| `make test-equivalence` | Scalar vs RVV equivalence test (100x75, non-power-of-two) at VLEN 128/256/512 |
| `make lmul-sweep` | Run Gaussian at LMUL=1, LMUL=2 (default), and LMUL=4, `-Os` |
| `make vlen-sweep` | Run the `-Os` RVV binary at VLEN 128/256/512 |
| `make canny_O3_vecinfo` | Build with `-fopt-info-vec-all`, writes report to `vec_report.txt` |
| `make run-fp-test` | Build & run the fixed-point vs exact-division Gaussian benchmark |
| `make binary-sizes` | Build everything and print binary sizes side by side |
| `make clean` | Remove all generated binaries and reports |

Run `qemu-riscv64 ./<binary_name>` directly for any binary built above (e.g. `qemu-riscv64 ./canny_O3_rvv`).

## Troubleshooting

- **"Cannot open: ./test_input.raw"** — run `make setup` first, and make sure you're running commands from the repo root.
- **"No such file or directory" on `qemu-riscv64 --version`** — check for a stray en-dash; retype the command instead of pasting it.
- **Linker error `undefined reference to gaussianBlurRVV`** — you're linking `main.cpp` (RVV entry point) with scalar source files. Use `main_scalar.cpp` for scalar builds, `main.cpp` for RVV builds (see `make canny_rv` vs `make canny_O0_rvv` in the Makefile).
- **After `git clean`, all `qemu-riscv64 ./canny_*` commands fail** — `git clean` removes untracked build artifacts. Run `make sweep sweep_rvv canny_fp_test` (or `make binary-sizes`) to rebuild everything.
