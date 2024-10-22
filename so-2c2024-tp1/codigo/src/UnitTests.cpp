#include "lib/littletest.hpp"
#include <vector>

#include "../src/CargarArchivos.hpp"
#include "../src/HashMapConcurrente.hpp"
#include "../src/ListaAtomica.hpp"
#include <thread>
#include <string>

// Tests Ejercicio 1

LT_BEGIN_SUITE(TestsEjercicio1)

ListaAtomica<int> l;

void set_up() {}

void tear_down() {}
LT_END_SUITE(TestsEjercicio1)

LT_BEGIN_TEST(TestsEjercicio1, ListaComienzaVacia)
LT_CHECK_EQ(l.longitud(), 0);
LT_END_TEST(ListaComienzaVacia)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaElemento)
l.insertar(42);
LT_CHECK_EQ(l.longitud(), 1);
LT_END_TEST(InsertarAgregaElemento)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaElementoCorrecto)
l.insertar(42);
LT_CHECK_EQ(l[0], 42);
LT_END_TEST(InsertarAgregaElementoCorrecto)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaEnOrden)
l.insertar(4);
l.insertar(3);
l.insertar(2);
l.insertar(1);
LT_CHECK_EQ(l.longitud(), 4);
LT_CHECK_EQ(l[0], 1);
LT_CHECK_EQ(l[1], 2);
LT_CHECK_EQ(l[2], 3);
LT_CHECK_EQ(l[3], 4);
LT_END_TEST(InsertarAgregaEnOrden)

LT_BEGIN_TEST(TestsEjercicio1, Test_Concurrencia_mismo_bucket)
for (int i = 0; i < 20; i++) // 20 veces para testear concurrencia
{
    ListaAtomica<std::string> ll;
    std::vector<std::thread> threads;
    std::mutex mtx;
    mtx.lock();
    /*insertar en el mismo bucket*/
    threads.emplace_back([&ll]()
                         { ll.insertar("apple"); });
    threads.emplace_back([&ll]()
                         { ll.insertar("aaaaa"); });
    threads.emplace_back([&ll]()
                         { ll.insertar("aaeei"); });
    threads.emplace_back([&ll]()
                         { ll.insertar("aeeei"); });
    threads.emplace_back([&ll]()
                         { ll.insertar("aaooo"); });
    threads.emplace_back([&ll]()
                         { ll.insertar("aaaau"); });
    threads.emplace_back([&ll]()
                         { ll.insertar("aauuu"); });

    for (auto &t : threads)
    {
        t.join();
    }
    mtx.unlock(); // para que no se me adelante el siguiente chequeo

    LT_CHECK_EQ(ll.longitud(), 7);
    /*no es valido pedir orden*/
    /*
    LT_CHECK_EQ(ll[0], "aauuu");
    LT_CHECK_EQ(ll[1], "aaaau");
    LT_CHECK_EQ(ll[2], "aaooo");
    LT_CHECK_EQ(ll[3], "aeeei");
    LT_CHECK_EQ(ll[4], "aaeei");
    LT_CHECK_EQ(ll[5], "aaaaa");
    LT_CHECK_EQ(ll[6], "apple");
    */
    // chequeo que se encuentre
    std::vector<std::string> actual;
    for (auto &val : ll)
    {
        actual.push_back(val);
    }
    LT_CHECK(std::find(actual.begin(), actual.end(), "apple") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "aaaaa") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "aaeei") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "aeeei") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "aaooo") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "aaaau") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "aauuu") != actual.end());
}
LT_END_TEST(Test_Concurrencia_mismo_bucket)

// Tests Ejercicio 2

LT_BEGIN_SUITE(TestsEjercicio2)

HashMapConcurrente hM;

void set_up() {}

void tear_down() {}
LT_END_SUITE(TestsEjercicio2)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoEnHashMapVacio)
LT_CHECK_EQ(hM.valor("Heladera"), 0);
LT_END_TEST(ValorEsCorrectoEnHashMapVacio)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoEnHashMapVacio)
std::vector<std::string> actual = hM.claves();
std::vector<std::string> expected = {};
LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoEnHashMapVacio)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasUnaInsercion)
hM.incrementar("Heladera");
LT_CHECK_EQ(hM.valor("Heladera"), 1);
LT_END_TEST(ValorEsCorrectoTrasUnaInsercion)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasUnaInsercion)
hM.incrementar("Heladera");
std::vector<std::string> actual = hM.claves();
std::vector<std::string> expected = {"Heladera"};
LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoTrasUnaInsercion)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasDosInsercionesMismaPalabra)
hM.incrementar("Heladera");
hM.incrementar("Heladera");
LT_CHECK_EQ(hM.valor("Heladera"), 2);
LT_END_TEST(ValorEsCorrectoTrasDosInsercionesMismaPalabra)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasDosInsercionesMismaPalabra)
hM.incrementar("Heladera");
hM.incrementar("Heladera");
std::vector<std::string> actual = hM.claves();
std::vector<std::string> expected = {"Heladera"};
LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoTrasDosInsercionesMismaPalabra)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasVariasInsercionesMismoBucket)
hM.incrementar("Heladera");
hM.incrementar("Heladera");
hM.incrementar("Lavarropa");
LT_CHECK_EQ(hM.valor("Heladera"), 2);
LT_CHECK_EQ(hM.valor("Lavarropa"), 1);
LT_END_TEST(ValorEsCorrectoTrasVariasInsercionesMismoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasVariasInsercionesMismoBucket)
hM.incrementar("Heladera");
hM.incrementar("Heladera");
hM.incrementar("Lavarropa");
std::vector<std::string> actual = hM.claves();
LT_CHECK_EQ(actual.size(), 2);
LT_CHECK(std::find(actual.begin(), actual.end(), "Heladera") != actual.end());
LT_CHECK(std::find(actual.begin(), actual.end(), "Lavarropa") != actual.end());
LT_END_TEST(ClavesEsCorrectoTrasVariasInsercionesMismoBucket)

