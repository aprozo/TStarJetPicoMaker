#!/usr/bin/env bash
set -euo pipefail

WORKDIR=/gpfs01/star/pwg/prozorov/TStarJetPicoMaker
cd "$WORKDIR"

JETPICODIR="$PWD/StRoot/eventStructuredAu"
BUILDDIR="$PWD/build"
LIBDIR="$PWD/libs"

mkdir -p "$LIBDIR"  "$BUILDDIR/StRoot"
rm -f "$LIBDIR"/lib*.so

# 1) Build event structure
make -C "$JETPICODIR" -j"$(nproc)" all
cp -v "$JETPICODIR"/*.so "$LIBDIR"/

# 2) Build TStarJetPicoMaker + StRefMultCorr in a LOCAL build dir
export CXXFLAGS="-std=c++11 -pipe -fPIC -Wall -Woverloaded-virtual -Wno-long-long -pthread -I$JETPICODIR -m32"

rsync -a --delete StRoot/TStarJetPicoMaker "$BUILDDIR/StRoot/"
rsync -a --delete StRoot/StRefMultCorr    "$BUILDDIR/StRoot/"

cd "$BUILDDIR" && cons CXXFLAGS="$CXXFLAGS" +StRefMultCorr +TStarJetPicoMaker 

# 3) Copy built libs to ./libs and ./sandbox
for lib in libTStarJetPicoMaker.so libStRefMultCorr.so; do
  find "$BUILDDIR"/.sl*/lib -name "$lib" -exec cp -v {} "$LIBDIR"/ \;
done

ls -ltr "$LIBDIR"/*.so || true

cd "$WORKDIR"
root4star -q -b ./macros/makeTStarJetPico.cxx