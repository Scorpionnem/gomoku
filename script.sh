#!/usr/bin/env bash
#
# install_sdl2_ttf.sh — build & install SDL2_ttf into ~/.local without root.
#
# Designed for 42 machines: no sudo, SDL2 already installed under ~/.local
# (via suprapack / Cosmos), system FreeType present. Falls back to the
# bundled FreeType/HarfBuzz submodules if the system ones are missing.
#
# Usage:
#   ./install_sdl2_ttf.sh              # build & install
#   PREFIX=~/somewhere ./install_sdl2_ttf.sh
#   TTF_BRANCH=SDL2 ./install_sdl2_ttf.sh
#
set -euo pipefail

PREFIX="${PREFIX:-$HOME/.local}"
TTF_BRANCH="${TTF_BRANCH:-SDL2}"
BUILD_DIR="$(mktemp -d "${TMPDIR:-/tmp}/sdl2_ttf.XXXXXX")"
JOBS="$(nproc 2>/dev/null || echo 2)"

log()  { printf '\033[1;34m==>\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33m warning:\033[0m %s\n' "$*" >&2; }
die()  { printf '\033[1;31m error:\033[0m %s\n' "$*" >&2; exit 1; }

cleanup() { [ -n "${BUILD_DIR:-}" ] && rm -rf "$BUILD_DIR"; }
trap cleanup EXIT

# --- sanity checks ----------------------------------------------------------

command -v git   >/dev/null || die "git not found"
command -v gcc   >/dev/null || die "gcc not found"
command -v make  >/dev/null || die "make not found"

# Make configure/pkg-config aware of our prefix before we probe anything.
export PATH="$PREFIX/bin:$PATH"
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig:${PKG_CONFIG_PATH:-}"
export LD_LIBRARY_PATH="$PREFIX/lib:${LD_LIBRARY_PATH:-}"

# SDL2 must already be present (this script only builds the TTF add-on).
if ! command -v sdl2-config >/dev/null && ! pkg-config --exists sdl2 2>/dev/null; then
    die "SDL2 not found under $PREFIX. Install base SDL2 first (e.g. suprapack install libsdl2)."
fi

# Decide FreeType strategy: prefer system, else bundled submodules.
USE_SYSTEM_FT=0
if pkg-config --exists freetype2 2>/dev/null; then
    USE_SYSTEM_FT=1
    log "system FreeType found: $(pkg-config --modversion freetype2)"
else
    warn "system FreeType not found — will build the bundled copy from submodules"
fi

# --- fetch source -----------------------------------------------------------

log "cloning SDL_ttf ($TTF_BRANCH) into $BUILD_DIR"
git clone --depth 1 -b "$TTF_BRANCH" \
    https://github.com/libsdl-org/SDL_ttf.git "$BUILD_DIR/SDL_ttf"
cd "$BUILD_DIR/SDL_ttf"

CONFIGURE_ARGS=( "--prefix=$PREFIX" )

# Point at our SDL2 if sdl2-config lives in the prefix.
if command -v sdl2-config >/dev/null; then
    CONFIGURE_ARGS+=( "--with-sdl-prefix=$PREFIX" )
fi

if [ "$USE_SYSTEM_FT" -eq 1 ]; then
    # Link the system FreeType/HarfBuzz instead of the (empty) submodules.
    CONFIGURE_ARGS+=( --disable-freetype-builtin --disable-harfbuzz-builtin )
else
    # Need the vendored sources — pull the submodules in.
    log "fetching bundled FreeType/HarfBuzz submodules"
    git submodule update --init --recursive
fi

# --- configure --------------------------------------------------------------

if [ ! -x ./configure ]; then
    [ -x ./autogen.sh ] && { log "running autogen.sh"; ./autogen.sh; }
fi

if [ -x ./configure ]; then
    log "configure ${CONFIGURE_ARGS[*]}"
    ./configure "${CONFIGURE_ARGS[@]}"
    log "building (-j$JOBS)"
    make -j"$JOBS"
    log "installing to $PREFIX"
    make install
else
    # No autotools available — fall back to CMake.
    command -v cmake >/dev/null || die "neither ./configure nor cmake available"
    log "no configure script; using CMake"
    CMAKE_ARGS=(
        -B build
        -DCMAKE_PREFIX_PATH="$PREFIX"
        -DCMAKE_INSTALL_PREFIX="$PREFIX"
        -DBUILD_SHARED_LIBS=ON
    )
    if [ "$USE_SYSTEM_FT" -eq 1 ]; then
        CMAKE_ARGS+=( -DSDLTTF_VENDORED=OFF )
    else
        CMAKE_ARGS+=( -DSDLTTF_VENDORED=ON )
    fi
    cmake "${CMAKE_ARGS[@]}"
    cmake --build build -j"$JOBS"
    cmake --install build
fi

# --- verify -----------------------------------------------------------------

if pkg-config --exists SDL2_ttf 2>/dev/null; then
    log "installed SDL2_ttf $(pkg-config --modversion SDL2_ttf)"
    log "link flags: $(pkg-config --cflags --libs SDL2_ttf)"
else
    warn "install finished but pkg-config can't see SDL2_ttf — check PKG_CONFIG_PATH"
fi

cat <<EOF

Done. SDL2_ttf is in $PREFIX.

If your shell doesn't already export these, add them to ~/.zshrc:

    export PATH="\$HOME/.local/bin:\$PATH"
    export LD_LIBRARY_PATH="\$HOME/.local/lib:\$LD_LIBRARY_PATH"
    export PKG_CONFIG_PATH="\$HOME/.local/lib/pkgconfig:\$PKG_CONFIG_PATH"

Then compile with:  \$(pkg-config --cflags --libs sdl2 SDL2_ttf)
or just add:        -lSDL2_ttf   to your existing SDL2 link line.
EOF