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
	XYSwapScreen scr;

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

	// TEST - MCE_RepScript
	std::vector<MatrixColumn *>::iterator MCitr = MClist.begin();
	float speed = 0;
	int newattr = scr.curs_attr_green();
	i = 0;
	std::vector<int> counts;
	std::vector<int> headattrs;
	std::vector<int> colattrs;
	counts.push_back(6);
	headattrs.push_back(scr.curs_attr_green() | scr.curs_attr_bold());
	colattrs.push_back(scr.curs_attr_green());
	counts.push_back(6);
	headattrs.push_back(scr.curs_attr_white() | scr.curs_attr_bold());
	colattrs.push_back(scr.curs_attr_white());
	counts.push_back(4);
	headattrs.push_back(scr.curs_attr_red() | scr.curs_attr_bold());
	colattrs.push_back(scr.curs_attr_red());
	counts.push_back(7);
	headattrs.push_back(scr.curs_attr_yellow() | scr.curs_attr_bold());
	colattrs.push_back(scr.curs_attr_yellow());
	counts.push_back(5);
	headattrs.push_back(scr.curs_attr_blue() | scr.curs_attr_bold());
	colattrs.push_back(scr.curs_attr_blue());
	counts.push_back(8);
	headattrs.push_back(scr.curs_attr_magenta() | scr.curs_attr_bold());
	colattrs.push_back(scr.curs_attr_magenta());
	counts.push_back(5);
	headattrs.push_back(scr.curs_attr_cyan() | scr.curs_attr_bold());
	colattrs.push_back(scr.curs_attr_cyan());
	while (!exitnow && MCitr != MClist.end())
	{
		std::vector<float> speeds;
		speeds.push_back(speed);
		speeds.push_back(speed+0.1);
		speeds.push_back(speed+0.2);
		speeds.push_back(speed+0.05);
		speeds.push_back(speed+0.5);
		speeds.push_back(speed+0.3);
		speeds.push_back(speed+0.4);
		char tmpstr[1024];
		sprintf (tmpstr, "Green*White*Red*Yellow*Blue*Magenta*Cyan*");
		(*MCitr)->add_setstring_event (false, false, false, tmpstr);
		(*MCitr)->add_clear_event (false, false, false);
		(*MCitr)->add_delay_event (false, false, false, 1);

		if (i % 2)
			(*MCitr)->add_multitone_stringdrop_script (speeds,counts,headattrs,colattrs);
		
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
