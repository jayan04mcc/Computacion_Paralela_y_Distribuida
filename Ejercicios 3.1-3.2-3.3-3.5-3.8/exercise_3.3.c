#include <stdio.h>
#include <mpi.h>

int main(void) {
    int comm_sz, my_rank;
    int local_sum, global_sum = 0;  // Inicializamos global_sum a 0

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    local_sum = my_rank; // Ejemplo de valor local para cada proceso

    int step = 1;
    while (step < comm_sz) {
        if (my_rank % (2 * step) == 0) {
            if (my_rank + step < comm_sz) {  // Solo recibe si el proceso está dentro del rango
                int received_sum;
                MPI_Recv(&received_sum, 1, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_sum += received_sum;
            }
        } else {
            int destination = my_rank - step;
            MPI_Send(&local_sum, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
            break; // El proceso envía su valor y luego sale del ciclo
        }
        step *= 2;
    }

    // El proceso 0 muestra el resultado final
    if (my_rank == 0) {
        global_sum = local_sum;
        printf("Global sum is %d\n", global_sum);
    }

    MPI_Finalize();
    return 0;
}
