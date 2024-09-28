#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

#include <mutex>

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

/*
void incrementar(string clave). Si clave existe en la tabla (en la lista
del bucket correspondiente), se debe incrementar su valor en uno. Si no
existe, se debe crear el par <clave, 1> y agregarlo a la tabla. Se debe
garantizar que sólo haya contención en caso de colisión de hash; es
decir, si dos o más threads intentan incrementar concurrentemente claves
que no colisionan, deben poder hacerlo sin inconvenientes.
*/

void HashMapConcurrente::incrementar(std::string clave) {
    // Completar (Ejercicio 2)
    unsigned int index_tabla = hashIndex(clave);
    // uso los semaforos mutex sobre cada indice de la tabla para no tener que bloquear toda la tabla entera
    // asi otros threads pueden seguir utilizando la tabla
    mutex_tabla[index_tabla].lock(); // wait
    if (tabla[index_tabla] != nullptr) // si existe en la lista del bucket correspondiente (en este caso collisiona hash)
    {
        ListaAtomica<hashMapPair>* lista_actual = tabla[index_tabla];
        
        for (auto it = lista_actual->begin(); it != lista_actual->end(); it++) // busco dentro de la tabla
        {
            if ((*it).first == clave) // la clase string sobrecarga su operador ==
            {
                (*it).second++;
                mutex_tabla[index_tabla].unlock(); // signal --> linea muy importante
                return;
            }
        }
    } else {
        tabla[index_tabla] = new ListaAtomica<hashMapPair>();
    }
    
    tabla[index_tabla]->insertar(hashMapPair(clave, (unsigned int)1));
    mutex_tabla[index_tabla].unlock(); // signal
}
/*
Devuelve todas las claves existentes en la tabla.
Esta operación debe ser no bloqueante y libre de inanición
*/

// con no bloqueante entiendo que no tiene que bloquear toda la tabla
// no tiene inanicion porque permite que en otros indices o buckets de la tabla se puedan ingresar elementos a medida que se recorre 
// y en el indice actual no permitimos que entren elementos, sino que queden en espera.
std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2)
    std::vector<std::string> res;
    for (int i = 0; i < cantLetras; i++)
    {
        mutex_tabla[i].lock();
        ListaAtomica<hashMapPair>* lista_actual = tabla[i];
        for (auto it = lista_actual->begin(); it != lista_actual->end(); it++)
        {
            res.push_back((*it).first);   
        }
        mutex_tabla[i].unlock();
    }
    return res;
}

/*
unsigned int valor(string clave). Devuelve el valor de clave en la tabla,
o 0 si la clave no existe en la tabla. Esta operación debe ser no bloqueante y libre de inanición
*/
// RECORDATORIO: Testear que si eliminan de la tabla, es posible acceder a nullptr
unsigned int HashMapConcurrente::valor(std::string clave) {
    // Completar (Ejercicio 2)
    unsigned int index_tabla = hashIndex(clave);
    mutex_tabla[index_tabla].lock();
    ListaAtomica<hashMapPair>* lista_actual = tabla[index_tabla];
    for (auto it = lista_actual->begin(); it != lista_actual->end(); it++)
    {
        if ((*it).first == clave)
        {
            mutex_tabla[index_tabla].unlock();
            return (*it).second;
        }
    }
    mutex_tabla[index_tabla].unlock();
    return 0;
}

float HashMapConcurrente::promedio() {

    float sum = 0.0;
    unsigned int count = 0;
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (const auto& p : *tabla[index]) {
            sum += p.second;
            count++;
        }
    }
    if (count > 0) {
        return sum / count;
    }
    return 0;        
}



#endif
