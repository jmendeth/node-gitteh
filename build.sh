#!/bin/bash
git submodule init
git submodule update

# Make sure node-gyp is available before starting
hash node-gyp &> /dev/null
if [ $? -eq 1 ]; then
    echo "node-gyp is required, and must be available on the path"
    exit 1
fi

mkdir -p deps/libgit2/build && \
    cd deps/libgit2/build && \
    cmake -D CMAKE_BUILD_TYPE=Release -D BUILD_SHARED_LIBS=false -D BUILD_CLAR=false .. && \
    cmake --build . && \
    cd ../../.. && \
    node-gyp rebuild;
