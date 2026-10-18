#!/bin/sh
# Usage: build.sh [debug|release], release by default
set -e
config="${1:-release}"

cd "$(dirname "$0")/.."
cmake --preset "$config"
cmake --build --preset "$config"
