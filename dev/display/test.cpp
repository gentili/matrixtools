#include <unistd.h>
#include <stdio.h>
#include "Screen.h"

int main (int argc, char * argv[])
{
	Screen scr;
	printf ("Display module test app\n");

	if (!scr.init())
	{
		printf ("Unable to initialize display terminal!\n");
		exit (1);
	}
	sleep (5);
	scr.shutdown();

	return 0;
}
