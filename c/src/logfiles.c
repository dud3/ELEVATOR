#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

int lfChunkLen = 80, lfSize, lfRank, lfPos = 0;
MPI_Status lfStatus;
MPI_File lfFile;
MPI_Info lfInfo;
char *lfName = "./logFile.txt";

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
  char lfStrTime[1000];
  struct tm * p = localtime(&lfTime);
  strftime(lfStrTime, 1000, "%c", p);

  char lfBuffer[lfChunkLen+1];

  for (int i = 0; i < lfChunkLen; i++) {
    lfBuffer[i] = ' ';
  }

  char *newBuffer;
  newBuffer = strcpy(lfBuffer, "[");
  newBuffer = strcat(newBuffer, lfStrTime);
  // newBuffer = strcat(newBuffer, ctime(&lfTime));
  newBuffer = strcat(newBuffer, "] ");

  char const lfDigit[] = "0123456789";
  char b[] = "0";
  char c[] = "0";
  char* strID = b;
  strID[0] = lfDigit[ourID];
  char* floorID = c;
  *floorID = lfDigit[floor];

  if (whoAreWe == 0) {
    newBuffer = strcat(strcat(newBuffer, "Elevator "), strID);
  } else {
    newBuffer = strcat(strcat(newBuffer, "Person "), strID);
    newBuffer = strcat(strcat(strcat(newBuffer, " ("), name), ")");
  }

  strcat(strcat(strcat(newBuffer, msg), floorID), ".");
  
  int i = 0;
  while (lfBuffer[i] != '\0') {
    i += 1;
  }
  lfBuffer[i] = ' ';
  
  lfBuffer[lfChunkLen-1] = '\n';
  lfBuffer[lfChunkLen] = '\0';
  
  MPI_File_seek(lfFile, (lfPos * lfSize + lfRank) * lfChunkLen, MPI_SEEK_SET);

  MPI_File_write(lfFile, lfBuffer, lfChunkLen, MPI_CHAR, &lfStatus);

  lfPos += 1;

}

void closeLogFile() {

  MPI_File_close(&lfFile);

  printf("%d/%d closed their chunk of the log file.\n", lfRank+1, lfSize);

}

void wordCountLog(char* key, int value) {

  // what time is it now?
  time_t lfTime;
  time(&lfTime);
  char lfStrTime[1000];
  struct tm * p = localtime(&lfTime);
  strftime(lfStrTime, 1000, "%c", p);

  char lfBuffer[lfChunkLen+1];

  for (int i = 0; i < lfChunkLen; i++) {
    lfBuffer[i] = ' ';
  }

  char *newBuffer;
  newBuffer = strcpy(lfBuffer, "[");
  newBuffer = strcat(newBuffer, lfStrTime);
  // newBuffer = strcat(newBuffer, ctime(&lfTime));
  newBuffer = strcat(newBuffer, "] ");

  char const lfDigit[] = "0123456789";
  char b[] = "0";
  char* valID = b;
  valID[0] = lfDigit[value];

  newBuffer = strcat(strcat(strcat(newBuffer, key), ": "), valID);
  
  int i = 0;
  while (lfBuffer[i] != '\0') {
    i += 1;
  }
  lfBuffer[i] = ' ';
  
  lfBuffer[lfChunkLen-1] = '\n';
  lfBuffer[lfChunkLen] = '\0';
  
  MPI_File_seek(lfFile, (lfPos * lfSize + lfRank) * lfChunkLen, MPI_SEEK_SET);

  MPI_File_write(lfFile, lfBuffer, lfChunkLen, MPI_CHAR, &lfStatus);

  lfPos += 1;

}