LT_BEGIN_TEST(TestsEjercicio2,
              ValorEsCorrectoTrasVariasInsercionesDistintoBucket)
hM.incrementar("Heladera");
hM.incrementar("Microondas");
hM.incrementar("Heladera");
LT_CHECK_EQ(hM.valor("Heladera"), 2);
LT_CHECK_EQ(hM.valor("Microondas"), 1);
LT_END_TEST(ValorEsCorrectoTrasVariasInsercionesDistintoBucket)

LT_BEGIN_TEST(TestsEjercicio2,
              ClavesEsCorrectoTrasVariasInsercionesDistintoBucket)
hM.incrementar("Heladera");
hM.incrementar("Microondas");
hM.incrementar("Heladera");
std::vector<std::string> actual = hM.claves();
LT_CHECK_EQ(actual.size(), 2);
LT_CHECK(std::find(actual.begin(), actual.end(), "Heladera") != actual.end());
LT_CHECK(std::find(actual.begin(), actual.end(), "Microondas") != actual.end());
LT_END_TEST(ClavesEsCorrectoTrasVariasInsercionesDistintoBucket)

LT_BEGIN_TEST(TestsEjercicio2, Incrementar_Promedio_y_claves_concurrente)
std::vector<std::thread> threads;
HashMapConcurrente hashm_c;
std::mutex mtx;

mtx.lock();

threads.emplace_back([&hashm_c]()
                     { hashm_c.incrementar("Heladera"); });
threads.emplace_back([&hashm_c]()
                     { hashm_c.incrementar("Heladera"); });
threads.emplace_back([&hashm_c]()
                     { hashm_c.incrementar("Microondas"); });
threads.emplace_back([&hashm_c]()
                     { hashm_c.incrementar("Microondas"); });
threads.emplace_back([&hashm_c]()
                     { hashm_c.incrementar("Micro"); });
threads.emplace_back([&hashm_c]()
                     { hashm_c.incrementar("Microbio"); });
threads.emplace_back([&hashm_c]()
                     { hashm_c.incrementar("Maximo"); });
threads.emplace_back([&hashm_c]()
                     { hashm_c.incrementar("Micro"); });

for (auto &t : threads)
{
    t.join();
}

mtx.unlock();

LT_CHECK_EQ(hashm_c.valor("Heladera"), 2);
LT_CHECK_EQ(hashm_c.valor("Microondas"), 2);
LT_CHECK_EQ(hashm_c.valor("Micro"), 2);
LT_CHECK_EQ(hashm_c.valor("Microbio"), 1);
LT_CHECK_EQ(hashm_c.valor("Maximo"), 1);
LT_CHECK_EQ(hashm_c.valor("Perro"), 0);
LT_CHECK_EQ(hashm_c.valor("Gato"), 0);

threads.clear(); // Limpiar el vector de hilos

std::vector<std::string> actual = hashm_c.claves();

LT_CHECK(std::find(actual.begin(), actual.end(), "Heladera") != actual.end());
LT_CHECK(std::find(actual.begin(), actual.end(), "Microondas") != actual.end());
LT_CHECK(std::find(actual.begin(), actual.end(), "Micro") != actual.end());
LT_CHECK(std::find(actual.begin(), actual.end(), "Microbio") != actual.end());
LT_CHECK(std::find(actual.begin(), actual.end(), "Maximo") != actual.end());
LT_CHECK(std::find(actual.begin(), actual.end(), "Perro") == actual.end()); // tiene que ser igual a actual.end() porque no lo va a encontrar
LT_CHECK(std::find(actual.begin(), actual.end(), "Gato") == actual.end());  // tiene que ser igual a actual.end() porque no lo va a encontrar

float promedio_c = hashm_c.promedio();
float promedio_multithread_cinco = hashm_c.promedioParalelo(5);
float promedio_multithread_cuatro = hashm_c.promedioParalelo(4);
float promedio_multithread_tres = hashm_c.promedioParalelo(3);
float promedio_multithread_dos = hashm_c.promedioParalelo(2);
float promedio_multithread_uno = hashm_c.promedioParalelo(1);

