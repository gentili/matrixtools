// System includes
#include <curses.h>
#include <math.h>

// Local includes
#include "MatrixColumn.h"

// Default constructor

MatrixColumn::MatrixColumn(int column)
{
	_column = column;
	memset (_curstr, 0, MC_MAX_STR);
	_curstrlen = 0;
	_curattrs = 0;
	_curchar = -1;
	_curfrac = 0;
	_curpos = -1;
	pthread_mutex_init(&_equeue_lock, NULL);

	return;
}

MatrixColumn::~MatrixColumn()
{
	// FIXME: Need to clear out any lingering events 
	// on the queue
	return;
}

void MatrixColumn::render(Screen * curscr)
{
#ifdef DEBUG
	// Tens digit
	curscr->curs_move(curscr->maxy()-3,_column);
	char ch = '0';
	ch += (int) (_column / 10);
	curscr->curs_addch(ch);
	// Ones digit
	curscr->curs_move(curscr->maxy()-2,_column);
	ch = '0';
	ch += _column % 10;
	curscr->curs_addch(ch);
#endif

	//// Main event loop

	bool terminate = false;
	pthread_mutex_lock(&_equeue_lock);
	// Is there anything in the queue?
	while (!terminate && !_equeue.empty())
	{
		// Get to the next unskipable uncompressable or last event
		while (((_equeue.front()->_skipable) ||
					(_equeue.front()->_compressable)) &&
				(_equeue.front() != _equeue.back()))
		{
			if (_equeue.front()->_compressable)
				_equeue.front()->compress(this, curscr);
			delete (_equeue.front());
			_equeue.pop_front();
		}
	
		// OK, now let the event do it's thing
		terminate = _equeue.front()->render(this, curscr);
		// If the event has not signalled to terminate the 
		// event loop, then it's done all it needs to and
		// we should pop it off the queue.
		if (!terminate)
		{
			delete(_equeue.front());
			_equeue.pop_front();
		}
	}
	pthread_mutex_unlock(&_equeue_lock);
	return;
}

bool MatrixColumn::eventspending()
{
	pthread_mutex_lock(&_equeue_lock);
	bool retval = !_equeue.empty();
	pthread_mutex_unlock(&_equeue_lock);

	return retval;
}

//// Functions to add repeating event scripts to the matrix column event queue

// This produces a repeating script
void MatrixColumn::add_multitone_stringdrop_script(std::vector<float> & speeds, 
		std::vector<int> & counts, 
		std::vector<int> & headattrs,
		std::vector<int> & colattrs)
{
	if ((headattrs.size() != colattrs.size()) ||
			(headattrs.size() != counts.size()) ||
			(headattrs.size() != speeds.size()))
		throw;

	std::vector<MatrixColumnEvent *> eventlist;
	for (int i = 0; i < (int) counts.size(); i++)
	{
		// Put in SA event
		MCE_SetAttr * newsa = new MCE_SetAttr(colattrs[i]);
		newsa->_skipable = false;
		newsa->_compressable = false;
		eventlist.push_back(newsa);
		
		// Put in SD event
		MCE_StringDrop * newsd = new MCE_StringDrop
			(speeds[i], 
			 counts[i], 
			 true,
			 headattrs[i]);
		newsd->_skipable = false;
		newsd->_compressable = false;
		eventlist.push_back(newsd);
	}
	MCE_RepScript * newe = new MCE_RepScript(eventlist);
	newe->_skipable = true;
	newe->_compressable = false;

	add_event(newe);
}

//// Functions to add new events to the matrixcolumn event queue

void MatrixColumn::add_delay_event(bool override, bool skipable, bool compressable, int duration)
{
	MCE_Delay * newe = new MCE_Delay(duration);
	newe->_skipable = skipable;
	newe->_compressable = compressable; // Meaningless for this event

	if (override)
	{
		override_event(newe);
	} else {
		add_event(newe);
	}
}

void MatrixColumn::add_clear_event(bool override, bool skipable, bool compressable)
{
	MCE_Clear * newe = new MCE_Clear();
	newe->_skipable = skipable;
	newe->_compressable = compressable; // Meaningless for this event

	if (override)
	{
		override_event(newe);
	} else {
		add_event(newe);
	}
}

void MatrixColumn::add_setattr_event(bool override, bool skipable, bool compressable, int newattrs)
{
	MCE_SetAttr * newe = new MCE_SetAttr(newattrs);
	newe->_skipable = skipable;
	newe->_compressable = compressable; // Meaningless for this event

	if (override)
	{
		override_event(newe);
	} else {
		add_event(newe);
	}
}

void MatrixColumn::add_setattr_event(bool override, bool skipable, bool compressable, std::vector<int> & newattrvec)
{
	MCE_SetAttr * newe = new MCE_SetAttr(newattrvec);
	newe->_skipable = skipable;
	newe->_compressable = compressable; // Meaningless for this event

	if (override)
	{
		override_event(newe);
	} else {
		add_event(newe);
	}
}

