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

	pthread_mutex_lock(&_equeue_lock);
	// Is there anything in the queue?
	if (!_equeue.empty())
	{
		// Get to the next unskipable uncompressable or last event
		while (((_equeue.front()->_skipable) ||
					(_equeue.front()->_compressable)) &&
				(_equeue.front() != _equeue.back()))
		{
			if (_equeue.front()->_compressable)
				_equeue.front()->compress(this);
			delete (_equeue.front());
			_equeue.pop_front();
		}
	
		// OK, now let the event do it's thing and
		// throw it away if it's done
		if (!_equeue.front()->render(this, 0))
		{
			delete(_equeue.front());
			_equeue.pop_front();
		}
	}
	pthread_mutex_unlock(&_equeue_lock);
	return;
}
