// System Includes
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>

// Local Includes
#include "Screen.h"
#include "MatrixColumn.h"
#include "ClockModule.h"

// AbstractModule Interface specification
void ClockModule::processchar (int c)
{
}

ResType * execute(Screen & scr, std::vector<MatrixColumn *> & MClist)
{
	return NULL;
}

// Private members
/*
int setupdigit(Screen & scr, int startcol, int digit, int horizseg)
{
	int i = startcol;
	sprintf (tmpstr, "Digit %d start blank ", digit);
	MClist[i]->add_setstring_event (false, false, false, tmpstr);
	MClist[i]->add_stringdrop_event (false, false, false, 1, -1, false, scr.curs_attr_bold() | scr.curs_attr_white());
	for (int j = 0; j < horizseg - 2; j++)
	{
		i += 2;
		sprintf (tmpstr, "Digit %d col %d ",digit,j);
		MClist[i]->add_setstring_event (false, false, false, tmpstr);
		MClist[i]->add_stringdrop_event (false, false, false, 1, -1, false, scr.curs_attr_bold() | scr.curs_attr_white());
	}
	i += 2;
	sprintf (tmpstr, "Digit %d end blank ",digit);
	MClist[i]->add_setstring_event (false, false, false, tmpstr);
	MClist[i]->add_stringdrop_event (false, false, false, 1, -1, false, scr.curs_attr_bold() | scr.curs_attr_white());

	return 0;
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

	// Check that screen params are doable
	if ((scr.maxx() < 64) || (scr.maxy() < 15))
	{
		scr.cleanup();
		printf ("ERROR: Screen must be at least 64x15\n"); 
		exit (1);
	}
		
	// Choose segment sizes
	int vertseg = (scr.maxy() - 3) / 4;
	int horizseg = (scr.maxx() / 7);
		
	// Build segments
	int onseg = scr.curs_attr_green() | scr.curs_attr_bold();
	int offseg = scr.curs_attr_green();

	std::vector<int> empty(scr.maxy(), offseg);
	
	std::vector<int> vert_topbot;
	vert_topbot.insert(vert_topbot.end(),vertseg+1,offseg);
	vert_topbot.insert(vert_topbot.end(),vertseg,onseg);
	vert_topbot.insert(vert_topbot.end(),1,offseg);
	vert_topbot.insert(vert_topbot.end(),vertseg,onseg);
	vert_topbot.insert(vert_topbot.end(),vertseg+1,offseg);

	std::vector<int> vert_top;
	vert_top.insert(vert_top.end(),vertseg+1,offseg);
	vert_top.insert(vert_top.end(),vertseg,onseg);
	vert_top.insert(vert_top.end(),2*vertseg+2,offseg);
	
	std::vector<int> vert_bot;
	vert_bot.insert(vert_bot.end(),2*vertseg+2,offseg);
	vert_bot.insert(vert_bot.end(),vertseg,onseg);
	vert_bot.insert(vert_bot.end(),vertseg+1,offseg);
	
	std::vector<int> horiz_top;
	horiz_top.insert(horiz_top.end(),vertseg,offseg);
	horiz_top.insert(horiz_top.end(),1,onseg);
	horiz_top.insert(horiz_top.end(),3*vertseg+2,offseg);
	
	std::vector<int> horiz_mid;
	horiz_mid.insert(horiz_mid.end(),2*vertseg+1,offseg);
	horiz_mid.insert(horiz_mid.end(),1,onseg);
	horiz_mid.insert(horiz_mid.end(),2*vertseg+1,offseg);
	
	std::vector<int> horiz_bot;
	horiz_bot.insert(horiz_bot.end(),3*vertseg+2,offseg);
	horiz_bot.insert(horiz_bot.end(),1,onseg);
	horiz_bot.insert(horiz_bot.end(),vertseg,offseg);

	std::vector<int> horiz_topmid;
	horiz_topmid.insert(horiz_topmid.end(),vertseg,offseg);
	horiz_topmid.insert(horiz_topmid.end(),1,onseg);
	horiz_topmid.insert(horiz_topmid.end(),vertseg,offseg);
	horiz_topmid.insert(horiz_topmid.end(),1,onseg);
	horiz_topmid.insert(horiz_topmid.end(),2*vertseg+1,offseg);

	std::vector<int> horiz_midbot;
	horiz_midbot.insert(horiz_midbot.end(),2*vertseg+1,offseg);
	horiz_midbot.insert(horiz_midbot.end(),1,onseg);
	horiz_midbot.insert(horiz_midbot.end(),vertseg,offseg);
	horiz_midbot.insert(horiz_midbot.end(),1,onseg);
	horiz_midbot.insert(horiz_midbot.end(),vertseg,offseg);
	
	std::vector<int> horiz_topbot;
	horiz_topbot.insert(horiz_topbot.end(),vertseg,offseg);
	horiz_topbot.insert(horiz_topbot.end(),1,onseg);
	horiz_topbot.insert(horiz_topbot.end(),2*vertseg+1,offseg);
	horiz_topbot.insert(horiz_topbot.end(),1,onseg);
	horiz_topbot.insert(horiz_topbot.end(),vertseg,offseg);

	std::vector<int> horiz_topmidbot;
	horiz_topmidbot.insert(horiz_topmidbot.end(),vertseg,offseg);
	horiz_topmidbot.insert(horiz_topmidbot.end(),1,onseg);
	horiz_topmidbot.insert(horiz_topmidbot.end(),vertseg,offseg);
	horiz_topmidbot.insert(horiz_topmidbot.end(),1,onseg);
	horiz_topmidbot.insert(horiz_topmidbot.end(),vertseg,offseg);
	horiz_topmidbot.insert(horiz_topmidbot.end(),1,onseg);
	horiz_topmidbot.insert(horiz_topmidbot.end(),vertseg,offseg);

	std::vector<int> colon;
	colon.insert(colon.end(),(int) (vertseg*1.5),offseg);
	colon.insert(colon.end(),2,onseg);
	colon.insert(colon.end(),vertseg,offseg);
	colon.insert(colon.end(),2,onseg);
	colon.insert(colon.end(),(int) (vertseg*1.5),offseg);

	// Add off attrs until the column is completely specified
	while ((int) vert_topbot.size() < scr.maxy())
	{
		vert_topbot.insert(vert_topbot.begin(),1,offseg);
		vert_topbot.insert(vert_topbot.end(),1,offseg);
		vert_top.insert(vert_top.begin(),1,offseg);
		vert_top.insert(vert_top.end(),1,offseg);
		vert_bot.insert(vert_bot.begin(),1,offseg);
		vert_bot.insert(vert_bot.end(),1,offseg);
		horiz_top.insert(horiz_top.begin(),1,offseg);
		horiz_top.insert(horiz_top.end(),1,offseg);
		horiz_mid.insert(horiz_mid.begin(),1,offseg);
		horiz_mid.insert(horiz_mid.end(),1,offseg);
		horiz_bot.insert(horiz_bot.begin(),1,offseg);
		horiz_bot.insert(horiz_bot.end(),1,offseg);
		horiz_topmid.insert(horiz_topmid.begin(),1,offseg);
		horiz_topmid.insert(horiz_topmid.end(),1,offseg);
		horiz_midbot.insert(horiz_midbot.begin(),1,offseg);
		horiz_midbot.insert(horiz_midbot.end(),1,offseg);
		horiz_topbot.insert(horiz_topbot.begin(),1,offseg);
		horiz_topbot.insert(horiz_topbot.end(),1,offseg);
		horiz_topmidbot.insert(horiz_topmidbot.begin(),1,offseg);
		horiz_topmidbot.insert(horiz_topmidbot.end(),1,offseg);
	}
	while ((int) colon.size() < scr.maxy())
	{
		colon.insert(colon.begin(),1, offseg);
		colon.insert(colon.end(),1, offseg);
	}
	
	// Build digits
	
	
	scr.cleanup();
	printf ("Screen: %dx%d\n",scr.maxx(), scr.maxy());
	printf ("Vert seg size: %d\n", vertseg);
	printf ("Horiz seg size: %d\n", horizseg);
	printf ("vert_topbot: %d\n", vert_topbot.size());
	printf ("vert_top: %d\n", vert_top.size());
	printf ("vert_bot: %d\n", vert_bot.size());
	printf ("horiz_top: %d\n", horiz_top.size());
	printf ("horiz_mid: %d\n", horiz_mid.size());
	printf ("horiz_bot: %d\n", horiz_bot.size());
	printf ("horiz_topmid: %d\n", horiz_topmid.size());
	printf ("horiz_midbot: %d\n", horiz_midbot.size());
	printf ("horiz_topbot: %d\n", horiz_topbot.size());
	printf ("colon: %d\n", colon.size());
	exit (0);

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

	// Set up the screen
	std::vector<MatrixColumn *>::iterator MCitr = MClist.begin();
	std::vector<int> newattr(empty);

	while (!exitnow && MCitr != MClist.end())
	{
		char tmpstr[1024];
		(*MCitr)->add_setattr_event (false, false, false, newattr);
		(*MCitr)->add_clear_event (false, false, false);

		MCitr++;
	}

	// Set up digits 
	char tmpstr[1024];
	i = 0;
	horizseg = horizseg / 2;

	setupdigit(0, 1, horizseg);
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
*/
