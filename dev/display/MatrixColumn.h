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
	// Constructor
	MatrixColumnEvent() { return; }
	// Destructor
	virtual ~MatrixColumnEvent() { return; }

	// Render for the given cycle
	// Returns true when this event has more work to do
	// in the next cycle and the event loop should terminate
	virtual bool render(MatrixColumn * mc, Screen * curscr) = 0;

	// Do all 
	virtual void compress(MatrixColumn * mc, Screen * curscr) = 0;
	
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

public:
	//// Inherited interface
	// Constructor
	MatrixColumn(int column);
	// Destructor
	virtual ~MatrixColumn();
	
	// To be called by Screen
	virtual void render(Screen * curscr);

	//// Class specific interface (user calls)
	bool eventspending();
	void add_clear_event(bool override, bool skipable, bool compressable, int duration);

protected:
	//// Internal functions
	// Add a new event to the queue
	void add_event(MatrixColumnEvent * newe);
	// Clear the queue and add this event
	void override_event(MatrixColumnEvent * newe);

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

	friend class MCE_Clear;
};

// This guy can be set both skipable and/or compressable
class MCE_Clear : public MatrixColumnEvent {
public:
	MCE_Clear(int duration) 
		{ _duration = duration; 
		  _lastcycle = -1;
		  return; }

	virtual ~MCE_Clear() { return; }

	virtual bool render(MatrixColumn * mc, Screen * curscr);
	
	virtual void compress(MatrixColumn * mc, Screen * curscr);

protected:
	void doclear(MatrixColumn * mc, Screen * curscr);
	
	// How long the empty column should persist before
	// doing anything else
	int	_duration;
	
	// The number of the last cycle this event was at the top
	// of the queue for; -1 means it's never been rendered
	int	_lastcycle;
};

#endif
