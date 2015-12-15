// System Includes
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>

// Local Includes
#include "display/Screen.h"
#include "MatrixTop.h"

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

MatrixTop * mtop = NULL;

void processchar (int c)
{
	if (mtop != NULL)
		mtop->processchar(c);
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
	if ((scr.maxx() < 20) || (scr.maxy() < 15))
	{
		scr.cleanup();
		printf ("ERROR: Screen must be at least 10x15\n"); 
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
		(*MCitr)->add_setattr_event (false, false, false, scr.curs_attr_green());
		(*MCitr)->add_clear_event (false, false, false);
		MCitr++;
	}

	// Start things up
	
	scr.startUpdates();
	mtop = new MatrixTop();
	mtop->execute(scr, MClist);  // App entry point
	delete (mtop);
	scr.stopUpdates();

	// cleanup the screen
	scr.cleanup();

	return 0;
}
