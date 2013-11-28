#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "main.h"
#include "logfiles.h"

const int SECONDS = 10.0;
double START;

typedef struct {

  int id;
  int state;

} Person;

enum State { Work, Elevator, Waiting };

int rand_num(int max) {
	return (rand() % max) + 1;
}

int main(int argc, char **argv) {

  int size, rank, rc, root = 0, nameLength = 20;

  MPI_Status status;
  MPI_File configFile = malloc(sizeof configFile);
  MPI_Info info;
  char *configFileName = "./configFile.txt";

  createLogFile();

  MPI_Init(&argc, &argv);
  
  initLogFile();

  MPI_Info_create(&info);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  printf("%d/%d started.\n", rank+1, size);

  char buf[nameLength + 1];

  rc = MPI_File_open(MPI_COMM_WORLD, configFileName,
                     MPI_MODE_RDONLY, info, &configFile);

  printf("%d/%d achieved the file_open result: %d.\n", rank+1, size, rc);

  // set the individual pointer to our position in the config file
  // master is the master of elevators
  rc = MPI_File_seek(configFile, rank * nameLength, MPI_SEEK_SET);
 
  rc = MPI_File_read(configFile, buf, nameLength, MPI_CHAR, &status);
  buf[nameLength] = '\0';
  int len = nameLength - 1;
  while ((len >= 0) && (buf[len] == ' ')) {
    buf[len] = '\0';
    len--;
  }

  rc = MPI_File_close(&configFile);

  char *ourname = buf;

  char str[50 + nameLength];
  printf(strcat (strcat (strcpy (str, "%d/%d has the name '"), ourname), "'.\n"), rank+1, size);

  // we check whether or not we are the root process
  if (rank == root) {
	  master(rank);
  }
  else {
	  worker(rank, ourname);
  }
 
  closeLogFile();
  
  printf("%d/%d ended.\n", rank+1, size);

  MPI_Finalize();
}

const int MAX_FLOOR = 3;

void master(int rank) {
	srand(time(NULL));
	
	//master manages elevators
	int floor = 1;
	bool up = true;
	
	while (true) {
		if (up) {
			//going up
			if (floor + 1 <= MAX_FLOOR) {
				floor++;
			}
			else {
				//at max floor, now going down.
				up = false;
				floor--;
			}
		}
		else {
			//going down
			if (floor - 1 >= 1) {
				floor--;
			}
			else {
				//at ground floor, go back up
				up = true;
				floor++;
			}
		}

        dumpLog(0, 1, "", " is now on floor ", floor);
		printf("elevator now on floor %d\n", floor);
		//tell workers what floor we're on
		MPI_Bcast(&floor, 1, MPI_INT, rank, MPI_COMM_WORLD);
		
		//elevator very fast, only 3 seconds to get in or out
		//also possibly dangerous to occupants.
		sleep(5);
	}
}

/**
 * Rank is processor number, name is the name of the person toiling away.
 */
void worker(int rank, char* name) {
	srand(time(NULL));
	
	int workTicks = rand_num(5);
	int currWorkTick = 0;
	int myFloor = rand_num(3);
	int desiredFloor = myFloor;
	enum State state = Work;
	
	while (true) {
		int floorOfElevator = 0;

		//receive what floor the elevator is on
		MPI_Bcast(&floorOfElevator, 1, MPI_INT, 0, MPI_COMM_WORLD);

		if (state == Elevator) {
            dumpLog(1, rank, name, " is riding the elevator to floor ", floor);
			printf("%d is riding the elevator to floor %d\n", rank, desiredFloor);
			
			//are we on the right floor? if so, state becomes work
			if (floorOfElevator == desiredFloor) {
                dumpLog(1, rank, name, " got off elevator and is now working on floor ", floor);
				printf("%d got off elevator and is now working on floor %d\n", rank, desiredFloor);
				state = Work;
				myFloor = desiredFloor;
			}
		}
		else if (state == Work) {
			//do work
			currWorkTick++;
            dumpLog(1, rank, name, " did some work on floor ", floor);
			printf("%d did some work on floor %d [%d/%d]\n", rank, myFloor, currWorkTick, workTicks);
			
			//receive our next task and wait for the elevator
			if (currWorkTick >= workTicks) {
                dumpLog(1, rank, name, " is done with work on floor ", floor);
				printf("%d is done with work on floor %d\n", rank, myFloor);
				currWorkTick = 0;
				workTicks = rand_num(5);
				desiredFloor = rand_num(3);
				state = Waiting;
			}
		}
		else if (state == Waiting) {
            dumpLog(1, rank, name, " is waiting for the elevator to go to floor ", floor);
			printf("%d is waiting for the elevator to go to floor %d from floor %d\n", rank, desiredFloor, myFloor);
			if (floorOfElevator == myFloor) {
                dumpLog(1, rank, name, " got on the elevator to go to floor ", floor);
				printf("%d got on the elevator to go to floor %d\n", rank, desiredFloor);
				state = Elevator;
			}
		}
	}
}
