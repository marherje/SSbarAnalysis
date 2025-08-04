#!/bin/bash

if [ ! -e rootfiles ]; then
    mkdir rootfiles
fi

if [ ! -e rootfiles/processed ]; then
    mkdir rootfiles/processed
fi

if [ ! -e rootfiles/processed/small ]; then
    mkdir rootfiles/processed/small
fi

if [ ! -e rootfiles/processed/big ]; then
    mkdir rootfiles/processed/big
fi

for pol in "eL_pR" #"eR_pL"
do
	for prod in "2f_hadronic" #"4f_WW_hadronic" "4f_ZZ_hadronic" "ZH" 
	do
    echo "Processing ${pol} ${prod}"
    ./main.exe /data/dust/user/marquezh/QQbarAnalysis2025/scripts/2025_QQbar/NTuples/small_test_NTuples/${pol}_${prod}_merged.root
    sleep 3s
    mv rootfiles/tmp_root/${pol}_${prod}_merged.tmp.root rootfiles/processed/small/${pol}_${prod}_merged.root
  done  
done