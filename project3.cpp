//Colin Sawyer
//CPSC 3220 Operating Systems Section 2
//Program 3
//Simulating thread concurrency with consumers and producers of molecules

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<ctime>
#include<unistd.h>
//number of atoms/molecules currently created
int carbon = 0, hydrogen = 0, oxygen = 0, ethanol = 0, water = 0, ozone = 0;

//time each thread will sleep after producing and the number produced every iteration
int carbon_sleep, oxygen_sleep, hydrogen_sleep, generateC, generateH, generateO;

//time each thread will sleep after consuming and the number of molecules required
int requiredE, ethanol_sleep, requiredW, water_sleep, requiredOz, ozone_sleep;
bool quit = false;

//create the locks, thread names, and conditional variables
pthread_mutex_t lockH, lockC, lockO, consumerLock;
pthread_t hp, op, cp, ec, oc, wc;
pthread_cond_t oxygen_cond, hydrogen_cond, carbon_cond;
using namespace std;

//thread for carbon producer
void * carbon_p(void * arg){
  while(!quit){
    //grab the lock and produce random number of atoms
    pthread_mutex_lock(&lockC);
    int c = (rand() % generateC) + 1;
    carbon += c;
    //print production, release lock, and go to sleep
    printf("Carbon producer runs - produces %i; C = %i\n", c, carbon);
    pthread_cond_signal(&carbon_cond);
    pthread_mutex_unlock(&lockC);
    usleep(carbon_sleep);
  }
}
//thread for oxygen producer
void * oxygen_p(void * arg){
  while(!quit){
    //grab the lock, produce random number of atoms
    pthread_mutex_lock(&lockO);
    int o = (rand() % generateO) + 1;
    oxygen += o;
    //print production, release lock, and go to sleep
    printf("Oxygen producer runs - produces %i; O = %i\n", o, oxygen);
    pthread_cond_signal(&oxygen_cond);
    pthread_mutex_unlock(&lockO);
    usleep(oxygen_sleep);
  }
}
//thread for hydrogen producer
void * hydrogen_p(void * arg){
  while(!quit){
    //grab the lock, produce random number of atoms
    pthread_mutex_lock(&lockH);
    int h = (rand() % generateH) + 1;
    hydrogen += h;
    //print production, release locks, and go to sleep
    printf("Hydrogen producer runs - produces %i; H = %i\n", h, hydrogen);
    pthread_cond_signal(&hydrogen_cond);
    pthread_mutex_unlock(&lockH);
    usleep(hydrogen_sleep);
  }
}
//thread for ozone consumer
void * ozone_c(void * arg){
  int currentOz = 0;
  //keep consuming until the quota is met
  while(currentOz < requiredOz){
    //acquire locks
    pthread_mutex_lock(&consumerLock);
    pthread_mutex_lock(&lockO);
    //get more resources if there is not enough for a molecule
    while(oxygen < 3){
      pthread_cond_wait(&oxygen_cond, &lockO);
    }
    oxygen -= 3;
    currentOz++;
    //print consumption, release locks, and go to sleep
    printf("Ozone consumer runs - consumes O3; O = %i\n", oxygen);
    pthread_mutex_unlock(&lockO);
    pthread_mutex_unlock(&consumerLock);
    usleep(ozone_sleep);
  }
  printf("%i Ozone molecules have been produced\n", currentOz);
}
//thread for ethanol consumer
void * ethanol_c(void * arg){
  int currentE = 0;
  //keep consuming until the quota is met
  while(currentE < requiredE){
    //acquire locks
    pthread_mutex_lock(&consumerLock);
    pthread_mutex_lock(&lockO);
    pthread_mutex_lock(&lockC);
    pthread_mutex_lock(&lockH);
    //get more resources if there is not enough for a molecule
    while(oxygen < 1) {pthread_cond_wait(&oxygen_cond, &lockO);}
    while(hydrogen < 6) {pthread_cond_wait(&hydrogen_cond, &lockH);}
    while(carbon < 2) {pthread_cond_wait(&carbon_cond, &lockC);}
    oxygen -= 1;
    hydrogen -= 6;
    carbon -= 2;
    currentE++;
    //print consumption, release locks, and go to sleep
    printf("Ethanol consumer runs - consumes C2H6O; C = %i, H = %i, O = %i\n", carbon, hydrogen, oxygen);
    pthread_mutex_unlock(&lockO);
    pthread_mutex_unlock(&lockC);
    pthread_mutex_unlock(&lockH);
    pthread_mutex_unlock(&consumerLock);
    usleep(ethanol_sleep);
  }
  printf("%i Ethanol molecules have been produced\n", currentE);
}
//thread for water consumer
void * water_c(void * arg){
  int currentW = 0;
  //keep consuming until quota is met
  while(currentW < requiredW){
    //acquire locks
    pthread_mutex_lock(&consumerLock);
    pthread_mutex_lock(&lockO);
    pthread_mutex_lock(&lockH);
    //get more resources if there is not enough to produce a molecule
    while(oxygen < 1) {pthread_cond_wait(&oxygen_cond, &lockO);}
    while(hydrogen < 2) {pthread_cond_wait(&hydrogen_cond, &lockH);}
    oxygen -= 1;
    hydrogen -= 2;
    currentW++;
    //print consumption, release locks, and go to sleep
    printf("Water consumer runs - consumes H2O; H = %i, O = %i\n", hydrogen, oxygen);
    pthread_mutex_unlock(&lockO);
    pthread_mutex_unlock(&lockH);
    pthread_mutex_unlock(&consumerLock);
    usleep(water_sleep);
  }
  printf("%i Water molecules have been produced\n", currentW);
}
int main(){
  //seed random number generator off of time
  srand(time(NULL));
  //parse all information from the input file into variables
  FILE *input = fopen("molecule.txt", "r");
  char line[100];
  fscanf(input, "%d", &generateC);
  fgets(line, 100, input);

  fscanf(input, "%d", &carbon_sleep);
  fgets(line, 100, input);

  fscanf(input, "%d", &generateH);
  fgets(line, 100, input);

  fscanf(input, "%d", &hydrogen_sleep);
  fgets(line, 100, input);

  fscanf(input, "%d", &generateO);
  fgets(line, 100, input);

  fscanf(input, "%d", &oxygen_sleep);
  fgets(line, 100, input);

  fscanf(input, "%d", &requiredE);
  fgets(line, 100, input);

  fscanf(input, "%d", &ethanol_sleep);
  fgets(line, 100, input);

  fscanf(input, "%d", &requiredW);
  fgets(line, 100, input);

  fscanf(input, "%d", &water_sleep);
  fgets(line, 100, input);

  fscanf(input, "%d", &requiredOz);
  fgets(line, 100, input);

  fscanf(input, "%d", &ozone_sleep);
  fgets(line, 100, input);
  //start the clock for the execution time of producing and consuming
  clock_t begin = clock();

  //create all of the threads and let them begin to run
  pthread_create(&op, NULL, &oxygen_p, NULL);
  pthread_create(&hp, NULL, &hydrogen_p, NULL);
  pthread_create(&cp, NULL, &carbon_p, NULL);
  pthread_create(&oc, NULL, &ozone_c, NULL);
  pthread_create(&wc, NULL, &water_c, NULL);
  pthread_create(&ec, NULL, &ethanol_c, NULL);
  //wait for consumers to finish
  pthread_join(oc, NULL);
  pthread_join(wc, NULL);
  pthread_join(ec, NULL);
  //tell producers to stop producing and wait for those threads to finish
  quit = true;
  pthread_join(op, NULL);
  pthread_join(hp, NULL);
  pthread_join(cp, NULL);
  //print results
  cout << "\nCarbon atoms produced = " << carbon + 2*requiredE << endl;
  cout << "Hydrogen atoms produced = " << hydrogen + 2*requiredW + 6*requiredE << endl;
  cout << "Oxygen atoms produced = " << oxygen + 3*requiredOz + requiredW + requiredE << endl;
  cout << requiredE << " Ethanol molecules produced" << endl;
  cout << requiredW << " Water molecules produced" << endl;
  cout << requiredOz << " Ozone molecules produced" << endl;
  clock_t end = clock();
  double totalTime = (double) (end - begin)/ CLOCKS_PER_SEC;
  cout << "\nExecution time: " << totalTime * 1000 << " milliseconds\n";
  return 0;
}
