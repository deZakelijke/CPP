

#include "mpi.h"

typedef struct{
    char *buffer;
    int size;
    int node_id;
} node;

// Initialize a network node.
node* node_init(int size, int rank);

// Free a network node.
void node_free(node *node_p);

// Load a starting message into the buffer.
void load_message(node *node_p, char *message, int message_len);

// Print the content of the buffer.
void print_buffer(node *node_p);

// Check if a node is the end node in the point-to-point broadcast.
int end_node(int l_i, int rank, int root);

// Broadcast a message.
int MYMPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, 
                MPI_Comm communicator);


