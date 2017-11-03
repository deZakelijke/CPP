/*
 * simulate.h
 */

#pragma once
typedef struct{
    double* old;
    double* current;
    double* new_A;
    int begin_i;
    int end_i;
    float c;
}arguments;

double *simulate(const int i_max, const int t_max, const int num_cpus,
        double *old_array, double *current_array, double *next_array);

arguments *init_arguments(double* old, double* current, double* new_A, int begin_i, int end_i, int c);

void free_arguments(arguments* args);

void* simulation_step(void* arguments);


