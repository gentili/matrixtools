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

	//////////////////////////////
	// Set up matrix columns
	//////////////////////////////
	
	// Set up a screen full of Matrix Columns
	std::vector<MatrixColumn *> MClist;
	int i;
	for (i = 0; i < scr.maxx(); i++)
	{
		MatrixColumn * newmcol = new MatrixColumn(i);
		scr.addArtifact(newmcol);
		MClist.push_back(newmcol);
	}
	
	// Start the screen update thread
	scr.startUpdates();

	// TEST - MCE_StringDrop
	std::vector<MatrixColumn *>::iterator MCitr = MClist.begin();
	float speed = 0;
	int newattr = scr.curs_attr_green();
	i = 0;
	while (!exitnow && MCitr != MClist.end())
	{
		char tmpstr[1024];
		sprintf (tmpstr, "Set String %02d then Fill                                ",i);
		(*MCitr)->add_setstring_event (false, false, false, tmpstr);
		(*MCitr)->add_setattr_event (false, false, false, newattr);
		(*MCitr)->add_clear_event (false, false, false);

		if (i % 2)
			(*MCitr)->add_stringdrop_event (false, false, false, speed, -1, false, scr.curs_attr_bold() | scr.curs_attr_white());
		
		MCitr++;
		speed += 0.13;
		if (speed > 1)
			speed -= floor(speed);
		i++;
	}

	// Wait for user input before returning
	while (!exitnow)
	{
		TEST_NSLEEP(0, 10000000);
	}

	// Stop the screen update thread
	scr.stopUpdates();

	// cleanup the screen
	scr.cleanup();

	return 0;
}
