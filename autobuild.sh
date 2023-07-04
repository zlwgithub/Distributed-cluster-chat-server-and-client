#!/bin/bash

set -e

cd `pwd`/middleware/protobuf &&
    protoc qq.proto  --cpp_out=./

# cd ../../

# rm -rf `pwd`/build/*

# cd `pwd`/build && 
#      cmake .. &&
#      make

# cd ..

