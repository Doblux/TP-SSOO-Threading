#include <atomic>
#include <iostream>

std::atomic<int> a(10);
int expected = 10;
int desired = 20;

int main(){

	while (!a.compare_exchange_weak(expected, desired)) {
		std::cout << "Intercambio fallido, valor actual: " << a.load() << ", valor esperado: " << expected << std::endl;
		// Aquí podrías modificar `expected` si es necesario para nuevos intentos
	}
	std::cout << "Intercambio exitoso, nuevo valor: " << a.load() << std::endl;
	return 0;
}
