#!/bin/bash

path=$PWD

for proc in "ZH" "2f_hadronic" "4f_WW_hadronic" "4f_ZZ_hadronic" "ZH"
do
    for pol in "eL_pR" "eR_pL"
    do
		folder_name="/data/dust/user/marquezh/QQbarAnalysis2025/scripts/2025_QQbar/NTuples/Big/${proc}/${pol}"
		ssbarfolder="/data/dust/user/marquezh/SSbarAnalysis/"
		if [ ! -d "$ssbarfolder/rootfiles/processed/big/$proc" ]; then
			mkdir -p $ssbarfolder/rootfiles/processed/big/$proc
		fi
		if [ ! -d "$ssbarfolder/rootfiles/processed/big/$proc/$pol" ]; then
			mkdir -p $ssbarfolder/rootfiles/processed/big/$proc/$pol
		fi
		# Verificar si el directorio existe
		if [ ! -d "$folder_name" ]; then
			echo "Directory $folder_name does not exist, skipping..."
			continue
		fi
		
     	cd $folder_name
    	echo $folder_name 
		
		# Verificar si hay archivos - método corregido
		FILES=(*)
		if [ ${#FILES[@]} -eq 1 ] && [ "${FILES[0]}" = "*" ]; then
			echo "No files found in $folder_name"
			cd $path
			continue
		fi
		
		echo "Found ${#FILES[@]} files in $folder_name"
		
     	i=0
  		for f in "${FILES[@]}"
    	do
 			name="00"$i
			if [ $i -gt 9 ]; then
    	     	name="0"$i
 			fi
 			if [ $i -gt 99 ]; then
         		name=$i
 			fi
			fullfilename="${folder_name}/${f}"
			echo file: $fullfilename
			cd $path  # Cambiar al directorio de trabajo
			
			cp ${path}/run_default.sh ${path}/run_${proc}_${pol}_${name}.sh
 			sed -i -e 's/xPOLx/'${pol}'/g' ${path}/run_${proc}_${pol}_${name}.sh
 			sed -i -e 's/xPROCx/'${proc}'/g' ${path}/run_${proc}_${pol}_${name}.sh
 			sed -i -e 's/xFILEx/'${f}'/g' ${path}/run_${proc}_${pol}_${name}.sh
 			sed -i -e 's:xFILENAMEx:'${fullfilename}':g' ${path}/run_${proc}_${pol}_${name}.sh
 			cp ${path}/run_default.sub ${path}/run_${proc}_${pol}_${name}.sub
 			sed -i -e 's/xPOLx/'${pol}'/g' ${path}/run_${proc}_${pol}_${name}.sub
 			sed -i -e 's/xPROCx/'${proc}'/g' ${path}/run_${proc}_${pol}_${name}.sub
 			sed -i -e 's/xNFILEx/'${name}'/g' ${path}/run_${proc}_${pol}_${name}.sub
			
			# Verificar si ya existe el archivo de salida
			if [ -f ${path}/${proc}_${pol}_${name}.root ]; then
				echo "Skip ${name}"
			else
				echo "Submit --- > ${name}"
				condor_submit run_${proc}_${pol}_${name}.sub
				sleep 0.1s
			fi
			
			i=$((i+1))  # Incrementar contador
		done
		
		cd $path  # Regresar al directorio original
	done
done