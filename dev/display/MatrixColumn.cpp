// System includes
#include <curses.h>

// Local includes
#include "MatrixColumn.h"

// Default constructor

MatrixColumn::MatrixColumn(int column)
{
	_column = column;

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
	return;
}
