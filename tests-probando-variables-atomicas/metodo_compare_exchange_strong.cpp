#include <atomic>
#include <iostream>

std::atomic<int> a(10);
int expected = 10;
int desired = 20;

// El método compare_exchange_strong se utiliza para realizar una comparación y un intercambio. 
// Compara el valor actual de la variable atómica con un valor esperado. 
// Si son iguales, establece la variable atómica a un nuevo valor y devuelve true.
// Si no son iguales, actualiza el valor esperado con el valor actual y devuelve false. 
// Esta operación es "fuerte" porque es más propensa a realizar el intercambio en un solo intento, 
// lo que puede ser útil en situaciones donde se necesita una garantía de que la operación se realiza en un solo paso.

int main(){
	if (a.compare_exchange_strong(expected, desired)) {
		std::cout << "Intercambio exitoso, nuevo valor: " << a.load() << std::endl;
	} else {
		std::cout << "Intercambio fallido, valor actual: " << a.load() << ", valor esperado: " << expected << std::endl;
	}
	return 0;
}
