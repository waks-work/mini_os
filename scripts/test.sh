#!/bin/bash
echo "🧪 Running tests for mini_os..."

# C waks_cstd project.
[ -f "Makefile" ] && make test
[ -f "CMakeLists.txt" ] && cd build && ctest .. && cd ..

echo "✅ Tests completed for mini_os!"