LT_CHECK_EQ(promedio_c, promedio_multithread_cinco);
LT_CHECK_EQ(promedio_c, promedio_multithread_cuatro);
LT_CHECK_EQ(promedio_c, promedio_multithread_tres);
LT_CHECK_EQ(promedio_c, promedio_multithread_dos);
LT_CHECK_EQ(promedio_c, promedio_multithread_uno);

std::vector<float> promedios(5);

// promedios no editan misma memoria
mtx.lock();
threads.emplace_back([&hashm_c, &promedios]()
                     { promedios[0] = hashm_c.promedioParalelo(1); });
threads.emplace_back([&hashm_c, &promedios]()
                     { promedios[1] = hashm_c.promedioParalelo(2); });
threads.emplace_back([&hashm_c, &promedios]()
                     { promedios[2] = hashm_c.promedioParalelo(3); });
threads.emplace_back([&hashm_c, &promedios]()
                     { promedios[3] = hashm_c.promedioParalelo(4); });
threads.emplace_back([&hashm_c, &promedios]()
                     { promedios[4] = hashm_c.promedioParalelo(5); });

for (auto &t : threads)
{
    t.join();
}
mtx.unlock();

LT_CHECK_EQ(promedio_c, promedios[0]);
LT_CHECK_EQ(promedio_c, promedios[1]);
LT_CHECK_EQ(promedio_c, promedios[2]);
LT_CHECK_EQ(promedio_c, promedios[3]);
LT_CHECK_EQ(promedio_c, promedios[4]);

threads.clear(); // Limpiar el vector de hilos

LT_END_TEST(Incrementar_Promedio_y_claves_concurrente)

// Tests Ejercicio 3

LT_BEGIN_SUITE(TestsEjercicio3)

HashMapConcurrente hM;

void set_up() {}

void tear_down() {}
LT_END_SUITE(TestsEjercicio3)

LT_BEGIN_TEST(TestsEjercicio3, PromedioEsCorrecto)
hM.incrementar("Heladera");
hM.incrementar("Heladera");
hM.incrementar("Heladera");
hM.incrementar("Heladera");
hM.incrementar("Microondas");
hM.incrementar("Microondas");

float actual = hM.promedio();
LT_CHECK_EQ(actual, 3);
LT_END_TEST(PromedioEsCorrecto)

// Completar TEST para caso concurrente
LT_BEGIN_TEST(TestsEjercicio3, PromedioConcurrenteEsCorrecto)
hM.incrementar("Heladera");
hM.incrementar("Heladera");
hM.incrementar("Heladera");
hM.incrementar("Televisor");
hM.incrementar("Microondas");
hM.incrementar("Microondas");

// calculamos el promedio en paralelo
float promedio_concurrente = hM.promedioParalelo(2); // Usamos 4 threads

// verificar que el promedio es el correcto
LT_CHECK_EQ(promedio_concurrente, 2); // (3+2+1)/3 = 2
LT_END_TEST(PromedioConcurrenteEsCorrecto)

// Tests Ejercicio 4

LT_BEGIN_SUITE(TestsEjercicio4)

HashMapConcurrente hM;

void set_up() {}

void tear_down() {}
LT_END_SUITE(TestsEjercicio4)

LT_BEGIN_TEST(TestsEjercicio4, CargarArchivoFunciona)
cargarArchivo(hM, "data/test-1");
LT_CHECK_EQ(hM.valor("Heladera"), 1);
LT_CHECK_EQ(hM.claves().size(), 5);
LT_END_TEST(CargarArchivoFunciona)

LT_BEGIN_TEST(TestsEjercicio4, CargarMultiplesArchivosFuncionaUnThread)
cargarMultiplesArchivos(hM, 1, {"data/test-1", "data/test-2", "data/test-3"});
LT_CHECK_EQ(hM.valor("Heladera"), 2);
LT_CHECK_EQ(hM.valor("NotebookOMEN"), 3);
LT_CHECK_EQ(hM.valor("Microondas"), 4);
LT_CHECK_EQ(hM.claves().size(), 12);
LT_END_TEST(CargarMultiplesArchivosFuncionaUnThread)

LT_BEGIN_TEST(TestsEjercicio4, CargarMultiplesArchivosFuncionaDosThreads)
cargarMultiplesArchivos(hM, 2, {"data/test-1", "data/test-2", "data/test-3"});
LT_CHECK_EQ(hM.valor("Heladera"), 2);
LT_CHECK_EQ(hM.valor("NotebookOMEN"), 3);
LT_CHECK_EQ(hM.valor("Microondas"), 4);
LT_CHECK_EQ(hM.claves().size(), 12);
LT_END_TEST(CargarMultiplesArchivosFuncionaDosThreads)

// Ejecutar tests
LT_BEGIN_AUTO_TEST_ENV()
AUTORUN_TESTS()
LT_END_AUTO_TEST_ENV()
