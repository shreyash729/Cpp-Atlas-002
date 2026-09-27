#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
mkdir -p build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
PORT=8080 ./build/server &
PID=$!
trap 'kill -TERM "$PID" 2>/dev/null; exit 0' SIGTERM SIGINT EXIT
get_snap() {
  find src include -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" \) -exec stat -c "%Y %n" {} + 2>/dev/null | sort
}
LAST_SNAP=$(get_snap)
while true; do
  sleep 2
  CUR_SNAP=$(get_snap)
  if [ "$CUR_SNAP" != "$LAST_SNAP" ]; then
    echo "[Engine] Rebuilding C++ binary..."
    if cmake --build build -j2; then
      kill -TERM "$PID" 2>/dev/null || true
      wait "$PID" 2>/dev/null || true
      PORT=8080 ./build/server &
      PID=$!
      echo "[Engine] Server restarted."
    fi
    LAST_SNAP="$CUR_SNAP"
  fi
done
