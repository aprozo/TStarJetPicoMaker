#!/usr/bin/env bash
set -euo pipefail

WORKDIR=$PWD

JETPICODIR="$PWD/StRoot/eventStructuredAu"
BUILDDIR="$PWD/build"
LIBDIR="$PWD/libs"

STAR_ARCH="${STAR_HOST_SYS:-${STAR_SYS:-}}"
if [[ "$STAR_ARCH" != *x8664* ]]; then
  echo "Error: 64-bit STAR environment not detected." >&2
  echo "Run this inside rhic_sl7.sif after: setup 64b" >&2
  exit 1
fi

mkdir -p "$LIBDIR"  "$BUILDDIR/StRoot"
rm -f "$LIBDIR"/lib*.so

# 1) Build event structure from a clean state to avoid reusing stale 32-bit objects
make -C "$JETPICODIR" clean
make -C "$JETPICODIR" -j"$(nproc)" all
file "$JETPICODIR"/libTStarJetPico.so | grep -q 'ELF 64-bit'
cp -v "$JETPICODIR"/*.so "$LIBDIR"/

# 2) Build TStarJetPicoMaker + StRefMultCorr in a LOCAL build dir
export CXXFLAGS="-std=c++11 -pipe -fPIC -Wall -Woverloaded-virtual -Wno-long-long -pthread -m64 -I$JETPICODIR"

rsync -a --delete StRoot/TStarJetPicoMaker "$BUILDDIR/StRoot/"
rsync -a --delete StRoot/StRefMultCorr    "$BUILDDIR/StRoot/"

cd "$BUILDDIR" && cons CXXFLAGS="$CXXFLAGS" +StRefMultCorr +TStarJetPicoMaker 

# 3) Copy built libs to ./libs and ./sandbox
for lib in libTStarJetPicoMaker.so libStRefMultCorr.so; do
  find "$BUILDDIR"/.sl*/lib -name "$lib" -exec cp -v {} "$LIBDIR"/ \;
done

ls -ltr "$LIBDIR"/*.so || true

cd "$WORKDIR"
root4star -q -b -l ./macros/makeTStarJetPico.cxx