// System Includes
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>

// Local Includes
#include "Screen.h"
#include "MatrixColumn.h"
#include "AbstractModule.h"
#include "ClockModule.h"

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

typedef enum MatrixStateEnum {
	MS_init = 0,
	MS_runmod,
	MS_exit
} MatrixState_e;

MatrixState_e mstate = MS_init;

AbstractModule * curmod = NULL;

void processchar (int c)
{
	// Deal with global control characters
	if (c == 'x')
		mstate = MS_exit;
	
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
	
	// Start the screen update thread
	scr.startUpdates();

	// Do an initial clear sweep
	std::vector<MatrixColumn *>::iterator MCitr = MClist.begin();

	while (MCitr != MClist.end())
	{
		(*MCitr)->add_clear_event (false, false, false);
		MCitr++;
	}

	////////////////////////
	// MAIN STATE MACHINE
	////////////////////////
	// Cross State variables
	ResType * rt = NULL;
	while (mstate != MS_exit)
	{
		switch (mstate)
		{
		case MS_init:
			// Do any setup 
			mstate = MS_runmod;
			break;
		case MS_runmod:
			rt = curmod->execute(scr, MClist);
			break;
		default:
			break;
		}
		// Do something here to determine
		// what the next state should be based on
		// something 
		NSLEEP(0, 10000000);
	}

	// Stop the screen update thread
	scr.stopUpdates();

	// cleanup the screen
	scr.cleanup();

	return 0;
}
