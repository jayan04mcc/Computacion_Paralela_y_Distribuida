#include <stdio.h>
#include <chrono>
#include <vector>
#include <iostream>
#define MAX 1000

int main() {
    
    double** A = new double* [MAX]; 
    for (int i = 0; i < MAX; i++) {
        A[i] = new double[MAX]; 
    }

    double* x = new double[MAX]; 
    double* y = new double[MAX]; 

    for (int i = 0; i < MAX; i++) {
        x[i] = 1.0;
        y[i] = 0.0;
        for (int j = 0; j < MAX; j++) {
            A[i][j] = 1.0;
        }
    }
    auto inicio = std::chrono::high_resolution_clock::now();
    // Primer par de bucles
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
    auto fin = std::chrono::high_resolution_clock::now();

    // Reiniciar y a 0 antes del segundo par de bucles
    for (int i = 0; i < MAX; i++) {
        y[i] = 0.0;
    }
    auto inicio2 = std::chrono::high_resolution_clock::now();
    // Segundo par de bucles
    for (int j = 0; j < MAX; j++) {
        for (int i = 0; i < MAX; i++) {
            y[i] += A[i][j] * x[j];
        }
    }
    auto fin2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> contador1 = fin - inicio;
    std::chrono::duration<double> contador2 = fin2 - inicio2;

    std::cout << "Tiempo del primer bucle: " << contador1.count() << " segundos\n";
    std::cout << "Tiempo del segundo bucle: " << contador2.count() << " segundos\n";

    for (int i = 0; i < MAX; i++) {
        delete[] A[i]; 
    }
    delete[] A; 
    delete[] x; 
    delete[] y; 

    return 0;
}