#!/bin/bash
set -e  # Ensure script stops on the first error

SRC_DIR="./src"
INC_DIR="-I./include"
OBJ_DIR="./obj"
BIN_DIR="./bin"
OBJ_FILES=""

mkdir -p "$OBJ_DIR"
mkdir -p "$BIN_DIR"

SOURCES=("main.c" "network.c" "http_handlers.c" "file_ops.c" "utils.c")

for source in "${SOURCES[@]}"; do
    obj_file="${OBJ_DIR}/$(basename $source .c).o"
    gcc -c -o "$obj_file" "$SRC_DIR/$source" $INC_DIR
    OBJ_FILES+="$obj_file "
done

gcc -o "${BIN_DIR}/http_server" $OBJ_FILES -pthread

"${BIN_DIR}/http_server" -directory "/Users/karolbroda/Personal/http_server_c/test"
