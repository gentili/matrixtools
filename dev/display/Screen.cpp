// System Includes
#include <math.h>
#include <sys/time.h>
#include <stdio.h>

// Local Includes
#include "Screen.h"

// Local defines

// Static variables

bool Screen::_inited = false;

WINDOW * Screen::_stdscr = NULL;
int Screen::_maxy = -1;
int Screen::_maxx = -1;
bool Screen::_colors = false;
int Screen::_colortbl[8];

float Screen::_updatefreq = 0;
struct timespec Screen::_updateperiod;
void * Screen::_charprocfunc = NULL;

int Screen::_cursattrs = 0;
int Screen::_updatecounter = 0;

vector<Artifact *> Screen::_artifactList;

pthread_mutex_t Screen::_updatelock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Screen::_updatecond = PTHREAD_COND_INITIALIZER;
bool Screen::_workavailable = false;

/////////////////////////////////////
///// Singleton Control Functions
/////////////////////////////////////

bool Screen::init(float updatefreq, void (* charprocfunc) (int))
{
	char	tmpbuf[256];
	
	if (_inited)
		return false;

	// Assign character processing callback
	_charprocfunc = (void *) charprocfunc;

	// Figure out update frequency dependent stuff
	if (updatefreq < 0)
		return false;
	
	_updatefreq = updatefreq;
	_updateperiod.tv_sec = (int) floor(1/updatefreq);
	_updateperiod.tv_nsec = (int) (1000000000.0 *
			(1/updatefreq - floor(1/updatefreq)));
	
	// OK, set up the terminal for operation
	_stdscr = initscr();
	if (!_stdscr)
		return false;
	if (nonl() == ERR) // turn off newline translation
		return false;
	if (cbreak() == ERR) // turn off line buffering erase/kill 
		return false;
	if (noecho() == ERR) // Disable auto echo by input functions
		return false;
	if (leaveok(_stdscr, true) == ERR)  // Leave cursor in place after update
		return false;
	timeout(0); // Because nodelay() doesn't work
	curs_set(0); // Make the cursor invisible

	// Initialize terminal info
	getmaxyx (_stdscr, _maxy, _maxx);

#ifdef DEBUG
	move (0,0);

	sprintf (tmpbuf,"Update Freq: %f\tPeriod: %d sec %d nsec\n",
		_updatefreq, 
		(int) _updateperiod.tv_sec, 
		(int) _updateperiod.tv_nsec);
	addstr (tmpbuf);

	sprintf (tmpbuf,"Maxy: %d\tMaxx: %d\n",_maxy, _maxx);
	addstr (tmpbuf);
#endif
	// Initialize attributes
	_colors = has_colors();
	if (_colors)
	{
		// OK, start the color subsystem
		start_color();
		/*
		 * FIXME: Add some sort of color tuning capability here
		 *
		if (can_change_color())
		{
			short r,g,b;
			addstr ("Can alter colors\n");
			// Pump up the volume on all the colors
			for (short i = 1; i < 8; i++)
			{
				color_content (i,&r,&g,&b);
				//r += 0x101;
				//g += 0x101;
				//b += 0x101;
				init_color(i, r, g, b);
			}
		}
		*/
#ifdef DEBUG
		sprintf (tmpbuf, "Colors: %d Color Pairs: %d\n",
				COLORS, COLOR_PAIRS);
		addstr (tmpbuf);
#endif
		// Define and set color attributes
		for (short i = 1; i < 8; i++)
		{
			// Initialize pairs one through eight to match
			// it's color on a black background
			init_pair(i,i,0);
			
			// Put the new color attribute in the color table
			_colortbl[i] = COLOR_PAIR(i);
			
#ifdef DEBUG
			short fg,bg;
			short r,g,b;
			color_content (i,&r,&g,&b);
			pair_content (i, &fg, &bg);

			attrset(COLOR_PAIR(i));
			sprintf (tmpbuf, "Color %d = (0x%03x:0x%03x:0x%03x), Pair %d = 0x%03x (%d:%d)\n", 
					i,r,g,b,i,COLOR_PAIR(i),fg,bg);
			addstr (tmpbuf);
			attron(A_BOLD);
			sprintf (tmpbuf, "+Bold = (0x%03lx)",COLOR_PAIR(i)|A_BOLD);
			addstr (tmpbuf);
			attroff(A_BOLD);
			attron(A_REVERSE);
			sprintf (tmpbuf, ", +Rvrs = (0x%03lx)",COLOR_PAIR(i)|A_REVERSE);
			addstr (tmpbuf);
			attroff(A_REVERSE);
			attron(A_REVERSE|A_BOLD);
			sprintf (tmpbuf, ", +Rvrs+Bold = (0x%03lx)\n",COLOR_PAIR(i)|A_REVERSE|A_BOLD);
			addstr (tmpbuf);
			attroff(A_REVERSE|A_BOLD);
#endif
		}

	} else
	{
		return false;
	}
	
#ifdef DEBUG
	refresh();
#endif
	_inited = true;
	return true;
}

