// System includes
#include <curses.h>

// Local includes
#include "MatrixColumn.h"

// Default constructor

MatrixColumn::MatrixColumn(int column)
{
	_column = column;
	memset (_curstr, 0, 1024);
	_curchar = 0;
	_curpos = 0;
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

void MatrixColumn::add_clear_event(bool override, bool skipable, bool compressable, int duration)
{
	MCE_Clear * newe = new MCE_Clear(duration);
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

bool MCE_Clear::render(MatrixColumn * mc, Screen * curscr)
{
#ifdef DEBUG
	curscr->curs_mvaddch(0,mc->_column, 'C');
	char ch = '0';
	ch += (_duration % 10);
	curscr->curs_mvaddch(curscr->maxy() - 4, mc->_column, ch);
#endif
	int cycle = curscr->updatecounter();
	if (_lastcycle == -1)
	{
		doclear(mc, curscr);
		_lastcycle = cycle;
		_duration--;
	} else
	{
		_duration -= DIFF_CYCLE(_lastcycle, cycle);
		_lastcycle = cycle;
	}
#ifdef DEBUG
	if (!(_duration >= 0))
	{
		doclear(mc, curscr);
	}
#endif
	return (_duration >= 0);
}

void MCE_Clear::compress(MatrixColumn * mc, Screen * curscr)
{
	doclear(mc, curscr);
}

void MCE_Clear::doclear(MatrixColumn * mc, Screen * curscr)
{
	for (int i = 0; i < curscr->maxy(); i++)
	{
		curscr->curs_mvaddch(i, mc->_column, ' ');
	}
}
