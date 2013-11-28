#!/bin/sh
#PBS -N ElevatorJob
#PBS -lnodes=4
module load openmpi
mpirun ~/ELEVATOR/main.exe
