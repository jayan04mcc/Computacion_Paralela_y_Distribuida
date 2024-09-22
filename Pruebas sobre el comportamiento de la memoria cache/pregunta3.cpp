#include <iostream>
#include <vector>
#include <chrono>
#include <random>
using namespace std;

void multiplyBlock(double **A, double **B, double **C, int N, int blockSize) {
    for (int i = 0; i < N; i += blockSize) {
        for (int j = 0; j < N; j += blockSize) {
            for (int k = 0; k < N; k += blockSize) {
                // Multiplicación de bloques
                for (int ii = i; ii < min(i + blockSize, N); ++ii) {
                    for (int jj = j; jj < min(j + blockSize, N); ++jj) {
                        for (int kk = k; kk < min(k + blockSize, N); ++kk) {
                            C[ii][jj] += A[ii][kk] * B[kk][jj];
                        }
                    }
                }
            }
        }
    }
}
int main() {
    int N = 1000;  // Tamaño de la matriz NxN
    int blockSize = 2;  // Tamaño del bloque (puedes ajustar este valor)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0.0,10.0);
    double **A = new double *[N];
    double **B = new double *[N];
    double **C = new double *[N];   

    for (size_t i = 0; i < N; i++)
    {
        A[i]= new double [N];
        B[i]= new double [N];
        C[i]= new double [N];
    }
    for (size_t i =0 ; i<N; i++){
        for (size_t j=0 ; j<N;j++){
            A[i][j] = dis(gen);
            B[i][j] = dis(gen);
        }
    }
    auto start = chrono::high_resolution_clock::now();
    multiplyBlock(A, B, C, N, blockSize);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;

    cout << "Tiempo de ejecución (por bloques): " << diff.count() << " s\n";
    return 0;
}