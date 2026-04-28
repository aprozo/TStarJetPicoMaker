#!/usr/bin/env bash

# check if not in singularity already
if [ -z "$SINGULARITY_NAME" ]; then
    SIF=/cvmfs/star.sdcc.bnl.gov/containers/rhic_sl7.sif
    BINDS="-B /direct -B /star -B /afs -B /gpfs -B /sdcc/lustre02"

    # Run inside the SL7 container; csh sources ~/.cshrc which sets up root4star.
    singularity exec -e $BINDS $SIF csh -c "cd $PWD && root4star -q -b -l ./macros/makeTStarJetPico.cxx"

else
   root4star -q -b -l ./macros/makeTStarJetPico.cxx
fi


