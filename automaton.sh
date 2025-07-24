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

#for pol in "eL_pR" #"eR_pL"
#do
#	for prod in "2f_hadronic" #"4f_WW_hadronic" "4f_ZZ_hadronic" "ZH" 
#	do
#    echo "Processing ${pol} ${prod}"
#    ./main.exe /data/dust/user/marquezh/QQbarAnalysis2025/scripts/2025_QQbar/NTuples/small_test_NTuples/${pol}_${prod}_merged.root
#    sleep 3s
#    mv rootfiles/tmp_root/${pol}_${prod}_merged.tmp.root rootfiles/processed/small/${pol}_${prod}_merged.root
#  done  
#done

for pol in "eL_pR" #"eR_pL"
do
	for prod in "2f_hadronic" #"4f_WW_hadronic" "4f_ZZ_hadronic" "ZH" 
	do
    echo "$(date): Processing ${pol} ${prod}"
    echo "$(date): Memory before processing:"
    free -h
    echo "$(date): Starting main.exe..."
    
    # Ejecutar con timeout y monitoreo
    timeout 7200 ./main.exe /data/dust/user/marquezh/QQbarAnalysis2025/scripts/2025_QQbar/NTuples/Big/${pol}_${prod}_merged.root &
    PID=$!
    
    # Monitorear el proceso
    while kill -0 $PID 2>/dev/null; do
        echo "$(date): Process $PID still running..."
        ps -p $PID -o pid,ppid,cmd,pmem,rss,vsz,etime
        sleep 30
    done
    
    wait $PID
    EXIT_CODE=$?
    
    echo "$(date): Process finished with exit code: $EXIT_CODE"
    echo "$(date): Memory after processing:"
    free -h
    
    if [ $EXIT_CODE -eq 0 ]; then
        echo "$(date): Successfully processed ${pol} ${prod}"
        if [ -f rootfiles/tmp_root/${pol}_${prod}_merged.tmp.root ]; then
            mv rootfiles/tmp_root/${pol}_${prod}_merged.tmp.root rootfiles/processed/big/${pol}_${prod}_merged.root
            echo "$(date): Output file moved successfully"
        else
            echo "$(date): ERROR: Output file not found!"
        fi
    else
        echo "$(date): ERROR: Processing failed for ${pol} ${prod} with exit code $EXIT_CODE"
    fi
    
    sleep 3s
  done  
done