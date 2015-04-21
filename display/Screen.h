#pragma once

// System includes
#include <string.h>
#include <ncurses.h>
#include <algorithm>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>

// Local includes

// Defines
#define MAX_SCREEN_CYCLE	10000

#define DIFF_CYCLE(start, end) ( \
	((end - start + MAX_SCREEN_CYCLE) % MAX_SCREEN_CYCLE) \
)

class Screen;

/* This class is an abstract base class that represents some artifact
to be rendered by the Screen class */

class Artifact {
public:
	// Destructor
	virtual ~Artifact() { return; }

	// Rendering function
	virtual void render(Screen * curscr) = 0;
};

/* This class defines a screen in which lines may be put.  It is a 
 singleton (we don't have multiple screens) and grabs the default tty
 if any. */

class Screen
{
public:
	//// Singleton Control Functions
	
	// Init grabs control of the tty, reads in all the specs,
	// and generally sets everything up to start updating the display
	bool init(float updatefreq, std::function<void(int)>&&);

	// This starts the screen update processing thread
	bool startUpdates();

	// This stops the screen update processing thread
	bool stopUpdates();

	// This releases the tty
	bool cleanup();

	//// Artifact Control Functions
	
	bool addArtifact(Artifact * newart);

	bool delArtifact(Artifact * oldart);

	bool flushArtifacts();

	//// Attribute control functions
	int curs_attr_red() 	{ return _colortbl[1]; }
	int curs_attr_green() 	{ return _colortbl[2]; }
	int curs_attr_yellow()	{ return _colortbl[3]; }
	int curs_attr_blue()	{ return _colortbl[4]; }
	int curs_attr_magenta()	{ return _colortbl[5]; }
	int curs_attr_cyan()	{ return _colortbl[6]; }
	int curs_attr_white()	{ return _colortbl[7]; }

	int curs_attr_bold()	{ return A_BOLD; }
	int curs_attr_reverse()	{ return A_REVERSE; }

	void curs_attr_set(int newattr)
				{ wattrset( _stdscr, newattr ); }

	//// Cursor control functions
	void curs_move(int y, int x)	{ wmove(_stdscr, y, x); }
	void curs_addch(char ch)	{ waddch(_stdscr, ch); }
	void curs_mvaddch(int y, int x, char ch)
					{ mvwaddch(_stdscr, y, x, ch); }
	void curs_mvaddstr(int y, int x, char * str)
					{ mvwaddstr(_stdscr, y, x, str); }
	void curs_mvchgat(int y, int x, int n, int newattr)
					{ mvwchgat(_stdscr, y, x, n, static_cast<attr_t>(newattr), 0, NULL); }

	//// Member accessors
	int maxy() 		{ return _maxy; }
	int maxx() 		{ return _maxx; }
	int updatecounter()	{ return _updatecounter; }
	
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
	static std::function<void(int)> _charprocfunc;

	// Current Screen state
	static int _updatecounter;

	// Artifact variables
	static std::vector<Artifact *> _artifactList;

	// Multithreading jabber
	static pthread_mutex_t _updatelock;
	static pthread_cond_t _updatecond;

        static std::thread _thread;
        static std::atomic_bool _exit;

private:
};

class XYSwapScreen : public Screen {
public:
	void curs_move(int x, int y)	{ wmove(_stdscr, y, x); }
	void curs_addch(char ch)	{ waddch(_stdscr, ch); }
	void curs_mvaddch(int x, int y, char ch)
					{ mvwaddch(_stdscr, y, x, ch); }
	void curs_mvaddstr(int x, int y, char * str)
					{ mvwaddstr(_stdscr, y, x, str); }
	void curs_mvchgat(int x, int y, int n, int newattr)
					{ mvwchgat(_stdscr, y, x, n, static_cast<attr_t> (newattr), 0, NULL); }
//	int maxy()		{ return _maxx; }
//	int maxx()		{ return _maxy; }

};
