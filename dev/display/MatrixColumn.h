#ifndef MATRIXCOLUMN_H
#define MATRIXCOLUMN_H

// System Includes
#include <deque>
#include <pthread.h>

// Local Includes
#include "Screen.h"

class MatrixColumn;

class MatrixColumnEvent {
public:
	// Render for the given cycle
	virtual bool render(MatrixColumn * mc, int cycle) = 0;

	// Do all 
	virtual void compress(MatrixColumn * mc) = 0;
	
	// If an event is skipable, then we simply 
	// discard it when another event shows up in
	// the queue.
	bool _skipable;

	// If an event is compressable, then we get
	// it to do all its work instantly when another
	// event shows up and move on to the new event
	// in the same cycle.
	bool _compressable;

	// NOTE: Skipable overrrides compressable.
protected:

};

class MatrixColumn : public Artifact {
	
	friend class MatrixColumnEvent;

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
	
	// Event Queue Stuff
	pthread_mutex_t	 _equeue_lock;	// Event queue access lock
	deque<MatrixColumnEvent *> _equeue;	// Event queue

};

#endif
