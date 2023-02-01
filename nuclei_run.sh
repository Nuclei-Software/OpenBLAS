#!/bin/bash
# description: Help to build and install Openblas quickly

OPENBLAS_ROOT=${OPENBLAS_ROOT:-$(readlink -f ../OpenBLAS)}
TARGET=${TARGET:-UX900FD}
CC=${CC:-riscv-nuclei-linux-gnu-gcc}
makeopts="HOSTCC=gcc TARGET=${TARGET} CC=${CC} NOFORTRAN=1 NO_SHARED=1 USE_THREAD=0 NO_LAPACK=1 USE_OPENMP=0 CFLAGS=-static BINARY=64"

function usage() {
  echo -n1 -e "\nPlease enter the choice:"
  echo "1. quick build (ARCH_EXT=v)"
  echo "2. quick build (none)"
  read -p "default 1:" choice
  if [ "$choice" = "2" ]; then
    ARCH_EXT=
    echo "choice 2: no RVV opt"
  else
    ARCH_EXT=v
    echo "choice 1: use RVV opt"
  fi
  read -p "Please check and Press Enter to continue build..."
}

function env_setup() {
  eval "${CC} -v"
  if [ $? -ne 0 ]; then
    echo "Please set ${CC} path correct!!!"
    exit 1
  fi
}

function clean_all() {
  runcmd="make ${makeopts} ARCH_EXT=${ARCH_EXT} clean"
  echo $runcmd
  eval $runcmd
}

function build_lib() {
  runcmd="make ${makeopts} ARCH_EXT=${ARCH_EXT}"
  echo $runcmd
  eval $runcmd
}

function intall_lib() {
  runcmd="make ${makeopts} PREFIX=${OPENBLAS_ROOT}/prefix install"
  echo $runcmd
  eval $runcmd
}

function build_tests() {
  runcmd="make ${makeopts} all"
  [ -d utest ] && {
    cd utest
    echo $runcmd
    eval $runcmd
    cd -
  }
  [ -d ctest ] && {
    cd ctest
    echo $runcmd
    eval $runcmd
    cd -
  }
  [ -d benchmark ] && {
    cd benchmark
    runcmd="make ${makeopts} goto"
    echo $runcmd
    eval $runcmd
    cd -
  }
}

function main() {
  usage
  clean_all
  build_lib
  intall_lib
  build_tests
}

env_setup
main
