#ifndef SCREEN_H
#define SCREEN_H

// System includes
#include <ncurses.h>

// Foreign includes
#include "MTObject.h"

// Local includes

/* This class defines a screen in which lines may be put.  It is a 
 singleton (we don't have multiple screens) and grabs the default tty
 if any. */

class Screen : public MTObject
{
public:
	// Init grabs control of the tty, reads in all the specs,
	// and generally sets everything up to start updating the display
	bool init(float updatefreq);

	void startUpdates() 	{ createThreads(); }

	bool shutdown();

	void attr_set_red() 	{ _cursattrs = _colortbl[1]; }
	void attr_set_green() 	{ _cursattrs = _colortbl[2]; }
	void attr_set_yellow()	{ _cursattrs = _colortbl[3]; }
	void attr_set_blue()	{ _cursattrs = _colortbl[4]; }
	void attr_set_magenta()	{ _cursattrs = _colortbl[5]; }
	void attr_set_cyan()	{ _cursattrs = _colortbl[6]; }
	void attr_set_white()	{ _cursattrs = _colortbl[7]; }

	void attr_on_bold()	{ _cursattrs |= A_BOLD; }

	void attr_register()	{ attrset( _cursattrs ); }
	
protected:
	// This is the main update thread
	virtual void run();

	// This describes if the screen has been inited yet
	static bool _inited;

	// Initialization state variables (describe system
	// params set at init)
	static WINDOW * _stdscr;
	static int _maxy;
	static int _maxx;
	static bool _colors;
	static int _colortbl[8];

	// Config variables
	static float _updatefreq;
	static struct timespec _updateperiod;

	// Current Screen state
	static int _cursattrs;

	// Multithreading jabber
	static pthread_mutex_t _updatelock;
	static pthread_cond_t _updatecond;
	static bool _workavailable;

private:
};

#endif
