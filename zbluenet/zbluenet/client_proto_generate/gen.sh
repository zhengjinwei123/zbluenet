#!/bin/bsh

set -o pipefail

script_name=`basename "$0"`
script_abs_name=`readlink -f "$0"`
script_path=`dirname "$script_abs_name"`

chmod +x "$script_path"/protoc

CC_COMPILE_PATH="$script_path"/protoc
CC_TARGET_PATH=../src/demo/proto
SOURCE_FOLDER="$script_path"/protofiles

$CC_COMPILE_PATH -I="$SOURCE_FOLDER" --cpp_out=../src/demo/proto/ $SOURCE_FOLDER/test.proto
