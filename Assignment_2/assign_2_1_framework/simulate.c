/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "simulate.h"


/* Add any global variables you may need. */


/* Add any functions you may need (like a worker) here. */

// Initializes a smaller part of the array for a process.
double *init_arr(double *array, int i_max, int rank, int numtasks);

// Calculates the next iteration in the simulation. 
void update_array(double *prev, double *curr, double *next, int length);

// Puts the local array back in the original array at the end of the sequence.
void put_result(double *mother_arr, double *child_arr, int base, int length);


double *init_arr(double *array, int i_max, int rank, int numtasks){
    int length, base;
    if (rank <= i_max%numtasks) {
        length = i_max/numtasks + 3;
        base = ((i_max/numtasks)+rank) *rank;
    } else {
        length = i_max/numtasks + 2;
        base = ((i_max/numtasks)+i_max%numtasks) * rank;
    }
    if (rank == 1 || rank == numtasks-1) {
        length--;
    }
   
    double *new_arr = malloc(sizeof (double) * length);
    for (int i = 0; i < length; i++){
        new_arr[i] = array[base+i];
    }

    return new_arr;
}


void update_array(double *prev, double *curr, double *next, int length){
    for (int i = 1; i < length-2; i++) {
        next[i] = 2*curr[i] - prev[i] + 0.5*( curr[i-1] - 
                  2 * (curr[i] - curr[i+1]));
    }
}


void put_result(double *mother_arr, double *child_arr, int base, int length){
    for (int i = 1; i <length-2; i++) {
        mother_arr[i+base] = child_arr[i];
    }
}


/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, double *old_array,
        double *current_array, double *next_array)
{
    // Variabelen initialiseren
    int numtasks, rank, tag=1, length;
    int next_add, prev_add, next_val, prev_val;
    double *prev_l, *curr_l, *next_l;
    MPI_Status stat;              

    MPI_Init(NULL, NULL);                   
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Verschillende processen intialiseren
    // Elk proces moet zijn eigen geheugen krijgen
    // Verdeel de array en voeg padding toe


    // Master proces die de buffers swapt
    if (rank == 0) {
        // every timestep, swap buffers
    } else {
        prev_l = init_arr(old_array, i_max, rank, numtasks);
        curr_l = init_arr(current_array, i_max, rank, numtasks);
        next_l = init_arr(next_array, i_max, rank, numtasks);

        next_add = rank + 1;
        prev_add = rank - 1;
        // Taak voor elk proces definieren

        for (int i = 0; i < t_max; i++) {

            update_array(prev_l, curr_l, next_l, length);
            if (rank == 1) {
                next_l[0] = 0;
            }
            if (rank == numtasks){
                next_l[length-1] = 0;
            }

            // Communicatie na elke tijdstap
            // Moet synchroon om de goede antwoorden te krijgen
            // De oneven processen verezenden eerst
            if (rank%2 == 0) {
                next_val = next_l[length-1];
                prev_val = next_l[1];
                MPI_Send(&prev_val, 1, MPI_INT, prev_add, tag, MPI_COMM_WORLD);
                MPI_Recv(&prev_val, 1, MPI_INT, prev_add, tag, MPI_COMM_WORLD, &stat);
                next_l[0] = prev_val;

                MPI_Send();
                MPI_Recv();
            } else {
                MPI_Recv(&prev_val, 1, MPI_INT, prev_add, tag, MPI_COMM_WORLD, &stat);
                MPI_Send();
                MPI_Recv();
                MPI_Send();
            }

        }
        put_result(current_array, curr_l, base, length);
        free(prev_l);
        free(curr_l);
        free(next_l);
    }

    
    // De boel afsluiten
    MPI_Finalize();

    /* You should return a pointer to the array with the final results. */
    return current_array;
}
