// System Includes
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>

// Local Includes
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
	std::vector<MatrixColumn *> MClist;
	for (int i = 0; i < scr.maxx(); i++)
	{
		MatrixColumn * newmcol = new MatrixColumn(i);
		scr.addArtifact(newmcol);
		MClist.push_back(newmcol);
	}
	
	for (int i = 0; i < scr.maxy(); i++)
		scr.curs_mvaddstr(i, 20, "TEST - Staggered MCE_Delay and MCE_Clear");
	
	// Start the screen update thread
	time_t startime = time(NULL);
	scr.startUpdates();

	// TEST - MCE_Delay and MCE_Clear events
	std::vector<MatrixColumn *>::iterator MCitr = MClist.begin();
	int i = 0;
	while (!exitnow && MCitr != MClist.end())
	{
		(*MCitr)->add_delay_event (false, false, false, i);
		(*MCitr)->add_clear_event (false, false, false);
		MCitr++;
		i++;
	}
	// Give everything time to complete
	TEST_NSLEEP (3,0);

	// Check that all the event queues are empty
	MCitr = MClist.begin();
	while (MCitr != MClist.end())
	{
		if ((*MCitr)->eventspending())
			TEST_ERR_HALT("ERROR: TEST - MCE_Delay and MCE_Clear - Events still pending\n");
		MCitr++;
	}

	// TEST - MCE_SetString, MCE_SetAttr and MCE_StringFill
	MCitr = MClist.begin();
	i = 0;
	while (!exitnow && MCitr != MClist.end())
	{
		char tmpstr[1024];
		sprintf (tmpstr, "Set String %02d then Fill                                ",i);
		(*MCitr)->add_setstring_event (false, false, false, tmpstr);
		int newattr = scr.curs_attr_red();
		if (i % 2)
			newattr |= scr.curs_attr_bold();
		if (i % 3)
			newattr |= scr.curs_attr_reverse();
		(*MCitr)->add_setattr_event (false, false, false, newattr);
		(*MCitr)->add_stringfill_event (false, false, false);
		MCitr++;
		i++;
	}

	// Give everything time to complete
	TEST_NSLEEP (2,0);

	// Check that all the event queues are empty
	MCitr = MClist.begin();
	while (MCitr != MClist.end())
	{
		if ((*MCitr)->eventspending())
			TEST_ERR_HALT("ERROR: TEST - MCE_SetString and MCE_StringFill - Events still pending\n");
		MCitr++;
	}
	
	// Wait for user input before returning
	while (!exitnow)
	{
		TEST_NSLEEP(0, 10000000);
	}

	// Stop the screen update thread
	scr.stopUpdates();
	time_t endtime = time(NULL);

	// cleanup the screen
	scr.cleanup();

	// Now output the number of the last cycle
	
	printf ("Total Cycles: %d\n", scr.updatecounter());
	printf ("Avg Cycles/s: %f\n", (float) scr.updatecounter() / 
			(float) (endtime - startime));
	printf ("All tests PASSED\n");
	return 0;
}
