/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 *
 * By Micha de Groot and Leon Bartelsman
 */

#include <stdio.h>
#include <stdlib.h>

#include "simulate.h"
#include <pthread.h>


/* Add any global variables you may need. */


/* Add any functions you may need (like a worker) here. */

// Hier moeten we iig de functie definieren die we aan elke thread meegeven.
arguments *init_arguments(double* old, double* current, double* new_A, int begin_i, int end_i, int c){
    arguments* args = malloc(sizeof (arguments));
    args->old = old;
    args->current = current;
    args->new_A = new_A;
    args->begin_i = begin_i;
    args->end_i = end_i;
    args->c = c;

    return args;
}

void free_arguments(arguments* args);

// Nog een losse functie die een tijdstap doet.

// Buffer swap functie? Alleen de pointers doorwisselen. XOR swap?


void* simulation_step(void* args){

   double* old = ((arguments *)args)->old;
   double* current = ((arguments *)args)->current;
   double* new_A = ((arguments*)args)->new_A;
   int begin_i = ((arguments *)args)->begin_i;
   int end_i = ((arguments *)args)->end_i;
   float c = ((arguments *)args)->c;

    /*
    new[begin:end] = 2 * current[begin:end] - old[begin:end] + 
                    c *(current[begin-1:end-1] - 2*current[begin:end] - 
                    current[begin+1:end+1]);
    */

    for(int i = begin_i; i < end_i; i++){
        new_A[i] = 2 * current[i] - old[i] + c *(current[i-1] - 2*current[i] - current[i+1]);
    }
    return NULL;
}

/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * num_threads: how many threads to use (excluding the main threads)
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{
    float c = 0.15;
    double* temp;
    pthread_t threads[num_threads];
    arguments args[num_threads];

    for (int j = 0; j < t_max; j++){

        for (int i = 0;  i< num_threads; i++){
            args[i].old = old_array;
            args[i].current = current_array;
            args[i].new_A = next_array;
            args[i].begin_i = i/num_threads;
            args[i].end_i = (i+1)/num_threads;
            args[i].c = c;
        }
    
        for (int i = 0; i < num_threads; i++){
            pthread_create(&threads[i], NULL, simulation_step, &args[i]);
        }
    
    
    
        for (int i = 0; i < num_threads; i++){
            pthread_join(threads[i], NULL);
        }
        /*
         * After each timestep, you should swap the buffers around. Watch out none
         * of the threads actually use the buffers at that time.
         */
    
        temp = old_array;
        old_array = current_array;
        current_array = next_array;
        next_array = temp;

    }
    /* You should return a pointer to the array with the final results. */
    return current_array;
}
