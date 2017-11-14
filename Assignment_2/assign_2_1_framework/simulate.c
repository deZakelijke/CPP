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
    int numtasks, rank, next, prev;
    MPI_Status stat;              // Moet dit een array zijn?

    MPI_Init();                   // ?
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Verschillende processen intialiseren

    // Master proces die de buffers swapt
    if (rank == 0) {

    } else {

    // Taak voor elk proces definieren
    // Communicatie na elke tijdstap
    // Moet synchroon om de goede antwoorden te krijgen
    // Alle communicatie blockende communicatie?


    }

    
    // De boel afsluiten
    MPI_Finalize();

    /* You should return a pointer to the array with the final results. */
    return current_array;
}
