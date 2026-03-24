#ifdef USE_MODULES
import iron.core;
#include "iron/macros.h"
#else 
#include "iron/macros.h"
#include "iron/unsafe/asserts.h"
#endif 

int main() {
	ASSERT_EQ(1, 1);
	ASSERT_NE(1, 2);
	PANIC_IF(1 == 2, "error");
	PANIC_UNLESS(1 == 1, "error");

}
