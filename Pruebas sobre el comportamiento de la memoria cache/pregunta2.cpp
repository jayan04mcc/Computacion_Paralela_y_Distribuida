#include <iostream>
#include <random>
#define MAX 10000  // Tamaño de las matrices
#include <chrono>

int main() {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0.0,10.0);
    double **A = new double *[MAX];
    double **B = new double *[MAX];
    double **C = new double *[MAX];
    

    for (size_t i = 0; i < MAX; i++)
    {
        A[i]= new double [MAX];
        B[i]= new double [MAX];
        C[i]= new double [MAX];
    }

    for (size_t i =0 ; i<MAX; i++){
        for (size_t j=0 ; j<MAX;j++){
            A[i][j] = dis(gen);
            B[i][j] = dis(gen);
        }
    }
    auto inicio =std::chrono::high_resolution_clock::now();
   
    for (int i = 0; i < MAX; ++i) {
        for (int j = 0; j < MAX; ++j) {
            for (int k = 0; k < MAX; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    auto fin =std::chrono::high_resolution_clock::now();

    
    std::chrono::duration<double> contador1 = fin - inicio;
   

    std::cout << "Tiempo del primer bucle: " << contador1.count() << " segundos\n";

    
    for (int i = 0; i < MAX; i++) {
        delete[] A[i]; 
    }
    delete[] A; 
    delete[] B; 
    delete[] C; 

    return 0;
} 