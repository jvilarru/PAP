#!/bin/bash        
if [ -z "$4" ]; then 
	echo Usage: $0 nivell_optimització[0,1,2,3] programa[matmul,dotprod,dotprod-basic] NUMTHREADS TAMANYS_PROBLEMA
	exit
fi
OPT=$1
PROG=$2
NT=$3
VL=$4
gcc -pthread -I ./ -O$OPT -o $PROG $PROG.c
./$PROG $NT $VL
	
