#!/usr/bin/env bash

root4star -q -b -l ./macros/makeTStarJetPico.cxx 2>&1 \
  | awk '!(/TStarJetPicoMaker:ERROR - TRefTable::Add : SetParent must be called before adding uid=[0-9]+/ || /StTriggerSimuMaker:INFO[[:space:]]+- /)'

# exit ${PIPESTATUS[0]}
