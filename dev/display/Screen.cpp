// System Includes
#include "ncurses.h"

// Local Includes
#include "Screen.h"

bool Screen::_inited = false;

bool Screen::init()
{
	if (_inited)
		return false;

	// OK, do all that funkey grabbing the screen stuff
	

	_inited = true;
	return true;
}

bool Screen::shutdown()
{
	if (!_inited)
		return false;

	_inited = false;
	return true;
}

void Screen::run()
{
}
