#!/bin/bash
echo "🔧 Starting C Development environment for mini_os..."

run_build() {
    clear
    echo -e "${BLUE}🔨 Rebuilding mini_os kernel...${NC}"
    
    # zig build handles its own caching, no need to cd into build
    if zig build; then
          echo -e "${GREEN} Build Successful! Booting in QEMU...${NC}"
          # Launching QEMU in 'nographic' or 'curses' mode is great for terminal work
          # We use -kernel to boot the multiboot binary directly
         qemu-system-x86_64 -device loader,file=zig-out/bin/kernel.bin,addr=0x100000 -display none -serial stdio
    else
          echo -e "${RED} Build Failed. Check the logic in kernel/ or build.zig${NC}"
    fi
  }

reload() {
  GREEN='\033[0;32m'
  BLUE='\033[0;34m'
  RED='\033[0;31m'
  NC='\033[0m'
  echo -e "${BLUE} mini_os System Watcher started...${NC}"
  export -f run_build

  # Track all C, Header, Assembly, and Zig build files
  find kernel boot driver -name "*.c" -o -name "*.h" -o -name "*.s" -o -name "*.zig" | entr -r bash -c run_build
}

# C projects with hot reload
if [ -f "CMakeLists.txt" ] && [ -f "scripts/dev-reload.sh" ]; then
    echo "🔧 Starting C with hot reload..."
    reload
    exit 0
fi

echo "⚠️  No C Development server detected"
echo "💡 Supported: C with hot reload"
