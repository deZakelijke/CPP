/*
 * Sieve of erathostenes in parallel
 *
 * By Micha de Groot and Leon Bartelsman
 *
 */

#define BUF_SIZE 16

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include <signal.h>

typedef struct{
    int data[BUF_SIZE];
    int buffer_in;
    int buffer_out;
    int contains;
    pthread_mutex_t buff_lock;
    pthread_cond_t items;
    pthread_cond_t space;
} buffer;

int end;
int counter = 0;


buffer* init_buffer(void){
    buffer* new_buffer = malloc(sizeof (buffer));
    new_buffer->buffer_in = 0;
    new_buffer->buffer_out = 0;
    new_buffer->contains = 0;
    pthread_mutex_init(&new_buffer->buff_lock, NULL);
    pthread_cond_init(&new_buffer->items, NULL);
    pthread_cond_init(&new_buffer->space, NULL);
    return new_buffer;
}


void to_buffer(int number, buffer* buff){
    pthread_mutex_lock(&buff->buff_lock);
    while(buff->contains >= BUF_SIZE){
        pthread_cond_wait(&buff->space, &buff->buff_lock);
    }

    buff->data[buff->buffer_in] = number;
    buff->buffer_in = (buff->buffer_in + 1) %BUF_SIZE;
    buff->contains++;

    pthread_cond_signal(&buff->items);
    pthread_mutex_unlock(&buff->buff_lock);
}

int from_buffer(buffer* buff){
    int number;
    pthread_mutex_lock(&buff->buff_lock);
    while(buff->contains == 0){
        pthread_cond_wait(&buff->items, &buff->buff_lock);
    }

    number = buff->data[buff->buffer_out];
    buff->buffer_out = (buff->buffer_out + 1) %BUF_SIZE;
    buff->contains--;

    pthread_cond_signal(&buff->space);
    pthread_mutex_unlock(&buff->buff_lock);
    return number;
}

void* generator_func(void* arg){
    buffer* buff = (buffer*) arg; 
    int number = 2;
    while(1){
        to_buffer(number, buff);
        number++;
    }
    return NULL;
}


void* work_func(void* arg){
    if (counter >= end) {
        double time = timer_end();
        printf("It took %f seconds.\n", time);
        exit(0);
    }
    buffer *buff_in = (buffer*)arg;
    buffer *buff_out = init_buffer();
    int first_number = from_buffer(buff_in);
    int number;
    int first_prime_found = 1;
    pthread_t next;
    printf("%d\n", first_number);
    counter++;

    while(1){
        number = from_buffer(buff_in);
        if (number%first_number){
            to_buffer(number, buff_out);
            if (first_prime_found){
                pthread_create(&next, NULL, work_func, (void*)buff_out);
                first_prime_found = 0;
            }
        }
    }
    return NULL;
}


int main(int argc, char* argv[]) {
    if (argc <= 1){
        end = 100;
    } else {
        end = atoi(argv[1]);
    }
    timer_start();
    pthread_t gen_thread;
    pthread_t work_thread;
    buffer* start_buffer = init_buffer();

    pthread_create(&gen_thread, NULL, generator_func, start_buffer);
    pthread_create(&work_thread, NULL, work_func, start_buffer);

    
    pthread_join(gen_thread, NULL);

    return 0;
}
