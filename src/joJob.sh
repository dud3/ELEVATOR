#!/bin/sh
#PBS -N ElevatorJob
#PBS -m abe
#PBS -lnodes=4
mpirun ~/ELEVATOR/main.exe
