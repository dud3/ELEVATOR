#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

const int SECONDS = 10.0;
double START;

typedef struct {

  int id;
  int state;

} Person;

enum State {New, Work, Move, Gone, Read, Write};
int id;
double t;

double rand_range(double min, double max) {

  return (double)rand()/RAND_MAX * (max - min) + min;

}

// return value, but not the number exclude
double rand_range_ex(int ex, int min, int max) {

  int random = ex;

  while (random == ex) {
    random = rand_range(min, max);
  }

  return random;
}


Person newPerson(int new_id) {
  Person person;
  person.id = new_id;
  person.state = New;
  return person;
}

void start_working(Person person) {

  t = MPI_Wtime() + rand_range(1, 4);
  person.state = Work;

}

void Simulate(Person person) {

  if (person.state == Work) {
    int now = MPI_Wtime();
    if (t < now) {
      person.state = Move;
    }
  }
}

int go_to_next_floor(int current_floor) {

  int random_value = rand() % 2;
  if (current_floor == 2) return random_value ? 3 : 4;
  if (current_floor == 3) return random_value ? 2 : 4;
  return random_value ? 2 : 3;

}


void Elevator(int rank, MPI_Comm comm) {

}

void Floor(int rank, MPI_Comm comm) {

}

int main(int argc, char **argv) {

  int size, rank, rc, root=0;

  MPI_Status status;
  MPI_File configFile = malloc(sizeof configFile);
  MPI_Info info;
  char *configFileName = "./configFile.txt";

  MPI_Init(&argc, &argv);

  MPI_Info_create(&info);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  printf("%d/%d started.\n", rank+1, size);

  char buf[21];

  rc = MPI_File_open(MPI_COMM_WORLD, configFileName,
                     MPI_MODE_RDONLY, info, &configFile);

  printf("%d/%d achieved the file_open result: %d.\n", rank+1, size, rc);

  // Set the individual pointer to 0
  rc = MPI_File_seek(configFile, 0, MPI_SEEK_SET);
 
  rc = MPI_File_read(configFile, buf, 20, MPI_CHAR, &status);
  buf[20] = '\0';

  rc = MPI_File_close(&configFile);

  char *ourname = buf;

  char str[100];
  printf(strcat (strcat (strcpy (str, "%d/%d has the name '"), ourname), "'.\n"), rank+1, size);

  // we check whether or not we are the root process
  if (rank == root) {
    // we do root-y stuff =)
  }
 
  printf("%d/%d ended.\n", rank+1, size);

  MPI_Finalize();
}
