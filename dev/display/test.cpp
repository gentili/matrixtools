#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "Screen.h"

int main (int argc, char * argv[])
{
	// Set up signal handling junk
	sigset_t globalsigmask;

	sigfillset(&globalsigmask);
	// Want to ABORT with stack trace on FPE
	sigdelset(&globalsigmask, SIGFPE);
	// Want to ABORT with stack trace on SEGV
	sigdelset(&globalsigmask, SIGSEGV);
	// Want to ABORT with stack trace on BUS
	sigdelset(&globalsigmask, SIGBUS);
	// Want to Abort with stack trace on ILL
	sigdelset(&globalsigmask, SIGILL);

	if (pthread_sigmask(SIG_SETMASK,
				&globalsigmask,
				NULL))
	{
		printf ("ERROR: Could not set global sigmask\n");
		exit(1);
	}
	
	// OK, set up the screen
	Screen scr;

	// Init the screen singleton
	if (!scr.init(10))
	{
		printf ("ERROR: Unable to initialize display terminal!\n");
		exit (1);
	}
	// Start the screen update thread
	scr.startUpdates();

	// Wait for a keystroke
	sleep (10);

	// Shutdown the screen singleton
	scr.shutdown();

	return 0;
}
