#!/bin/sh
#PBS -N ElevatorJob
#PBS -lnodes=4
mpirun ./main.exe
