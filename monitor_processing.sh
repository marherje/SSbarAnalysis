#!/bin/bash

# Script para monitorear el procesamiento de datos
LOG_FILE="processing_monitor.log"

echo "$(date): Starting monitoring..." >> $LOG_FILE

while true; do
    # Verificar si main.exe está corriendo
    if pgrep -f "main.exe" > /dev/null; then
        PID=$(pgrep -f "main.exe")
        echo "$(date): main.exe running with PID: $PID" >> $LOG_FILE
        
        # Información de memoria y CPU
        ps -p $PID -o pid,ppid,cmd,pmem,pcpu,rss,vsz,etime >> $LOG_FILE
        
        # Información del sistema
        echo "$(date): System memory:" >> $LOG_FILE
        free -h >> $LOG_FILE
        
        # Verificar si el proceso está colgado (no usa CPU)
        CPU_USAGE=$(ps -p $PID -o pcpu --no-headers | tr -d ' ')
        echo "$(date): Current CPU usage: ${CPU_USAGE}%" >> $LOG_FILE
        
        # Si CPU < 0.1% por más de 5 minutos, algo puede estar mal
        if (( $(echo "$CPU_USAGE < 0.1" | bc -l) )); then
            echo "$(date): WARNING: Low CPU usage detected!" >> $LOG_FILE
        fi
        
        # Verificar archivos de salida
        if [ -f rootfiles/tmp_root/*.tmp.root ]; then
            ls -lh rootfiles/tmp_root/*.tmp.root >> $LOG_FILE
        fi
        
    else
        echo "$(date): main.exe not running" >> $LOG_FILE
    fi
    
    echo "----------------------------------------" >> $LOG_FILE
    sleep 600  # Verificar cada 10 minutos
done
