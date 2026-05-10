#!/bin/bash

PROJECT_DIR="$(dirname "$0")"
BUILD_DIR="$PROJECT_DIR/build"

mkdir -p "$BUILD_DIR"

cmake -S "$PROJECT_DIR" -B "$BUILD_DIR"

cmake --build "$BUILD_DIR" --config Release

echo "$BUILD_DIR/media_daemon"