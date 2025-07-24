#!/bin/bash
# file name: kt_xNAMEfile.sh

path="/data/dust/user/marquezh/SSbarAnalysis"
source $path/init_ilcsoft.sh

# Copiar el ejecutable
cp ${path}/main.exe .

# Copiar archivos de configuración necesarios
cp -r /data/dust/user/marquezh/SSbarAnalysis/etc .

# Crear directorios de salida necesarios
mkdir -p rootfiles/tmp_root

echo $PWD

./main.exe xFILENAMEx
sleep 3s
ogname="xFILEx"
ssfiletmp="$(basename "${ogname%.root}").tmp.root"
ssfile="$(basename "${ssfiletmp%.tmp.root}").root"
mv rootfiles/tmp_root/$ssfiletmp ${path}/rootfiles/processed/big/xPROCx/xPOLx/$ssfile

#for file in rootfiles/tmp_root/*.tmp.root; do
#    # Quitar la parte final .tmp.root y poner .root
#    newname="$(basename "${file%.tmp.root}").root"
#    # Copiar con el nuevo nombre
#    cp "$file" "${path}/rootfiles/proccessed/processed/big/xPROCx/xPOLx/$newname"
#done
