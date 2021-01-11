#!/bin/sh

# riscv-compliance
cd riscv-compliance
patch -p1 < ../patches/001-prepare-riscv-compliance.patch
