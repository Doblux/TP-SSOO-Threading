#ifndef CHM_CPP
#define CHM_CPP

#include <fstream>
#include <iostream>
#include <thread>

#include "HashMapConcurrente.hpp"

#include <functional> // Para std::bind
#include <mutex>
#include <thread>
#include <vector>

std::mutex mutex_promedio;

HashMapConcurrente::HashMapConcurrente() {
  for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
    tabla[i] = new ListaAtomica<hashMapPair>();
  }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
  return (unsigned int)(tolower(clave[0]) - 'a');
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
  // uso los semaforos mutex sobre cada indice de la tabla para no tener que
  // bloquear toda la tabla entera asi otros threads pueden seguir utilizando la
  // tabla
  mutex_tabla[index_tabla].lock(); // wait
  ListaAtomica<hashMapPair> *lista_actual = tabla[index_tabla];
  for (auto it = lista_actual->begin(); it != lista_actual->end();
      it++) // busco dentro de la tabla
  {
    if ((*it).first == clave) // la clase string sobrecarga su operador ==
    {
      (*it).second++;
      mutex_tabla[index_tabla].unlock(); // signal --> linea muy importante
      return;
    }
  }
  tabla[index_tabla]->insertar(hashMapPair(clave, (unsigned int)1));
  mutex_tabla[index_tabla].unlock(); // signal
}
/*
Devuelve todas las claves existentes en la tabla.
Esta operación debe ser no bloqueante y libre de inanición
*/

// con no bloqueante entiendo que no tiene que bloquear toda la tabla
// no tiene inanicion porque permite que en indices o buckets de la tabla anteriores al actual
// se puedan ingresar elementos a medida que se recorre, y en el indice actual y los siguientes no
// permitimos que entren elementos, sino que queden en espera.
std::vector<std::string> HashMapConcurrente::claves() {
  // Completar (Ejercicio 2)
  for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++){
        mutex_tabla[i].lock();
    }
  std::vector<std::string> res;
  for (int i = 0; i < cantLetras; i++) {
    ListaAtomica<hashMapPair> *lista_actual = tabla[i];
    for (auto it = lista_actual->begin(); it != lista_actual->end(); it++) {
      res.push_back((*it).first);
    }
    mutex_tabla[i].unlock();
  }
  return res;
}

/*
unsigned int valor(string clave). Devuelve el valor de clave en la tabla,
o 0 si la clave no existe en la tabla. Esta operación debe ser no bloqueante y
libre de inanición
*/
// RECORDATORIO: Testear que si eliminan de la tabla, es posible acceder a
// nullptr
unsigned int HashMapConcurrente::valor(std::string clave) {
  // Completar (Ejercicio 2)
  unsigned int index_tabla = hashIndex(clave);
  mutex_tabla[index_tabla].lock();
  ListaAtomica<hashMapPair> *lista_actual = tabla[index_tabla];
  for (auto it = lista_actual->begin(); it != lista_actual->end(); it++) {
    if ((*it).first == clave) {
      mutex_tabla[index_tabla].unlock();
      return (*it).second;
    }
  }
  mutex_tabla[index_tabla].unlock();
  return 0;
}

float HashMapConcurrente::promedio() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++){
        mutex_tabla[i].lock();
    }

    float sum = 0.0;
    unsigned int count = 0;
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (const auto &p : *tabla[index]) {
            sum += p.second;
            count++;
        }
        mutex_tabla[index].unlock();
    }
    if (count > 0) {
        return sum / count;
    }
    return 0;
}

void HashMapConcurrente::promedio_tuneado(unsigned int nro_thread, float &sum,
                                          unsigned int &count,
                                          unsigned int nro_tabla,
                                          unsigned int &cantThreads,
                                          std::vector<int>& clavesAProcesar) {
    while (nro_tabla < cantLetras) {
        if (tabla[nro_tabla]->longitud() == 0) // si es null o si tiene 0 elementos
        // vamos a mirar la siguiente tabla
        {
            mutex_tabla[nro_tabla].unlock();
            nro_tabla++;
        } else {
            unsigned int lista_size = tabla[nro_tabla]->longitud();
            if (nro_thread < lista_size) // si esta en rango hacemos el promedio, sino ajustamos
            // los indices en el siguiente bucket
            {
              mutex_promedio.lock();
              sum += (*tabla[nro_tabla])[nro_thread].second;
              count++;
              clavesAProcesar[nro_tabla]--;
              if (clavesAProcesar[nro_tabla] == 0) {
                mutex_tabla[nro_tabla].unlock();
              }
              mutex_promedio.unlock();
              nro_thread += cantThreads;
            } else { // si me pasé de indice
                nro_thread = nro_thread - lista_size;       // calculo cuanto me pase de largo (el +1 por la longitud
                // (lista_size), cualquier cosa probar sin el +1)
                nro_tabla++; // me cambio de lista o "bucket"
            }
        }
    }
}

float HashMapConcurrente::promedioParalelo(unsigned int cantThreads) {
  for (int i = 0; i < cantLetras; i++) {
    mutex_tabla[i].lock();
  }
  float sum = 0.0;
  unsigned int count = 0;
  unsigned int nro_tabla = 0;
  std::vector<int> clavesAProcesar;
  for (int i = 0; i < cantLetras; i++) {
    clavesAProcesar.push_back(tabla[i]->longitud());
  }
  std::vector<std::thread> threads;
  for (unsigned int i = 0; i < cantThreads; i++) {
    threads.emplace_back(
        /*Uso de std::ref:
        Esto permite que std::thread sepa que estás pasando referencias a las
        variables. Sin std::ref, el hilo intentaría copiar las variables, lo que
        llevaría a errores de tipo porque no coinciden con la firma de
        promedio_tuneado
        */
        std::bind(&HashMapConcurrente::promedio_tuneado, this, i,
                  std::ref(sum), std::ref(count), nro_tabla,
                  std::ref(cantThreads), std::ref(clavesAProcesar)));
  }

  for (auto &t : threads) {
    t.join();
  }

  if (count > 0) { // cuidado : posible condicion de carrera
    return sum / count;
  }

  return 0.0; // Agregar caso para evitar retorno de valor no válido
}

#endif