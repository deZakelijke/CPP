
#include <stdlib.h>
#include <stdio.h>
#include "ring_network_broadcast.h"
#include <mpi.h>


int MYMPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, 
                MPI_Comm communicator) {
    int numtasks, rank, tag = 1;
    MPI_Status stat;              
    MPI_Comm_size(communicator, &numtasks);
    MPI_Comm_rank(communicator, &rank);
    int next_add = (rank + 1) % numtasks;
    int prev_add = (rank - 1 + numtasks) % numtasks;

    if (rank == root) { 
        printf("Root %d is sending message: %s to node %d and %d\n", root, (char*)buffer, next_add, prev_add);
        MPI_Send(buffer, count, MPI_CHAR, next_add, tag, MPI_COMM_WORLD);
        MPI_Send(buffer, count, MPI_CHAR, prev_add, tag, MPI_COMM_WORLD);
    } else {
        if (end_node(numtasks, rank)) { 
            printf("Ending node %d is waiting for broadcast.\n", rank);
            if (numtasks%2 == 0) {
                printf("Single end node, will receive two broadcasts.\n");
                MPI_Recv(buffer, count, MPI_CHAR, prev_add, tag, communicator, &stat);
                MPI_Recv(buffer, count, MPI_CHAR, next_add, tag, communicator, &stat);
            } else {
                if (numtasks / 2 < rank) { // if ccw
                    printf("Ending node %d is receiving a singe broadcast from %d.\n", rank, next_add);
                    MPI_Recv(buffer, count, MPI_CHAR, next_add, tag, communicator, &stat);
                } else { // if cw
                    printf("Ending node %d is receiving a singe broadcast from %d.\n", rank, prev_add);
                    MPI_Recv(buffer, count, MPI_CHAR, prev_add, tag, communicator, &stat);
                }
            }
        } else if (numtasks / 2 < rank) { // if ccw
            printf("Node %d is waiting for broadcast from %d.\n", rank, next_add);
            MPI_Recv(buffer, count, MPI_CHAR, next_add, tag, communicator, &stat);
            printf("Node %d has received broadcast: %s.\n", rank, (char*)buffer);
            MPI_Send(buffer, count, MPI_CHAR, prev_add, tag, MPI_COMM_WORLD);
        } else  { // if cw
            printf("Node %d is waiting for broadcast from %d.\n", rank, prev_add);
            MPI_Recv(buffer, count, MPI_CHAR, prev_add, tag, communicator, &stat);
            printf("Node %d has received broadcast: %s.\n", rank, (char*)buffer);
            MPI_Send(buffer, count, MPI_CHAR, next_add, tag, MPI_COMM_WORLD);
        }
    }
    printf("Node %d is done with broadcast.\n", rank);
    return 0;
}

node* node_init(int size, int rank){
    node *new_node = malloc(sizeof (node));
    new_node->size = size;
    new_node->node_id = rank;
    new_node->buffer = malloc(sizeof (char) * size);
    return new_node;
}


void node_free(node *node_p){
    if (node_p == NULL) {
        printf("Trying to free NULL node.\n");
    } else {
        free(node_p->buffer);
        free(node_p);
    }
}


void load_message(node *node_p, char *message, int message_len){
    for (int i = 0; i < message_len; i++) {
        node_p->buffer[i] = message[i];
    }
}


void print_buffer(node *node_p){
    printf("Node %d has received the following message: \"%s\".\n", node_p->node_id, node_p->buffer);
}


int end_node(int l_i, int rank){
    float l = (float)l_i;
    if (l / 2 == (float)rank) {
        return 1;
    }
    if (l / 2 < rank && l / 2 > (rank - 1)) {
        return 1;
    }
    if (l / 2 > rank && l / 2 < (rank + 1)) {
        return 1;
    }
    return 0;
}


int main(int argc, char *argv[]) {
    // For now, only works with 0 as sender
    int sender_id = 0;
    int numtasks, rank;
    int message_len = 5;
    char message[5] = {'H', 'e', 'l', 'l', 'o'};
    char empty[5] = {'0', '0', '0', '0', '0'};
    node *node_p;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    node_p = node_init(message_len, rank);
    printf("Process %d has started.\n", rank);

    if (rank == sender_id) {
        load_message(node_p, message, message_len);
        MYMPI_Bcast(node_p->buffer, node_p->size, MPI_CHAR, rank, MPI_COMM_WORLD);
    } else {
        load_message(node_p, empty, message_len);
        MYMPI_Bcast(node_p->buffer, node_p->size, MPI_CHAR, sender_id, MPI_COMM_WORLD);
    }

    print_buffer(node_p);
    node_free(node_p);
    MPI_Finalize();
    return 0;
}
