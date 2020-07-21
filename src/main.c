#include <logger.h>
#ifdef UNIT_TEST
extern int test_nanofloat(void);
#endif  //  UNIT_TEST

int main(void)
{	
	enable_log();          //  Enable logging via Arm Semihosting. Note: ST Link must be connected or this line will hang.
	debug_println("Starting..."); debug_force_flush();

#ifdef UNIT_TEST
	test_nanofloat();
#endif  //  UNIT_TEST

	debug_println("Done");
	debug_force_flush();   //  Flush the debug buffer before we halt.
	for (;;) {}            //  Loop forever.
}

#ifdef UNIT_TEST
#include "../lib/nano-float/test/test.c"
#endif  //  UNIT_TEST
