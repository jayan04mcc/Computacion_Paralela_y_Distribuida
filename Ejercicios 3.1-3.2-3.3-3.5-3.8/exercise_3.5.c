#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void read_matrix_vector(double* A, double* x, int n) {
    for (int i = 0; i < n * n; i++) {
        A[i] = rand() % 10;  // Ejemplo, inicializa con valores aleatorios.
    }
    for (int i = 0; i < n; i++) {
        x[i] = rand() % 10;  // Ejemplo, inicializa con valores aleatorios.
    }
}

int main(int argc, char** argv) {
    int comm_sz, my_rank, n;
    double* A = NULL, * x = NULL, * local_A, * local_y, * y;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        // Leer el tamaño de la matriz (n)
        printf("Ingrese el orden de la matriz (n): ");
        scanf_s("%d", &n);

        // Verificar que n sea divisible por comm_sz
        if (n % comm_sz != 0) {
            printf("Error: n debe ser divisible por el número de procesos.\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }

        // Asignar memoria para la matriz y el vector en el proceso 0
        A = (double*)malloc(n * n * sizeof(double));
        x = (double*)malloc(n * sizeof(double));
        y = (double*)malloc(n * sizeof(double));

        // Leer los valores de la matriz y el vector
        read_matrix_vector(A, x, n);
    }

    // Distribuir n y x a todos los procesos
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (my_rank != 0) {
        x = (double*)malloc(n * sizeof(double));
    }
    MPI_Bcast(x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Asignar memoria para y en cada proceso
    y = (double*)malloc(n * sizeof(double)); // Esto asegura que todos los procesos tengan y

    // Cada proceso tiene n/comm_sz columnas de la matriz
    int local_n = n / comm_sz;
    local_A = (double*)malloc(n * local_n * sizeof(double));
    local_y = (double*)malloc(n * sizeof(double));

    // Distribuir las columnas de A entre los procesos
    MPI_Datatype column_type;
    MPI_Type_vector(n, local_n, n, MPI_DOUBLE, &column_type);
    MPI_Type_commit(&column_type);

    if (my_rank == 0) {
        // Enviar columnas a cada proceso
        for (int i = 1; i < comm_sz; i++) {
            MPI_Send(&A[i * local_n], 1, column_type, i, 0, MPI_COMM_WORLD);
        }

        // Copiar columnas locales en local_A
        for (int i = 0; i < local_n; i++) {
            for (int j = 0; j < n; j++) {
                local_A[j * local_n + i] = A[j * n + i];
            }
        }
    }
    else {
        MPI_Recv(local_A, n * local_n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Calcular el producto de la submatriz local por el vector
    for (int i = 0; i < n; i++) {
        local_y[i] = 0.0;
        for (int j = 0; j < local_n; j++) {
            local_y[i] += local_A[i * local_n + j] * x[my_rank * local_n + j];
        }
    }

    // Combinar resultados en el proceso 0
    MPI_Reduce_scatter(local_y, y,&local_n, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    // Imprimir el resultado en el proceso 0
    if (my_rank == 0) {
        printf("Resultado del producto matriz-vector:\n");
        for (int i = 0; i < n; i++) {
            printf("%f\n", y[i]);
        }
    }

    // Liberar memoria
    if (my_rank == 0) {
        free(A);
        free(x);
        
    }
    free(local_A);
    free(local_y);
    free(y);
    MPI_Type_free(&column_type);
    MPI_Finalize();
    return 0;
}
