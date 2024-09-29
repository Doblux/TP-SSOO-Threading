#ifndef HMC_HPP
#define HMC_HPP

#include <mutex>
#include <array>
#include <atomic>
#include <string>
#include <vector>

#include "ListaAtomica.hpp"


typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
 public:
    static constexpr int cantLetras = 26;

    HashMapConcurrente();

    void incrementar(std::string clave);
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);

    float promedio();
    
    std::pair<std::string, unsigned int> promedioParalelo(unsigned int cantThreads);

 private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
    
    static unsigned int hashIndex(std::string clave);

    std::mutex mutex_tabla[HashMapConcurrente::cantLetras];
    
    void promedio_tuneado(unsigned int& nro_thread, float& sum, unsigned int& count, unsigned int& nro_tabla, unsigned int& cantThreads);
};

#endif  /* HMC_HPP */
