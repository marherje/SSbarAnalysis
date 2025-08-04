#!/bin/bash
path="/data/dust/user/marquezh/SSbarAnalysis"

if [ ! -e ${path}/rootfiles ]; then
    mkdir ${path}/rootfiles
fi

if [ ! -e ${path}/rootfiles/merged ]; then
    mkdir ${path}/rootfiles/merged
fi

for proc in "2f_hadronic" #"4f_WW_hadronic" "4f_ZZ_hadronic" "ZH"
do
    for pol in "eL_pR" "eR_pL"
    do
	if [ ! -e ${path}/rootfiles/processed/big/${proc}/${pol} ]; then
	    echo "Directory ${path}/rootfiles/processed/big/${proc}/${pol} does not exist, skipping..."
        continue
	fi
    echo "Processing ${proc} ${pol}"
    ./hadd_large_files.sh ${path}/rootfiles/merged/${proc}_${pol}_merged.root "${path}/rootfiles/processed/big/${proc}/${pol}/*"
    done
done
			     
