all: matrixcalc

matrixcalc: matrixcalc.c
	gcc -o matrixcalc matrixcalc.c -l pthread