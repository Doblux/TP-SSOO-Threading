#ifndef LISTA_ATOMICA_HPP
#define LISTA_ATOMICA_HPP

#include <atomic>
#include <cstddef>

template<typename T>
class ListaAtomica {
 private:
    struct Nodo {
        Nodo(const T &val) : _valor(val), _siguiente(nullptr) {}

        T _valor;
        Nodo *_siguiente;
    };

    std::atomic<Nodo *> _cabeza;

 public:
    ListaAtomica() : _cabeza(nullptr) {}

    ~ListaAtomica() {
        Nodo *n, *t;
        n = _cabeza.load();
        while (n) {
            t = n;
            n = n->_siguiente;
            delete t;
        }
    }
    // Investigando los métodos me encontre todo esto
    // Nodo* antiguo = _cabeza.exchange(val);
    // metodo exchange: Cambia el valor de la variable atomica '_cabeza' a val y guarda el antiguo valor en antiguo

    // a.compare_exchange_strong(b, c) compara a con b
    // si a == b (comparacion)  --> a = c (asignacion) y devuelve true
    // si a != b (comparacion)  --> actualiza a con el valor actual de a y devuelve false

    // a.compare_exchange_weak(b, c) compara a con b
    // si a == b (comparacion)  -->  a = c (asignacion) y devuelve true
    // si a != b (comparacion)  -->  actualiza a con el valor actual de a, y devuelve false

    // Weak : Puede fallar en situaciones normales, Ideal para bucles (puede reintentar), Optimizado para rendimiento en alta contención
    // Strong: No debería fallar en condiciones normales; Menos frecuente en bucles de reintentos;  Más estricto, puede ser menos eficiente si se espera contención

    // Contención Baja: Situaciones donde hay poco acceso simultáneo a los recursos. 
    // Esto permite que los hilos trabajen de manera más independiente, mejorando el rendimiento general.
    // Contención Alta: Situaciones donde muchos hilos intentan acceder al mismo recurso al mismo tiempo.
    // Esto puede llevar a esperas prolongadas y un uso ineficiente de los recursos.
    
    
    void insertar(const T &valor) {
        // Completar (Ejercicio 1)
        Nodo* nuevo = new Nodo(valor);
        Nodo* cabeza_actual = _cabeza.load();
        nuevo->_siguiente = _cabeza; // para asegurar que no sea nullptr cuando el while de false

        while (!_cabeza.compare_exchange_weak(cabeza_actual, nuevo)) // "mientras todos los threads no tengan bien actualizada la _cabeza"
        {
            nuevo->_siguiente = cabeza_actual; // asegura apuntar a la cabeza mas reciente
        } // si _cabeza != cabeza_actual, actualiza la cabeza a su valor real, devuelve false y negado da true
    }

    T& operator[](size_t i) const {
        Nodo *n = _cabeza.load();
        for (size_t j = 0; j < i; j++) {
            n = n->_siguiente;
        }
        return n->_valor;
    }

    unsigned int longitud() const {
        Nodo *n = _cabeza.load();
        unsigned int cant = 0;
        while (n != nullptr) {
            cant++;
            n = n->_siguiente;
        }
        return cant;
    }

    struct iterator {
    private:
        ListaAtomica *_lista;

        typename ListaAtomica::Nodo *_nodo_sig;

        iterator(ListaAtomica<T> *lista, typename ListaAtomica<T>::Nodo *sig)
            : _lista(lista), _nodo_sig(sig) {}

    public:
        iterator &operator=(const typename ListaAtomica::iterator &otro) {
            _lista = otro._lista;
            _nodo_sig = otro._nodo_sig;
            return *this;
        }

        T& operator*() {
            return _nodo_sig->_valor;
        }

        iterator& operator++() { 
            _nodo_sig = _nodo_sig->_siguiente;
            return *this;
        }

        iterator operator++(int) { 
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const typename ListaAtomica::iterator &otro) const {
            return _lista->_cabeza.load() == otro._lista->_cabeza.load()
                && _nodo_sig == otro._nodo_sig;
        }

        bool operator!=(const typename ListaAtomica::iterator &otro) const {
            return !(*this == otro);
        }

        friend iterator ListaAtomica<T>::begin();
        friend iterator ListaAtomica<T>::end();
    };

    iterator begin() { 
        return iterator(this, _cabeza);
    }

    iterator end() { 
        return iterator(this, nullptr);
    }
};

#endif /* LISTA_ATOMICA_HPP */
