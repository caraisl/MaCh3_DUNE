#!/bin/bash
source build/bin/setup.MaCh3DUNE.sh
export OMP_NUM_THREADS=8
Fit build/Configs/EventRates_BeamFDStandardRecord.yaml
echo "Complete"
