#!/bin/bash

# Script para hacer hadd con archivos grandes de manera eficiente
# Uso: ./hadd_large_files.sh output.root "input_pattern"

OUTPUT_FILE=$1
INPUT_PATTERN=$2
BATCH_SIZE=30  # Número de archivos a procesar por lote

if [ "$#" -ne 2 ]; then
    echo "Uso: $0 output.root 'input_pattern'"
    echo "Ejemplo: $0 merged.root 'data_*.root'"
    exit 1
fi

# Configurar variables de entorno para archivos grandes
export ROOT_HIST=32000
export ROOT_FLUSHER=1000000
export ROOT_CACHESIZE=100000000
export ROOT_TTREE_CLUSTER_SIZE=32000
export ROOT_COMPRESSION_LEVEL=1

# Obtener lista de archivos
FILES=($(ls ${INPUT_PATTERN}))
TOTAL_FILES=${#FILES[@]}

echo "Total de archivos a procesar: $TOTAL_FILES"
echo "Procesando en lotes de $BATCH_SIZE archivos"

# Procesar en lotes
TEMP_FILES=()
BATCH_COUNT=0

for ((i=0; i<$TOTAL_FILES; i+=$BATCH_SIZE)); do
    BATCH_COUNT=$((BATCH_COUNT + 1))
    TEMP_OUTPUT="temp_batch_${BATCH_COUNT}.root"
    
    # Crear lista de archivos para este lote
    BATCH_FILES=()
    for ((j=i; j<i+$BATCH_SIZE && j<$TOTAL_FILES; j++)); do
        BATCH_FILES+=("${FILES[j]}")
    done
    
    echo "Procesando lote $BATCH_COUNT: archivos $((i+1)) a $((i+${#BATCH_FILES[@]}))"
    
    # Ejecutar hadd para este lote
    hadd -f -v 1 "$TEMP_OUTPUT" "${BATCH_FILES[@]}"
    
    if [ $? -eq 0 ]; then
        TEMP_FILES+=("$TEMP_OUTPUT")
        echo "Lote $BATCH_COUNT completado exitosamente"
    else
        echo "Error en lote $BATCH_COUNT"
        exit 1
    fi
done

# Combinar todos los archivos temporales
echo "Combinando ${#TEMP_FILES[@]} archivos temporales..."
hadd -f -v 1 "$OUTPUT_FILE" "${TEMP_FILES[@]}"

# Limpiar archivos temporales
if [ $? -eq 0 ]; then
    echo "Limpiando archivos temporales..."
    rm -f "${TEMP_FILES[@]}"
    echo "Proceso completado exitosamente: $OUTPUT_FILE"
else
    echo "Error al combinar archivos temporales"
    exit 1
fi
