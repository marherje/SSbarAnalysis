#!/bin/bash

if [ ! -e NTuples ]; then
    mkdir NTuples
fi

if [ ! -e NTuples/Big ]; then
    mkdir NTuples/Big
fi

for proc in "2f_hadronic" "4f_WW_hadronic" "4f_ZZ_hadronic" "ZH"
do
    if [ ! -e NTuples/Big/${proc} ]; then
            mkdir NTuples/Big/${proc}
        fi
    for pol in "eL_pR" "eR_pL"
    do
	if [ ! -e NTuples/Big/${proc}/${pol} ]; then
	    mkdir NTuples/Big/${proc}/${pol}
	fi
	mv ${proc}*${pol}*.root NTuples/Big/${proc}/${pol}/.
    rm *${proc}*${pol}*.txt
    rm *${proc}*${pol}*.sh
    rm *${proc}*${pol}*.sub
    rm *${proc}*${pol}*.log
    rm *${proc}*${pol}*.xml
    done
done
			     
