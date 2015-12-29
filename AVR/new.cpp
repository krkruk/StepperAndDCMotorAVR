#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "new.hpp"


// XXX: Implementacja obsługi wyjątków? Teraz chyba nie jest potrzebna :)

// new i new[]
void * operator new(size_t size) {
    return malloc(size);
}

void * operator new[](size_t size) {
    return malloc(size);
}

// Naiwna implementacja new w miejscu
void * operator new(size_t, void * ptr) {
	return ptr;
}

void * operator new[](size_t, void * ptr) {
    return ptr;
}

// delete i delete[]
void operator delete(void* ptr) {
    free(ptr);
}

void operator delete[](void* ptr) {
    free(ptr);
}

// Naiwna implementacja delete w miejscu.

void operator delete(void *, void * ptr) {
	free(ptr);
}

void operator delete[](void *, void *ptr) {
	free(ptr);
}

namespace {

inline char& flag_part(__guard *g) {
	return *(reinterpret_cast<char*>(g));
}

inline uint8_t& sreg_part(__guard *g) {
	return *(reinterpret_cast<uint8_t*>(g) + sizeof(char));
}

} // anonymous namespace

int __cxa_guard_acquire(__guard *g) {
    uint8_t oldSREG = SREG;
    cli();
    // Inicjalizacja zmiennej statycznej powinna być bezpieczna wielowątkowo.
    // Jeśli chcesz zrezygnować z tego mechanizmu, kompiluj z:
    // -fno-threadsafe-statics
    if (flag_part(g)) {
        SREG = oldSREG;
        return false;
    } else {
        sreg_part(g) = oldSREG;
        return true;
    }
//	return !*(char *)(g);
}
void __cxa_guard_release(__guard *g) {
	flag_part(g) = 1;
	SREG = sreg_part(g);
	//	*(char *)g = 1;
}
void __cxa_guard_abort (__guard *g) {
	SREG = sreg_part(g);
}

void __cxa_pure_virtual(void) {
	// Zgodnie ze standardem C++ powinno być std::terminate()
	abort();
}
void __cxa_deleted_virtual(void) {
	// Zgodnie ze standardem C++ powinno być std::terminate()
	abort();
}