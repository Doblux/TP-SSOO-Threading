#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>

#include <thread>

#include "CargarArchivos.hpp"

int cargarArchivo( HashMapConcurrente &hashMap, std::string filePath) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
        if (!file.is_open()) {
            std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
            return -1;
        }
        while (file >> palabraActual) { // esta guardando el archivo en palabra actual
            // Completar (Ejercicio 4)
            hashMap.incrementar(palabraActual);
            cant++;
        }
        // Cierro el archivo.
        if (!file.eof()) {
            std::cerr << "Error al leer el archivo" << std::endl;
            file.close();
            return -1;
        }
    file.close();
    return cant;
}

// 1 thread por archivo explicitamente en el enunciado
void cargarMultiplesArchivos( HashMapConcurrente &hashMap, unsigned int cantThreads, std::vector<std::string> filePaths ) {
    // Completar (Ejercicio 4)
    std::vector<std::thread> threads;

    if (cantThreads > filePaths.size()){ // 1 thread por cada archivo y listo
        for (int i = 0; i < filePaths.size(); i++)
        {
            threads.emplace_back(
                cargarArchivo, hashMap, filePaths[i]
            );
        }
        
    } else {
        int i = 0;
        while (i < filePaths.size()){
            for (int j = 0; j < cantThreads && i < filePaths.size(); j++) // podamos por cantidad de archivos
            {
                threads.emplace_back(
                    cargarArchivo, hashMap, filePaths[j]
                );
                i++;
            }
            
            for (int k = 0; k < cantThreads && i < filePaths.size(); k++) // podamos por cantidad de archivos
            {
                threads[k].join(); // seria como wait hasta q los threads terminen
            }
            
        }
    }

}

#endif
