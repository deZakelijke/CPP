/*
 * simulate.h
 */

#pragma once

// Get the length of the local array.
int get_l(int i_max, int rank, int numtasks);

// Get the starting index of the original array.
int get_b(int i_max, int rank, int numtasks);

// Initializes a smaller part of the array for a process.
double *init_arr(double *array, int length, int base);

// Calculates the next iteration in the simulation. 
void update_array(double *prev, double *curr, double *next, int length);

// Puts the local array back in the original array at the end of the sequence.
void put_result(double *mother_arr, double *child_arr, int base, int length);


double *simulate(const int i_max, const int t_max, double *old_array,
        double *current_array, double *next_array);