void MatrixColumn::add_setstring_event(bool override, bool skipable, bool compressable, char * newstr)
{
	MCE_SetString * newe = new MCE_SetString(newstr);
	newe->_skipable = skipable;
	newe->_compressable = compressable; // Meaningless for this event

	if (override)
	{
		override_event(newe);
	} else {
		add_event(newe);
	}
}

void MatrixColumn::add_stringfill_event(bool override, bool skipable, bool compressable)
{
	MCE_StringFill * newe = new MCE_StringFill();
	newe->_skipable = skipable;
	newe->_compressable = compressable; // Meaningless for this event

	if (override)
	{
		override_event(newe);
	} else {
		add_event(newe);
	}
}

void MatrixColumn::add_stringdrop_event(bool override, bool skipable, bool compressable, float speed, int charcount, bool cont, int headcharattr)
{
	MCE_StringDrop * newe = new MCE_StringDrop(speed, charcount, cont, headcharattr);
	newe->_skipable = skipable;
	newe->_compressable = compressable;

	if (override)
	{
		override_event(newe);
	} else {
		add_event(newe);
	}
}

// Private MatrixColumn functions

void MatrixColumn::add_event(MatrixColumnEvent * newe)
{
	pthread_mutex_lock(&_equeue_lock);
		_equeue.push_back(newe);
	pthread_mutex_unlock(&_equeue_lock);
}

void MatrixColumn::override_event(MatrixColumnEvent * newe)
{
	pthread_mutex_lock(&_equeue_lock);
		while (!_equeue.empty())
		{
			delete(_equeue.front());
			_equeue.pop_front();
		}
		_equeue.push_back(newe);
	pthread_mutex_unlock(&_equeue_lock);
}

///////////////////////////
// Matrix Column Events
///////////////////////////

// The delay event

MCE_Delay::MCE_Delay(int duration)
{
	_duration = duration;
	_lastcycle = -1;
	return;
}

bool MCE_Delay::render(MatrixColumn * mc, Screen * curscr)
{
#ifdef DEBUG
	curscr->curs_mvaddch(0,mc->_column, 'D');
	char ch = '0';
	ch += (_duration % 10);
	curscr->curs_mvaddch(1, mc->_column, ch);
#endif
	int cycle = curscr->updatecounter();
	if (_lastcycle == -1)
	{
		_duration--;
	} else
	{
		_duration -= DIFF_CYCLE(_lastcycle, cycle);
	}
	_lastcycle = cycle;
#ifdef DEBUG
	if (!(_duration >= 0))
	{
		curscr->curs_mvaddch(0, mc->_column, ' ');
		curscr->curs_mvaddch(1, mc->_column, ' ');
	}
#endif
	return (_duration >= 0);
}

//// MCE_Clear

bool MCE_Clear::render(MatrixColumn * mc, Screen * curscr)
{
	doclear(mc, curscr);
	return false;
}

void MCE_Clear::compress(MatrixColumn * mc, Screen * curscr)
{
	doclear(mc, curscr);
}

void MCE_Clear::doclear(MatrixColumn * mc, Screen * curscr)
{
	curscr->curs_attr_set(mc->_curattrs);
	for (int i = 0; i < curscr->maxy(); i++)
	{
		curscr->curs_mvaddch(i, mc->_column, ' ');
	}
}

//// MCE_SetAttr

MCE_SetAttr::MCE_SetAttr(int newattr)
{
	_newattr = newattr;
}

MCE_SetAttr::MCE_SetAttr(std::vector<int> & newattrvec)
{
	_newattrvec = newattrvec;
}

bool MCE_SetAttr::render(MatrixColumn * mc, Screen * curscr)
{
	mc->_curattrs = _newattr;
	mc->_curattrvec = _newattrvec;
	return false;
}

void MCE_SetAttr::compress(MatrixColumn * mc, Screen * curscr)
{
	mc->_curattrs = _newattr;
	mc->_curattrvec = _newattrvec;
}

//// MCE_SetString

MCE_SetString::MCE_SetString(char * newstr)
{
	// Make a copy of the new string
	memset (_newstr, 0, MC_MAX_STR);
	strncpy (_newstr, newstr, MC_MAX_STR - 1);
}

bool MCE_SetString::render(MatrixColumn * mc, Screen * curscr)
{
	strncpy (mc->_curstr, _newstr, MC_MAX_STR);
	mc->_curstrlen = strlen(mc->_curstr);
	return false;
}

void MCE_SetString::compress(MatrixColumn * mc, Screen * curscr)
{
	strncpy (mc->_curstr, _newstr, MC_MAX_STR);
	return;
}

