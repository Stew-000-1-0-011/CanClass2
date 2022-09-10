#include <stdint.h>
#include <stddef.h>
//#include <stm32f103xb.h>
//#include <core_cm3.h>

int f()
{
	_Static_assert(sizeof(size_t) == 4);
	return 0;
}
