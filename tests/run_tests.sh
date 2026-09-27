#!/usr/bin/env bash
set -e
cd "$(dirname "$0")/.."
rm -rf build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release 1>&2
cmake --build build --target challenge_tests -j2 1>&2
exec ./build/challenge_tests
