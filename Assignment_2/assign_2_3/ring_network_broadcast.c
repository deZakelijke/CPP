/*
 *  Made by Micha de Groot
 *  
 *  This program creates a ring-network where each node can only message 
 *  its neighbour. One node, specified by the user, will broadcast a 
 *  message to all other nodes.
 *
 */

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
        printf("Node %d is sending message: %s \n", root, (char*)buffer);
        MPI_Send(buffer, count, MPI_CHAR, next_add, tag, MPI_COMM_WORLD);
        MPI_Send(buffer, count, MPI_CHAR, prev_add, tag, MPI_COMM_WORLD);
    } else {
        if (end_node(numtasks, rank, root)) { 
            if (numtasks%2 == 0) {
                MPI_Recv(buffer, count, MPI_CHAR, prev_add, tag, communicator, &stat);
                MPI_Recv(buffer, count, MPI_CHAR, next_add, tag, communicator, &stat);
            } else {
                if (numtasks / 2 < (rank - root + numtasks) % numtasks) { // if ccw
                    MPI_Recv(buffer, count, MPI_CHAR, next_add, tag, communicator, &stat);
                } else { // if cw
                    MPI_Recv(buffer, count, MPI_CHAR, prev_add, tag, communicator, &stat);
                }
            }
        } else if (numtasks / 2 < (rank - root + numtasks) % numtasks) { // if ccw
            MPI_Recv(buffer, count, MPI_CHAR, next_add, tag, communicator, &stat);
            MPI_Send(buffer, count, MPI_CHAR, prev_add, tag, MPI_COMM_WORLD);
        } else  { // if cw
            MPI_Recv(buffer, count, MPI_CHAR, prev_add, tag, communicator, &stat);
            MPI_Send(buffer, count, MPI_CHAR, next_add, tag, MPI_COMM_WORLD);
        }
    }
    return 0;
}

node* node_init(int size, int rank){
    node *new_node = malloc(sizeof (node));
    if (new_node == NULL) {
        printf("Failed to init node.\n");
    }
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


int end_node(int l_i, int rank, int root){
    float l = (float)l_i;
    if (l / 2 == (float)rank) {
        return 1;
    }
    if (l / 2 < (rank-root+l_i) % l_i  && l / 2 > (rank-1-root+l_i) % l_i) {
        return 1;
    }
    if (l / 2 > (rank-root+l_i) % l_i  && l / 2 < (rank+1-root+l_i) % l_i) {
    //if (l / 2 > rank && l / 2 < (rank + 1)) {
        return 1;
    }
    return 0;
}


int main(int argc, char *argv[]) {
    int sender_id=0;
    int message_len;
    char def_mes[6] = {'H', 'e', 'l', 'l', 'o', '!'};
    if (argc > 1) {
        sender_id = atoi(argv[1]);
        message_len = atoi(argv[2]);
    } else {
        printf("Format for input: <sending node> <message size> <message>\n");
        printf("wrong input format, will revert to default settings:\n");
        printf("Sending node:0, message size: 6, message: Hello!\n");
        message_len = 6;
    }

    char message[message_len];
    char empty[message_len];
    if (argc > 3) {
        for (int i = 0; i < message_len; i++) {
            message[i] = argv[3][i];
            empty[i] = '0';
        }
    } else {
        for (int i = 0; i < message_len; i++) {
            message[i] = def_mes[i];
            empty[i] = '0';
        }
    }

    int numtasks, rank;
    node *node_p;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (sender_id >= numtasks) { 
        sender_id = 0;
    }


    node_p = node_init(message_len, rank);

    if (rank == sender_id) {
        load_message(node_p, message, message_len);
        MYMPI_Bcast(node_p->buffer, node_p->size, MPI_CHAR, rank, MPI_COMM_WORLD);
    } else {
        load_message(node_p, empty, message_len);
        MYMPI_Bcast(node_p->buffer, node_p->size, MPI_CHAR, sender_id, MPI_COMM_WORLD);
    }

    if (rank != sender_id) {
        print_buffer(node_p);
    }
    node_free(node_p);
    MPI_Finalize();
    return 0;
}
