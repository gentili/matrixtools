#ifndef FULLCOLUMN_H
#define FULLCOLUMN_H

// System Includes
#include <deque>

// Local Includes
#include "Screen.h"

class MatrixColumnEvent {
public:
	bool _skipable;
	bool _compressable;
protected:

};

class MatrixColumn : public Artifact {
public:
	// Inherited interface
	MatrixColumn(int column);
	virtual ~MatrixColumn();
	
	// Class specific interface
	virtual void render(Screen * curscr);

protected:

	//// Config variables
	// Column this artifact occupies
	int	_column;
	// Current String
	char	_curstr[1024];

	// Integer part describes which character
	// of _curstr was last displayed
	float   _curchar;

	// Current position of cursor in column
	int	_curpos;
	
	// Event Queue
	deque<MatrixColumnEvent> _eventqueue;
};

#endif
