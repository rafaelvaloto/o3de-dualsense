#!/bin/bash
# =============================================================================
# o3de-dualsense - Code Formatter Script
# =============================================================================
# This script runs clang-format on all C++ source files in the Code directory
# Automatically finds clang-format from Visual Studio on Windows
# Usage: ./format-code.sh [--check] [--dry-run]
# =============================================================================

# http://redsymbol.net/articles/unofficial-bash-strict-mode/
set -euo pipefail
IFS=$'\n\t'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CODE_DIR="${SCRIPT_DIR}/Code"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# =============================================================================
# Platform-specific setup
# =============================================================================

function win_setup() {
    local vswhere
    vswhere="$(cygpath -u "/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe")"

    if [ ! -f "${vswhere}" ]; then
        printf "${YELLOW}Warning: vswhere.exe not found at %s${NC}\n" "${vswhere}"
        printf "${YELLOW}Trying to use clang-format from PATH...${NC}\n"
        return
    fi

    local clang_format
    clang_format="$("${vswhere}" -products '*' -latest -find VC/Tools/Llvm/bin/clang-format.exe 2>/dev/null || echo "")"

    if [ -z "${clang_format}" ] || [ ! -f "$(cygpath -u "${clang_format}")" ]; then
        printf "${YELLOW}Warning: clang-format not found in Visual Studio${NC}\n"
        printf "${YELLOW}Trying to use clang-format from PATH...${NC}\n"
        return
    fi

    local clang_format_dir
    clang_format_dir="$(cygpath -u "$(dirname "${clang_format}")")"
    export PATH="${clang_format_dir}:${PATH}"
    printf "${GREEN}Found clang-format in Visual Studio: %s${NC}\n" "${clang_format}"
}

function mac_setup() {
    # macOS doesn't have nproc, define it
    function nproc {
        sysctl -n hw.logicalcpu
    }
    export -f nproc
}

# Detect OS and run platform-specific setup
case "${OSTYPE}" in
    "darwin"*)
        mac_setup
        ;;
    "msys"|"cygwin"|"win32")
        win_setup
        ;;
    "linux-gnu"*)
        # Linux - clang-format should be in PATH
        ;;
    *)
        printf "${YELLOW}Unknown OS type: %s${NC}\n" "${OSTYPE}"
        ;;
esac

# =============================================================================
# Verify clang-format is available
# =============================================================================

if ! command -v clang-format &> /dev/null; then
    printf "${RED}Error: clang-format is not installed or not in PATH${NC}\n"
    echo "Please install clang-format:"
    echo "  - Windows: Install Visual Studio with C++ tools, or: choco install llvm"
    echo "  - Ubuntu/Debian: sudo apt install clang-format"
    echo "  - macOS: brew install clang-format"
    exit 1
fi

# Display clang-format version
printf "\n${BLUE}%s${NC}\n\n" "$(clang-format --version)"

# =============================================================================
# Parse command line arguments
# =============================================================================

CHECK_ONLY=false
DRY_RUN=false
VERBOSE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --check)
            CHECK_ONLY=true
            shift
            ;;
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        --verbose|-v)
            VERBOSE=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --check     Check if files are formatted (returns non-zero if not)"
            echo "  --dry-run   Show which files would be formatted without modifying them"
            echo "  --verbose   Show detailed output"
            echo "  --help      Show this help message"
            exit 0
            ;;
        *)
            printf "${RED}Unknown option: %s${NC}\n" "$1"
            exit 1
            ;;
    esac
done

# =============================================================================
# Find and format files
# =============================================================================

printf "${BLUE}Scanning for C++ files in: %s${NC}\n" "${CODE_DIR}"

# Exclude ThirdParty directory (it has its own formatting rules)
if [ "$CHECK_ONLY" = true ]; then
    printf "${BLUE}Running in check mode...${NC}\n\n"

    FAILED=0
    while IFS= read -r -d '' file; do
        if [ "$VERBOSE" = true ]; then
            echo "Checking: $file"
        fi
        if ! clang-format --dry-run --Werror "$file" 2>/dev/null; then
            printf "${RED}Needs formatting: %s${NC}\n" "$file"
            FAILED=1
        fi
    done < <(find "${CODE_DIR}" \( -name "*.h" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.inl" \) -not -path "*ThirdParty*" -print0)

    if [ "$FAILED" -eq 1 ]; then
        printf "\n${RED}Some files need formatting. Run './format-code.sh' to fix.${NC}\n"
        exit 1
    else
        printf "\n${GREEN}All files are properly formatted!${NC}\n"
        exit 0
    fi

elif [ "$DRY_RUN" = true ]; then
    printf "${BLUE}Running in dry-run mode (no files will be modified)...${NC}\n\n"

    find "${CODE_DIR}" \( -name "*.h" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.inl" \) -not -path "*ThirdParty*" -print0 | \
        while IFS= read -r -d '' file; do
            echo "Would format: $file"
        done

    printf "\n${GREEN}Dry run complete.${NC}\n"

else
    printf "${BLUE}Formatting files...${NC}\n\n"

    # Use xargs for parallel processing (like GamepadCore does)
    find "${CODE_DIR}" \( -name "*.h" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.inl" \) -not -path "*ThirdParty*" -print0 | \
        xargs -0 -P "$(nproc 2>/dev/null || echo 4)" -n 100 clang-format -i

    printf "\n${GREEN}Formatting complete!${NC}\n"
fi

printf "\n${BLUE}Done!${NC}\n"

