#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "Screen.h"

bool exitnow = false;

void processchar (int c)
{
	if (c == 'x')
		exitnow = true;
}

int main (int argc, char * argv[])
{
	// Set up signal handling junk
	sigset_t globalsigmask;

	sigemptyset(&globalsigmask);
	sigaddset(&globalsigmask, SIGWINCH);
	// Want to ABORT on FPE
	// sigdelset(&globalsigmask, SIGFPE);
	// Want to ABORT on SEGV
	// sigdelset(&globalsigmask, SIGSEGV);
	// Want to ABORT on BUS
	// sigdelset(&globalsigmask, SIGBUS);
	// Want to Abort on ILL
	// sigdelset(&globalsigmask, SIGILL);
	// Want to ABORT on TERM
	// sigdelset(&globalsigmask, SIGTERM);

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
	if (!scr.init(10, &processchar))
	{
		printf ("ERROR: Unable to initialize display terminal!\n");
		exit (1);
	}
	// Start the screen update thread
	scr.startUpdates();

	while (!exitnow)
	{
		struct timespec ts;
		ts.tv_sec = 1;
		ts.tv_nsec = 0;
		nanosleep (&ts, NULL);
		
	}
	// Shutdown the screen singleton
	scr.shutdown();

	return 0;
}
