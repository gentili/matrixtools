#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "Screen.h"
#include "MatrixColumn.h"

// Local Defines
#define TEST_CYCLEFREQ	30

#define TEST_ERR_HALT(errstr) do { \
	scr.stopUpdates(); \
	scr.cleanup(); \
	printf("%s",errstr); \
	exit (1); \
} while (0);

#define TEST_NSLEEP(sec, nsec) do { \
	struct timespec ts; \
	ts.tv_sec = sec; \
	ts.tv_nsec = nsec; \
	nanosleep (&ts, NULL); \
} while (0);

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
	if (!scr.init(TEST_CYCLEFREQ, &processchar))
	{
		printf ("ERROR: Unable to initialize display terminal\n");
		exit (1);
	}
	// Check that term meets minimum reqs for testing
	if ((scr.maxx() < 80) || (scr.maxy() < 25))
	{
		TEST_ERR_HALT ("ERROR: terminal must be at least 80 columns by 25 rows\n");
	}

	//////////////////////////////
	// Start Matrix Column Testing
	//////////////////////////////
	
	// Set up a screen full of Matrix Columns
	vector<MatrixColumn *> MClist;
	for (int i = 0; i < scr.maxx(); i++)
	{
		MatrixColumn * newmcol = new MatrixColumn(i);
		scr.addArtifact(newmcol);
		MClist.push_back(newmcol);
	}
	
	for (int i = 0; i < scr.maxy(); i++)
		scr.curs_mvaddstr(i, 20, "TEST - Instantanious MCE_Clear");
	
	// Start the screen update thread
	time_t startime = time(NULL);
	scr.startUpdates();

	TEST_NSLEEP(1,0);
	// TEST - MCE_Clear events
	vector<MatrixColumn *>::iterator MCitr = MClist.begin();
	while (!exitnow && MCitr != MClist.end())
	{
		(*MCitr)->add_clear_event (false, false, false, 0);
		MCitr++;
		TEST_NSLEEP (0,10000000);
	}
	// Give everything time to complete
	TEST_NSLEEP (1,0);

	// Check that all the event queues are empty
	MCitr = MClist.begin();
	while (MCitr != MClist.end())
	{
		if ((*MCitr)->eventspending())
			TEST_ERR_HALT("ERROR: TEST - Instantanious MCE_Clear - Events still pending\n");
		MCitr++;
	}

	// TEST - MCE_StringFill
	/*
	while (!exitnow)
	{
		struct timespec ts;
		ts.tv_sec = 0;
		ts.tv_nsec = 100000000;
		nanosleep (&ts, NULL);
		
	}
	*/

	// Stop the screen update thread
	time_t endtime = time(NULL);
	scr.stopUpdates();

	// cleanup the screen
	scr.cleanup();

	// Now output the number of the last cycle
	
	printf ("Total Cycles: %d\n", scr.updatecounter());
	printf ("Avg Cycles/s: %f\n", (float) scr.updatecounter() / 
			(float) (endtime - startime));
	printf ("All tests PASSED\n");
	return 0;
}
