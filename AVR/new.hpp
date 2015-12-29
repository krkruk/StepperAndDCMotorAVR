#ifndef NEW_HPP_
#define NEW_HPP_

//
// avr-libc nie dostarcza operatorów new i delete.
// Ma także problemy z definicją obsługi dziedziczenia i dziedziczenia
// wirtualnego. Plik poprawia te problemy.
// Źródło:
//  http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=59453
//

#include <stdlib.h>

#ifdef __cplusplus

// Operatory new, new[]
extern void * operator new(size_t size);
extern void * operator new[](size_t size);

// Operatory new i new[] alokujące w miejscu
extern void * operator new(size_t size, void *ptr);
extern void * operator new[](size_t size, void *ptr);

// Operatory delete i delete[]
extern void operator delete(void *ptr);
extern void operator delete[](void *ptr);

// Operatory delete i delete[] alokujące w miejscu
extern void operator delete(void *srcPtr, void *dstPtr);
extern void operator delete[](void *srcPtr, void *dstPtr);

// Obsługa dziedziczeń i dziedziczeń wirtualnych...
__extension__ typedef int __guard __attribute__((mode (__DI__)));

extern "C" {
	int __cxa_guard_acquire(__guard *);
	void __cxa_guard_release (__guard *);
	void __cxa_guard_abort (__guard *);

	void __cxa_pure_virtual(void) __attribute__ ((__noreturn__));
	void __cxa_deleted_virtual(void) __attribute__ ((__noreturn__));
}

#endif // __cplusplus

#endif /* NEW_HPP_ */
