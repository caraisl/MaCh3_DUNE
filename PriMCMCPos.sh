#!/bin/bash
CONFIG=build/Configs/EventRates_BeamFD${1}Unc.yaml
PRIORCONFIG=build/Configs/EventRates_BeamFD${1}UncPrior.yaml
POSTOUTPUT=Results/Dune${1}UncPost.root
echo $CONFIG
source build/bin/setup.MaCh3DUNE.sh
export OMP_NUM_THREADS=8
Predictive $PRIORCONFIG
Fit $CONFIG
echo $CONFIG General:OutputFile:${POSTOUTPUT}
Predictive $CONFIG General:OutputFile:${POSTOUTPUT}
echo "Complete"