bool Screen::startUpdates()
{
	if (!_inited || threads() > 0)
		return false;

	createThreads();
	return true;
}

bool Screen::stopUpdates()
{
	if (!_inited || (threads() <= 0))
		return false;

	terminate();
	while (threads() > 0)
	{
		// FIXME: Put a thread safe nanosleep here
		// or something to stop the useless spinning
	}
	return true;
}

	
bool Screen::cleanup()
{
	if (!_inited || (threads() > 0))
		return false;

	endwin();
	_inited = false;
	return true;
}

/////////////////////////////////////
///// Artifact Control Functions
/////////////////////////////////////

bool Screen::addArtifact (Artifact * newart)
{
	if (!_inited || threads() > 0)
		return false;

	// Add the artifact
	_artifactList.push_back(newart);
	
	return true;
}

bool Screen::delArtifact(Artifact * oldart)
{
	if (!_inited || threads() > 0)
		return false;

	// Delete the artifact
	static vector<Artifact *>::iterator aitr = 
		find (_artifactList.begin(),
				_artifactList.end(),
				oldart);

	if (aitr == _artifactList.end())
		return false;

	_artifactList.erase(aitr);
	
	return true;
}

bool Screen::flushArtifacts()
{
	if (!_inited || threads() > 0)
		return false;

	// Clear out the entire artifact list
	_artifactList.clear();
	
	return true;
}


/////////////////////////////////////
///// Main Screen Update Thread
/////////////////////////////////////

void Screen::run()
{
#ifdef DEBUG
	char dbgstr[256];
	FILE * dbglog = fopen ("Screen_run.log", "w");
#endif
	// OK, this routine is a single thread that manages 
	// updating and refreshing the screen.  This should 
	// run a maximum of _updatefreq times a second
	
	_updatecounter = 0;
	struct timeval start;
	struct timeval now;
	struct timespec due;

	gettimeofday(&start,NULL);
	// Immediate update is due
	due.tv_sec = start.tv_sec;
	due.tv_nsec = start.tv_usec * 1000;
	
	while (!shouldterminate())
	{
		// What time is it?
		gettimeofday(&now,NULL);

#ifdef DEBUG
		fprintf(dbglog, "Now %d:%09d Due %d:%09d\n",
				(int) now.tv_sec, (int) now.tv_usec * 1000,
				(int) due.tv_sec, (int) due.tv_nsec);
#endif
		// Are we due for an update?
		if ((now.tv_sec > due.tv_sec) ||
				((now.tv_sec == due.tv_sec) &&
				 (now.tv_usec * 1000 > due.tv_nsec)))
		{
			// Yes, so calc when next update's due
			due.tv_sec = now.tv_sec + _updateperiod.tv_sec;
			due.tv_nsec = now.tv_usec * 1000 + _updateperiod.tv_nsec;
			if (due.tv_nsec >= 1000000000)
			{
				due.tv_nsec -= 1000000000;
				due.tv_sec++;
			}

			// Update the counter
			if (++_updatecounter >= MAX_SCREEN_CYCLE)
				_updatecounter = 0;
#ifdef DEBUG
			move (_maxy-1,0);
			sprintf (dbgstr,"%d",_updatecounter);
			addstr (dbgstr);
#endif 
			// fire off the refresh
			refresh();
		}

		// Go through the artifact list and do any work
		// that's available in this update
		for (vector<Artifact *>::iterator Aitr = _artifactList.begin();
				Aitr != _artifactList.end();
				Aitr++)
		{
			(*Aitr)->render(this);
		}
		

		// Now check to see if there are any characters
		// that need to be sent to the client

		int newchar;
		while ((newchar = wgetch(_stdscr)) != ERR)
		{
			if (_charprocfunc != NULL)
				(* (void (*) (int)) _charprocfunc) (newchar);
		}
		
		// Now run through the artifact list to see if
		// any work has become available

		pthread_mutex_lock (&_updatelock);

		if (!_workavailable)
		{
			// OK, now sleep until the next update
			// or until some work wakes us up
			pthread_cond_timedwait(&_updatecond, &_updatelock, &due);
		}
		pthread_mutex_unlock (&_updatelock);

	}
	threadExit();
}
