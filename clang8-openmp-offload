#!/bin/sh

#Change this file permission to $chmod +x if needed.
echo "Downloading source files....."
wget https://releases.llvm.org/8.0.0/llvm-8.0.0.src.tar.xz
wget https://releases.llvm.org/8.0.0/cfe-8.0.0.src.tar.xz
wget https://releases.llvm.org/8.0.0/openmp-8.0.0.src.tar.xz
wget https://releases.llvm.org/8.0.0/compiler-rt-8.0.0.src.tar.xz

echo "Unpacking......"
tar xf llvm-8.0.0.src.tar.xz
tar xf cfe-8.0.0.src.tar.xz
tar xf openmp-8.0.0.src.tar.xz
tar xf compiler-rt-8.0.0.src.tar.xz

echo "Moving to the required directory......"
mv cfe-8.0.0.src llvm-8.0.0.src/tools/clang
mv openmp-8.0.0.src llvm-8.0.0.src/projects/openmp
mv compiler-rt-8.0.0.src llvm-8.0.0.src/projects/compiler-rt

#build directory
echo "Making build directory"
mkdir build
echo "Change to ./build"
cd build

echo "Initiating cmake"
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(pwd)/../install \
	-DCLANG_OPENMP_NVPTX_DEFAULT_ARCH=sm_60 \
	-DLIBOMPTARGET_NVPTX_COMPUTE_CAPABILITIES=35,60,70 ../llvm-8.0.0.src

echo "Initiating building process...."
make -j6
make -j6 install

echo "Building OpenMP offload.."
cd ..
mkdir build-openmp
cd build-openmp

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(pwd)/../install \
	-DCMAKE_C_COMPILER=$(pwd)/../install/bin/clang \
	-DCMAKE_CXX_COMPILER=$(pwd)/../install/bin/clang++ \
	-DLIBOMPTARGET_NVPTX_COMPUTE_CAPABILITIES=35,60,70 \
	../llvm-8.0.0.src/projects/openmp

echo "Building offload library...."
make -j6
make -j6 install

echo "Return to original directory"
cd ..
