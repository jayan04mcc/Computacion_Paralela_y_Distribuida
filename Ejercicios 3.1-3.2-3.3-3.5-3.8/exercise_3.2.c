#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv) {
    int rank, size;
    long long int total_tosses, local_tosses, number_in_circle = 0, global_in_circle;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("ingrese el num total de lanzamientos ");
        scanf_s("%lld", &total_tosses);
    }

  
    MPI_Bcast(&total_tosses, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // Calculate the number of tosses for each process
    local_tosses = total_tosses / size;

   
    unsigned int seed = (unsigned int)(rank + 1);

    // Perform the Monte Carlo simulation
    for (long long int i = 0; i < local_tosses; i++) {
        double x = (double)rand() / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand() / RAND_MAX * 2.0 - 1.0;

        if (x * x + y * y <= 1.0) {
            number_in_circle++;
        }
    }

    
    MPI_Reduce(&number_in_circle, &global_in_circle, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Estimate pi
        double pi_estimate = 4.0 * (double)global_in_circle / (double)total_tosses;
        printf("Valor estimado de pi: %lf\n", pi_estimate);
    }

    MPI_Finalize();
    return 0;
}
