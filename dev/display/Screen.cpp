// System Includes

// Local Includes
#include "Screen.h"

bool Screen::_inited = false;
WINDOW * Screen::_stdscr = NULL;
int Screen::_maxy = -1;
int Screen::_maxx = -1;

bool Screen::init()
{
	char	tmpbuf[256];
	
	if (_inited)
		return false;

	// OK, do all that funky grabbing the screen stuff
	_stdscr = initscr();
	if (!_stdscr)
		return false;
	nonl();
	cbreak();
	noecho();

	leaveok(_stdscr, true);

	// Initialize terminal info
	getmaxyx (_stdscr, _maxy, _maxx);

	// Output the info
	move (0,0);
	sprintf (tmpbuf,"Maxy: %d\nMaxx: %d",_maxy, _maxx);
	addstr (tmpbuf);
	refresh();
	_inited = true;
	return true;
}

bool Screen::shutdown()
{
	if (!_inited)
		return false;

	endwin();
	_inited = false;
	return true;
}

void Screen::run()
{
}
