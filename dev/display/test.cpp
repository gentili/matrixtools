#include <unistd.h>
#include <stdio.h>
#include "Screen.h"

int main (int argc, char * argv[])
{
	Screen scr;
	printf ("Display module test app\n");

	scr.init();
	scr.shutdown();

	return 0;
}
