#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

int lfChunkLen = 100, lfSize, lfRank;
MPI_Status lfStatus;
MPI_File lfFile;
MPI_Info lfInfo;
char *lfName = "./logFile.txt";
char *lfBuffer;

void createLogFile() {

}

void initLogFile() {

  MPI_Info_create(&lfInfo);

  MPI_Comm_size(MPI_COMM_WORLD, &lfSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &lfRank);

  printf("%d/%d initialized their chunk of the log file.\n", lfRank+1, lfSize);

  int rc = MPI_File_open(MPI_COMM_WORLD, lfName,
                         MPI_MODE_CREATE | MPI_MODE_RDWR, lfInfo, &lfFile);

  printf("%d/%d achieved the file_open result %d for their chunk of the log file.\n", lfRank+1, lfSize, rc);

}

// whoAreWe .. 0 for elevator, 1 for person
// ourID .. which number are we
// floor .. where are we
void dumpLog(int whoAreWe, int ourID, char* name, char* msg, int floor) {

  // what time is it now?
  time_t lfTime;
  time(&lfTime); 

  char *newBuffer;
  newBuffer = strcat(strcat(strcpy(lfBuffer, "["), ctime(&lfTime)), "] ");

  char const lfDigit[] = "0123456789";
  char *strID = "0";
  strID[0] = lfDigit[ourID];
  char *floorID = "0";
  floorID[0] = lfDigit[floor];

  if (whoAreWe == 0) {
    newBuffer = strcat(strcat(newBuffer, "Elevator "), strID);
  } else {
    newBuffer = strcat(strcat(newBuffer, "Person "), strID);
    newBuffer = strcat(strcat(strcat(newBuffer, " ("), name), ")");
  }

  strcat(strcat(strcat(newBuffer, msg), floorID), ".\n");
  
  int i = 0;
  
  while (lfBuffer[i] != '\0') {
    i++;
  }

  MPI_File_write(lfFile, lfBuffer, i-1, MPI_CHAR, &lfStatus);

  printf("%d/%d did some serious log file writing.\n", lfRank+1, lfSize);

}

void closeLogFile() {

  MPI_File_close(&lfFile);

  printf("%d/%d closed their chunk of the log file.\n", lfRank+1, lfSize);

}

