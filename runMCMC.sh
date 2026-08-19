 #!/bin/bash                                                                                                                                                                                                                               
 CONFIG=build/Configs/EventRates_BeamFDOldUnc.yaml
 OUTPUT=Results/DuneOutputOld${1}.root
 source build/bin/setup.MaCh3DUNE.sh
 export OMP_NUM_THREADS=8
 Fit $CONFIG General:OutputFile:${OUTPUT} General:Systematics:XsecStepScale:${1}
 echo "Complete"
    
