#!/bin/sh

set -x

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-../build}
BUILD_TYPE=${BUILD_TYPE:-release}
INSTALL_DIR=${INSTALL_DIR:-../${BUILD_TYPE}-install-cpp11}
BUILD_NO_EXAMPLES=${BUILD_NO_EXAMPLES:-0}

ln -sf $BUILD_DIR/$BUILD_TYPE-cpp11/compile_commands.json

mkdir -p $BUILD_DIR/$BUILD_TYPE-cpp11 \
    && cd $BUILD_DIR/$BUILD_TYPE-cpp11 \
    && cmake \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_BUILD_NO_EXAMPLES=$BUILD_NO_EXAMPLES \
        $SOURCE_DIR \
    && make $*