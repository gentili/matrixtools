// System Includes

// Local Includes
#include "Screen.h"

// Local defines
#define DEBUG	1

// Static variables

bool Screen::_inited = false;
WINDOW * Screen::_stdscr = NULL;
int Screen::_maxy = -1;
int Screen::_maxx = -1;
bool Screen::_colors = false;

// Member functions of the Screen singleton

bool Screen::init()
{
	char	tmpbuf[256];
	
	if (_inited)
		return false;

	// OK, set up the terminal for operation
	_stdscr = initscr();
	if (!_stdscr)
		return false;
	if (nonl() == ERR) // turn off newline translation
		return false;
	if (cbreak() == ERR) // turn off line buffering erase/kill 
		return false;
	if (noecho() == ERR)
		return false;
	if (leaveok(_stdscr, true) == ERR)
		return false;
	
	// Initialize terminal info
	getmaxyx (_stdscr, _maxy, _maxx);

#ifdef DEBUG
	move (0,0);
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
