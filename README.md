# 1. Checkout
```sh
git clone --recursive https://github.com/franzflasch/riscv-test-scripts.git
```

# 2. Install patches
```sh
./apply_patches.sh
```

# 3. Generate qemu register states
```sh
./build_qemu_tests.sh <32/64>
```

# 4. Run riscv_em tests:
```sh
./build_riscv_em_tests.sh <32/64>
```
