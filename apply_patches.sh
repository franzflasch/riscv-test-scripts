#!/bin/sh

set -e

root_dir=$(PWD)

# riscv-compliance
cd cd ${root_dir}/riscv-compliance
patch -p1 < ../patches/001-prepare-riscv-compliance.patch

cd cd ${root_dir}/riscv-isa-sim
patch -p1 < ../patches/002-prepare-riscv-isa-sim.patch
