// System Includes

// Local Includes
#include "Screen.h"

bool Screen::_inited = false;
WINDOW * Screen::_stdscr = NULL;
int Screen::_maxy = -1;
int Screen::_maxx = -1;
bool Screen::_colors = false;

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

	move (0,0);
	// Initialize terminal info
	getmaxyx (_stdscr, _maxy, _maxx);
	sprintf (tmpbuf,"Maxy: %d\tMaxx: %d\n",_maxy, _maxx);
	addstr (tmpbuf);

	_colors = has_colors();
	// Initialize colors
	if (_colors)
	{
		// OK, start the color subsystem
		start_color();
		addstr ("Color support enabled\n");
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
		sprintf (tmpbuf, "Colors: %d Color Pairs: %d\n",
				COLORS, COLOR_PAIRS);
		addstr (tmpbuf);
		for (short i = 0; i < 8; i++)
		{
			short fg,bg;
			short r,g,b;
			init_pair(i,7-i,0);
			
			color_content (7-i,&r,&g,&b);
			pair_content (i, &fg, &bg);
			attrset(COLOR_PAIR(i)|A_BOLD);
			sprintf (tmpbuf, "(0x%03x:0x%03x:0x%03x) Color %d = %d:%d\t", 
					r,g,b,i,fg,bg);
			addstr (tmpbuf);
			attron(A_BOLD);
			sprintf (tmpbuf, "B|Color %d = %d:%d\n", 
					i,fg,bg);
			addstr (tmpbuf);
		}

	} else
	{
		addstr ("Color support disabled\n");
		addstr ("NORMAL\n");
		attrset (A_STANDOUT);
		addstr ("STANDOUT\n");
		attrset (A_UNDERLINE);
		addstr ("UNDERLINE\n");
		attrset (A_BOLD);
		addstr ("BOLD\n");
		attrset (A_BLINK);
		addstr ("BLINK\n");
		attrset (A_DIM);
		addstr ("DIM\n");
	}
	

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
