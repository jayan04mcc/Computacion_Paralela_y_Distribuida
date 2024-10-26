#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void merge(int* arr1, int n1, int* arr2, int n2, int* result) {
    int i = 0, j = 0, k = 0;
    while (i < n1 && j < n2) {
        if (arr1[i] <= arr2[j]) {
            result[k++] = arr1[i++];
        }
        else {
            result[k++] = arr2[j++];
        }
    }
    while (i < n1) result[k++] = arr1[i++];
    while (j < n2) result[k++] = arr2[j++];
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int comm_sz, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int n;
    int* local_data = NULL;
    int* other_data = NULL;

    if (my_rank == 0) {
        // Proceso 0 lee n y lo envía a todos los procesos
        n = 16; // Ejemplo de tamaño total de datos
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_n = n / comm_sz;
    local_data = (int*)malloc(local_n * sizeof(int));

    // Cada proceso genera datos aleatorios y los ordena localmente
    srand(my_rank + 1);
    for (int i = 0; i < local_n; i++) {
        local_data[i] = rand() % 100;
    }
    qsort(local_data, local_n, sizeof(int), compare);

    // Proceso 0 recopila e imprime las listas locales ordenadas (sin combinarlas)
    if (my_rank == 0) {
        printf("Listas locales ordenadas:\n");
        for (int i = 0; i < local_n; i++) printf("%d ", local_data[i]);
        printf("\n");

        int* other_data = (int*)malloc(local_n * sizeof(int));
        for (int p = 1; p < comm_sz; p++) {
            MPI_Recv(other_data, local_n, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int i = 0; i < local_n; i++) printf("%d ", other_data[i]);
            printf("\n");
        }
        free(other_data);
    }
    else {
        MPI_Send(local_data, local_n, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Combinar los datos en una estructura de árbol
    int step = 1;
    int* temp_data = NULL;
    while (step < comm_sz) {
        if (my_rank % (2 * step) == 0) {
            if (my_rank + step < comm_sz) {
                temp_data = (int*)malloc(local_n * step * sizeof(int));
                MPI_Recv(temp_data, local_n * step, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int* merged_data = (int*)malloc(local_n * (step + 1) * sizeof(int));
                merge(local_data, local_n * step, temp_data, local_n * step, merged_data);
                free(local_data);
                free(temp_data);
                local_data = merged_data;
                local_n *= 2;
            }
        }
        else {
            int destination = my_rank - step;
            MPI_Send(local_data, local_n, MPI_INT, destination, 0, MPI_COMM_WORLD);
            break;
        }
        step *= 2;
    }

    // El proceso 0 imprime la lista combinada final
    if (my_rank == 0) {
        printf("Lista global combinada:\n");
        for (int i = 0; i < n; i++) printf("%d ", local_data[i]);
        printf("\n");
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}