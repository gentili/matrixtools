#include <unistd.h>
#include <stdio.h>
#include "Screen.h"

int main (int argc, char * argv[])
{
	Screen scr;
	printf ("Display module test app\n");

	// Init the screen singleton
	if (!scr.init(10))
	{
		printf ("Unable to initialize display terminal!\n");
		exit (1);
	}
	// Start the screen update thread
	scr.startUpdates();

	// Wait for a keystroke

	// Shutdown the screen singleton
	sleep (5);
	scr.shutdown();

	return 0;
}