//// MCE_StringFill

bool MCE_StringFill::render(MatrixColumn * mc, Screen * curscr)
{
	dofill(mc, curscr);
	return false;
}

void MCE_StringFill::compress(MatrixColumn * mc, Screen * curscr)
{
	dofill(mc, curscr);
}

void MCE_StringFill::dofill(MatrixColumn * mc, Screen * curscr)
{
	// Set the current attribute
	curscr->curs_attr_set(mc->_curattrs);
	
	int mystrlen = strlen (mc->_curstr);
	// Now cycle through each row and output the char
	for (int i = 0; i < curscr->maxy(); i++)
	{
		if (i < mystrlen)
		{
			curscr->curs_mvaddch(i, mc->_column, mc->_curstr[i]);
		} else
		{
			curscr->curs_mvaddch(i, mc->_column, ' ');
		}
	}
}

//// MCE_StringDrop

MCE_StringDrop::MCE_StringDrop(float speed, int charcount, bool cont, int headcharattr)
{
	_speed = speed;
	_orig_charcount = _charcount = charcount;
	_orig_cont = _cont = cont;
	_headcharattr = headcharattr;
}

bool MCE_StringDrop::render(MatrixColumn * mc, Screen * curscr)
{
	// First, if not cont, then we need to reset everything
	if (!_cont)
	{
		// Indicates that we're at the top of the 
		// column and no characters have been output yet
		mc->_curpos = -1;
		mc->_curchar = -1;
		mc->_curfrac = 0;
		// Now we make cont == true so that the next
		// round will continue where this one left off
		_cont = true;
	}
	// OK, now we find out how many characters to output this round
	float outtot = _speed + mc->_curfrac;
	int outint = (int) floor(outtot);
	mc->_curfrac = outtot - floor(outtot);
	
	// Now, if there are any characters to output, do it
	for (;outint > 0;outint--)
	{
		// OK, first take the current character
		// and switch it to normal colour
		if (mc->_curpos >= 0)
		{
			// If the attribute vector is filled in
			// then we need to use the appropriate
			// attr for this row, otherwise just use
			// the global attribute
			if (mc->_curattrvec.empty())
			{
				curscr->curs_attr_set(mc->_curattrs);
			} else
			{
				assert ((int) mc->_curattrvec.size() > mc->_curpos);
				curscr->curs_attr_set(mc->_curattrvec[mc->_curpos]);
			}
			curscr->curs_mvaddch(mc->_curpos,
					mc->_column,
					mc->_curstr[mc->_curchar]);
		}
		
		// Now inc the curpos and check for wrapping
		if (++mc->_curpos >= curscr->maxy())
			mc->_curpos = 0;

		// Now inc the curchar and check for wrapping
		if (++mc->_curchar >= mc->_curstrlen)
			mc->_curchar = 0;

		// Now output the bold white character
		curscr->curs_attr_set(_headcharattr);
		curscr->curs_mvaddch(mc->_curpos, mc->_column,
				mc->_curstr[mc->_curchar]);

		// Now dec the charcount and exit
		// if we're done
		if (_charcount > 0)
			if (--_charcount == 0)
				return false;
	}

	// Still more chars to output so true
	return true;
}

void MCE_StringDrop::compress(MatrixColumn * mc, Screen * curscr)
{
	// FIXME: Need to fill this in!
}

void MCE_StringDrop::reset()
{
	// Need to reset all vars that can change
	// during operation
	
	_cont = _orig_cont;
	_charcount = _orig_charcount;
}

//// MCE_RepScript

MCE_RepScript::MCE_RepScript (std::vector<MatrixColumnEvent *> & eventlist)
{
	_eventlist = eventlist;
	_curevent = _eventlist.begin();
}

MCE_RepScript::~MCE_RepScript()
{
	// Must cycle through the event list and
	// blow away all the events
	
	for (_curevent = _eventlist.begin();
			_curevent != _eventlist.end();
			_curevent++)
	{
		delete (*_curevent);
	}
}

bool MCE_RepScript::render(MatrixColumn * mc, Screen * curscr)
{
#ifdef DEBUG
	// This is a repscript
	curscr->curs_mvaddch(curscr->maxy()-5,mc->_column,'R');
	char ch = '0';
	ch += (_eventlist.size() % 10);
	curscr->curs_mvaddch(curscr->maxy()-4, mc->_column, ch);
#endif
	bool terminate = false;
	while (!terminate)
	{
		terminate = (*_curevent)->render(mc, curscr);
		if (!terminate)
		{
			_curevent++;
			if (_curevent == _eventlist.end())
			{
				_curevent = _eventlist.begin();
			}
			(*_curevent)->reset();
		}
	}
	return true;
}

void MCE_RepScript::reset()
{
	// FIXME: Fill this in!
}
