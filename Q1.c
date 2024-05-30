#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>
#include <time.h>


#define INFINITY INT_MAX

/*int graph[NUM_CITIES][NUM_CITIES] = {
    {0, 10, 15, 20, 25},
    {10, 0, 35, 25, 30},
    {15, 35, 0, 30, 45},
    {20, 25, 30, 0, 55},
    {25, 30, 45, 55, 0}
};*/

// writing dynamic code for graph of cities

#define NUM_CITIES 200
#define MAX_DISTANCE 120

int graph[NUM_CITIES][NUM_CITIES];

void generate_symmetric_graph() {
    srand(time(NULL)); // Initialize random seed

    for (int i = 0; i < NUM_CITIES; i++) {
        for (int j = i + 1; j < NUM_CITIES; j++) {
            int distance = rand() % (MAX_DISTANCE + 1); // Generate random distance
            graph[i][j] = distance;
            graph[j][i] = distance; // Mirror the distance
        }
        graph[i][i] = 0; // Set diagonal elements to 0
    }
}

void print_graph() {
    printf("Graph:\n");
    for (int i = 0; i < NUM_CITIES; i++) {
        for (int j = 0; j < NUM_CITIES; j++) {
            printf("%d\t", graph[i][j]);
        }
        printf("\n");
    }
}


int best_path[NUM_CITIES];
int best_cost = INFINITY;

int calculate_cost(int path[]) {
    int cost = 0;
    for (int i = 0; i < NUM_CITIES - 1; i++) {
        cost += graph[path[i]][path[i + 1]];
    }
    cost += graph[path[NUM_CITIES - 1]][path[0]]; // Return to the starting city
    return cost;
}

void two_opt(int path[]) {
    int improved = 1;
    while (improved) {
        improved = 0;
        for (int i = 0; i < NUM_CITIES - 1; i++) {
            for (int j = i + 1; j < NUM_CITIES; j++) {
                int new_path[NUM_CITIES];
                for (int k = 0; k < i; k++) {
                    new_path[k] = path[k];
                }
                for (int k = i, l = j; k <= j; k++, l--) {
                    new_path[k] = path[l];
                }
                for (int k = j + 1; k < NUM_CITIES; k++) {
                    new_path[k] = path[k];
                }
                int new_cost = calculate_cost(new_path);
                if (new_cost < calculate_cost(path)) {
                    for (int k = 0; k < NUM_CITIES; k++) {
                        path[k] = new_path[k];
                    }
                    improved = 1;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    clock_t start_time, end_time;
    double seq_time, par_time;
    
    generate_symmetric_graph();
    //print_graph();

    // Sequential TSP Solver
    start_time = clock();
    int seq_path[NUM_CITIES];
    for (int i = 0; i < NUM_CITIES; i++) {
        seq_path[i] = i;
    }
    two_opt(seq_path);
    int seq_cost = calculate_cost(seq_path);
    end_time = clock();
    seq_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    // Parallel TSP Solver
    start_time = clock();
    int chunk_size = NUM_CITIES / size;
    int start = rank * chunk_size;
    int end = start + chunk_size;

    int local_path[NUM_CITIES];
    for (int i = 0; i < NUM_CITIES; i++) {
        local_path[i] = i;
    }
    two_opt(local_path);

    int local_cost = calculate_cost(local_path);

    MPI_Allreduce(&local_cost, &best_cost, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

    if (best_cost == local_cost) {
        for (int i = 0; i < NUM_CITIES; i++) {
            best_path[i] = local_path[i];
        }
    }

    MPI_Bcast(best_path, NUM_CITIES, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    end_time = clock();
    par_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    if (rank == 0) {
        printf("Sequential TSP Solver:\n");
        printf("Best Path: ");
        for (int i = 0; i < NUM_CITIES; i++) {
            printf("%d ", seq_path[i]);
        }
        printf("\nBest Cost: %d\n", seq_cost);
        printf("Time taken: %f seconds\n\n", seq_time);

        printf("Parallel TSP Solver:\n");
        printf("Best Path: ");
        for (int i = 0; i < NUM_CITIES; i++) {
            printf("%d ", best_path[i]);
        }
        printf("\nBest Cost: %d\n", best_cost);
        printf("Time taken: %f seconds\n", par_time);
    }

    return 0;
}

