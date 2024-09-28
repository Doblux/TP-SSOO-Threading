#include <atomic>
#include <iostream>

std::atomic<int> a(10);

int main(){
	int old_value = a.exchange(20); // Cambia el valor de 'a' a 20 y guarda el antiguo (10)
	std::cout << "Valor anterior: " << old_value << ", Nuevo valor: " << a.load() << std::endl; // Imprime 10 y 20
	return 0;
}
