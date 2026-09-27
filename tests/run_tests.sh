#!/usr/bin/env bash
set -e
cd "$(dirname "$0")/.."
cmake --build build --target challenge_tests -j2 1>&2
exec ./build/challenge_tests
