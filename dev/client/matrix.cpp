// System Includes
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include "sysinfo.h"

// Local Includes
#include "Screen.h"
#include "MatrixColumn.h"
#include "AbstractModule.h"
#include "PSModule.h"

// Local Defines
#define CYCLEFREQ	30

#define ERR_HALT(errstr) do { \
	scr.stopUpdates(); \
	scr.cleanup(); \
	printf("%s",errstr); \
	exit (1); \
} while (0);

#define NSLEEP(sec, nsec) do { \
	struct timespec ts; \
	ts.tv_sec = sec; \
	ts.tv_nsec = nsec; \
	nanosleep (&ts, NULL); \
} while (0);

AbstractModule * curmod = NULL;

void processchar (int c)
{
	if (curmod != NULL)
		curmod->processchar(c);
}

int main (int argc, char * argv[])
{
	// Set up signal handling junk
	sigset_t globalsigmask;

	sigemptyset(&globalsigmask);
	sigaddset(&globalsigmask, SIGWINCH);

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
	if (!scr.init(CYCLEFREQ, &processchar))
	{
		printf ("ERROR: Unable to initialize display terminal\n");
		exit (1);
	}

	// Check that screen params are doable
	if ((scr.maxx() < 64) || (scr.maxy() < 15))
	{
		scr.cleanup();
		printf ("ERROR: Screen must be at least 64x15\n"); 
		exit (1);
	}
		
	// Set up a screen full of Matrix Columns
	std::vector<MatrixColumn *> MClist;
	for (int i = 0; i < scr.maxx(); i++)
	{
		MatrixColumn * newmcol = new MatrixColumn(i);
		scr.addArtifact(newmcol);
		MClist.push_back(newmcol);
	}
	
	// Schedule an initial clear sweep
	std::vector<MatrixColumn *>::iterator MCitr = MClist.begin();

	while (MCitr != MClist.end())
	{
		(*MCitr)->add_clear_event (false, false, false);
		MCitr++;
	}

	// Start with a process module
	curmod = new PSModule();
	
	////////////////////////
	// MAIN LOOP
	////////////////////////
	// Cross State variables
	while (curmod != NULL)
	{
		AbstractModule * nextmod = NULL;
		scr.startUpdates();
		nextmod = curmod->execute(scr, MClist);
		scr.stopUpdates();
		delete (curmod);
		curmod = nextmod;
	}

	// cleanup the screen
	scr.cleanup();

	return 0;
}
