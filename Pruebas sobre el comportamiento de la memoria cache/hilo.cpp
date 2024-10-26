#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

std::mutex mtx; // Mutex para proteger la variable max en la simulación de escritura concurrente

// Función para comparar elementos en paralelo
void compare(const std::vector<int>& A, std::vector<bool>& m, int i) {
    int n = A.size();
    for (int j = 0; j < n; j++) {
        if (A[j] > A[i]) {
            m[i] = true; // Si encontramos un elemento mayor, descartamos A[i] como posible máximo
        }else{
            m[i]= false;
            break;
        }
    }
}

// Función para encontrar el máximo basado en los valores marcados en m[]
void findMax(const std::vector<int>& A, const std::vector<bool>& m, int& max) {
    for (int i = 0; i < A.size(); i++) {
        if (m[i]) { // Si m[i] es verdadero, A[i] es el mayor
            std::lock_guard<std::mutex> lock(mtx); // Protegemos la escritura concurrente
            max = A[i];
        }
    }
}

int main() {
    // Arreglo de ejemplo
    std::vector<int> A = {5, 6, 9, 2, 9, 7, 8, 3};
    int n = A.size();
    std::vector<bool> m(n, true); // Inicializamos m[] con "verdadero"
    int max = -1; // Para almacenar el máximo valor

    // Vector de hilos
    std::vector<std::thread> threads;

    // Comparaciones en paralelo
    for (int i = 0; i < n; i++) {
        threads.push_back(std::thread(compare, std::ref(A), std::ref(m), i));
    }

    // Esperamos a que todos los hilos terminen
    for (auto& th : threads) {
        th.join();
    }

    // Reiniciamos los hilos para buscar el valor máximo
    threads.clear();

    // En paralelo, encontramos el máximo
    threads.push_back(std::thread(findMax, std::ref(A), std::ref(m), std::ref(max)));

    // Esperamos que el hilo termine
    for (auto& th : threads) {
        th.join();
    }

    // Imprimimos el valor máximo
    std::cout << "El valor máximo es: " << max << std::endl;

    return 0;
}
