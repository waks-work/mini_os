#!/bin/bash
echo "🔍 Linting code for mini_os..."

# C waks_cstd project.
[ -f ".clang-format" ] && find src/ -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" | xargs clang-format -i
[ -f "CMakeLists.txt" ] && cmake-format -i CMakeLists.txt 2>/dev/null || true

echo "✅ Linting completed for mini_os!"
