#!/bin/sh
set -e

tmpFile=$(mktemp)

gcc -I./include -o $tmpFile src/*.c -lcurl

exec "$tmpFile" "$@"
