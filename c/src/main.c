#define _GNU_SOURCE
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
#include "dlist.h"

//map-reduce:
/*
  master currently sends out the floor of the elevator.
  the children need to calculate what line of the file to read, and then
  count the words on that line. basically whenever we are doing work,
  we count the words in the file. each thing of work is just
  one line count, no need for work ticks. it's easy to log words in the
  file, but harder to combine them. guess we'll just skip that part...

  to calculate lines to read, we need max # of nodes. each child will keep
  track of the line it's supposed to be reading. then each time we add the
  max # of nodes to the line. for example:
    - we have 10 nodes, 100 lines in file.
	- node 1 reads line 1, 11, 21, etc.
	- node 2 reads line 2, 12, 22, etc.
	
  so eventually we will read everything. but how do we handle the
  issue of going past the end of the file? we will read in the entire
  file and query the size of the file, that's how!

  since using mpi i/o to read this file is going to be difficult, instead read
  in the entire file first. then each pocessor will query part of the dynamic
  2d array (i.e. list of lines) and count words! won't read the 10gb file
  but at least it will be a finished assignment...

  optimization that should allow 10gb file: main function uses
  mathematical trickery to only store into memory line that match the
  ones it should process. so each processor will read the entire file
  (which will be stupid slow), but they will only store in memory
  pieces of the file, following the same logic shown above in the
  example. with enough nodes, becomes possible to read 10gb file.
*/

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

  //read in the file to be counted, line by line.
  FILE * fp;
  char * line = NULL;
  size_t lineLen = 0;
  ssize_t read;
  
  dlist* list = NULL;
  dlist_create(10, &list);
  printf("size of list is %d\n", list->size);

  for (int c = 0; c < 15; c++) {
  	  char buf[5]; //long enough for test + num, e.g. test1
  	  sprintf(buf, "test%d", c);
	  printf("cap before call is %d\n", list->capacity);
	  dlist_append(&list, buf);
	  printf("cap after call is %d\n", list->capacity);
  }

  char* thestring;


  printf("capacity is %d\n", list->capacity);
  for (int c = 0; c < 15; c++) {
	  dlist_get(&list, c, &thestring);
	  printf("string is: %s\n", thestring);
  }
  
  fp = fopen(argv[1], "r");
  if (fp == NULL) {
	  MPI_Finalize();
	  exit(EXIT_FAILURE);
  }
  
  while ((read = getline(&line, &lineLen, fp)) != -1) {
	  printf("Retrieved line of lengt %zu :\n", read);
	  printf("%s", line);
  }
  
  if (line)
	  free(line);
  
  // we check whether or not we are the root process.
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
	srand(time(NULL) + rank);
	
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

        dumpLog(0, 1, "", " now passes floor ", floor);
		printf("elevator now passes floor %d\n", floor);
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
	srand(time(NULL) + rank);
	
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
            dumpLog(1, rank, name, " is on the elevator to floor ", desiredFloor);
			printf("%d is riding the elevator to floor %d\n", rank, desiredFloor);
			
			//are we on the right floor? if so, state becomes work
			if (floorOfElevator == desiredFloor) {
                dumpLog(1, rank, name, " got off the elevator on floor ", desiredFloor);
				printf("%d got off elevator and is now working on floor %d\n", rank, desiredFloor);
				state = Work;
				myFloor = desiredFloor;
			}
		}
		else if (state == Work) {
			//do work
			currWorkTick++;
            dumpLog(1, rank, name, " did some work on floor ", myFloor);
			printf("%d did some work on floor %d [%d/%d]\n", rank, myFloor, currWorkTick, workTicks);
			
			//receive our next task and wait for the elevator
			if (currWorkTick >= workTicks) {
                dumpLog(1, rank, name, " is done with work on floor ", myFloor);
				printf("%d is done with work on floor %d\n", rank, myFloor);
				currWorkTick = 0;
				workTicks = rand_num(5);
				desiredFloor = myFloor;
				while (desiredFloor == myFloor) {
					desiredFloor = rand_num(3);
				}
				state = Waiting;
			}
		}
		else if (state == Waiting) {
            dumpLog(1, rank, name, " waits before going to floor ", desiredFloor);
			printf("%d is waiting for the elevator to go to floor %d from floor %d\n", rank, desiredFloor, myFloor);
			if (floorOfElevator == myFloor) {
                dumpLog(1, rank, name, " got on the elevator to floor ", desiredFloor);
				printf("%d got on the elevator to go to floor %d\n", rank, desiredFloor);
				state = Elevator;
			}
		}
	}
}
