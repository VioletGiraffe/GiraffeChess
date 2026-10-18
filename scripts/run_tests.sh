#!/bin/sh
# Usage: run_tests.sh [debug|release] [quick|full], release full by default
# Builds the configuration first.
set -e
config=release
suite=full

for arg in "$@"; do
	case "$arg" in
		quick|full) suite="$arg" ;;
		*) config="$arg" ;;
	esac
done

cd "$(dirname "$0")/.."
scripts/build.sh "$config"
ctest --preset "$config-$suite"
