/*
//Yusreen Shah
//Sources:GeeksforGeeks, slides, and assignment 2
//With Starvation  
//Compile: gcc Monkey.c -lpthread
Note: it is supposed to go infinitely.
**/
#include <pthread.h>

#include <semaphore.h>

#include <stdio.h>

#include <stdbool.h>

#define N 5 //number of monkeys
# define WAITING 2 //waiting to cross
# define IMPATIENT 1 //wants to cross
# define CROSSING 0 //is crossing
# define EAST 4 //is going eastward
# define WEST 5 //is going westward

//the variables
int state[N]; //Store state of monkeys
int dir[N]; // Store direction of monkeys
int count[N] = {0,0,0,0,0}; //Store number of times each monkey has crossed
int mon[N] = {1,2,3,4,5};//all monkeys
int currentDir = WEST; //get the direction that is in motion

//Semaphores 
sem_t mutex; //Mutex is used such that no two monkeys may access board or unboard at the same time.
sem_t S[N]; // the semaphores for each monkey
/**
This function is used to make sure that the monkey is moving in the same direction as the previous one.
It's current state is IMPATIENT, which means that it wants to cross.
If it's direction is the same as current, it can cross and its state is changed to WAITING
*/

void test(int phnum) {
  //Set the direction of the monkey, it can be east or west. 
  if (count[phnum] % 2 == 0) {
    dir[phnum] = WEST;

    printf("Monkey %d wants to go west.", phnum);
  } else {
    dir[phnum] = EAST;
    printf("Monkey %d wants to go east.", phnum);
  }
  //If monkey wants to cross and there is 
  if (state[phnum] == IMPATIENT && dir[phnum] == currentDir) {
    // state that it can cross
    state[phnum] = CROSSING;
    count[phnum]++; //keep track of the number of times the monkey has crossed
    sleep(2);
    printf("Monkey %d is crossing \n", phnum + 1);
    sem_post( & S[phnum]); //unlocks the semaphore for Monkey

  }
}
/**
This function sees if the monkey can get on the rope
It waits for the signal to get on the rope using a semaphore
*/

void on_rope(int phnum) {
  // state that it is impatient
  state[phnum] = IMPATIENT;
  printf("Monkey %d wants to cross\n", phnum + 1);
  // see if any monkey is crossing in its direction
  test(phnum);
  // if unable to cross, wait to be signalled 
  sem_wait( & S[phnum]);

  sleep(1);
}

/**
This function sees that the monkey gets off the rope.
It then changes its state from CROSSING to WAITING.
*/
void off_rope(int phnum) {
  // change state to WAITING 
  state[phnum] = WAITING;

  printf("Monkey %d is on the other side\n", phnum + 1);
  printf("Monkey %d is now waiting\n", phnum + 1);
  if (count[phnum] % 2 == 0) {
    currentDir = WEST;
  } else {
    currentDir = EAST;

  }
}
/**
This function is for each monkey, so that they can wait, and cross.
*/
void * monkey(void * num) {

  while (1) {

    int * i = num;

    sleep(1);

    on_rope( * i); //Climb if possible

    sleep(0);

    off_rope( * i); //get off rope.
  }
}

int main() {

  int i;
  pthread_t thread_id[N];

  // initialize the semaphores 
  sem_init( & mutex, 0, 1);

  for (i = 0; i < N; i++)

    sem_init( & S[i], 0, 0);

  for (i = 0; i < N; i++) {

    // create monkey processes 
    pthread_create( & thread_id[i], NULL,
      monkey, & mon[i]);

    printf("Monkey %d is waiting\n", i + 1);
  }

  for (i = 0; i < N; i++)

    pthread_join(thread_id[i], NULL);
}
