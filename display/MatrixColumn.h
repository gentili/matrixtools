#ifndef MATRIXCOLUMN_H
#define MATRIXCOLUMN_H

// System Includes
#include <deque>
#include <pthread.h>
#include <atomic>

// Local Includes
#include "Screen.h"

// Defines

#define MC_MAX_STR	1024

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

	// Reset the event to its original state
	// (as if it had never been run)
	virtual void reset() = 0;
	
	// If an event is skipable, then we simply 
	// discard it when another event shows up in
	// the queue.
	bool _skipable;

	// If an event is compressable, then we get
	// it to do all its work instantly when another
	// event shows up and move on to the new event
	// in the same cycle.
	bool _compressable;

	// NOTE: Skipable overrides compressable.
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
	long getLRU() { 
		return _lru.load();
	}

	void resetLRU() {
		_lru.store(0);
	}
	
	// Script manipulators
	
	void add_multitone_stringdrop_script(std::vector<float> & speeds,
			std::vector<int> & counts,
			std::vector<int> & headattrs,
			std::vector<int> & colattrs);

	// Event manipulators
	bool eventspending();
	void add_delay_event(bool override, bool skipable, bool compressable, int duration);
	void add_clear_event(bool override, bool skipable, bool compressable);
	void add_setattr_event(bool override, bool skipable, bool compressable, int newattrs);
	void add_setattr_event(bool override, bool skipable, bool compressable, std::vector<int> & newattrvec);
	void add_setstring_event(bool override, bool skipable, bool compressable, const char * newstr);
	void add_stringfill_event(bool override, bool skipable, bool compressable);
	void add_stringdrop_event(bool override, bool skipable, bool compressable, float speed, int charcount, bool cont, int headcharattr);

protected:
	//// Internal functions
	// Add a new event to the queue
	void add_event(MatrixColumnEvent * newe);
	// Clear the queue and add this event
	void override_event(MatrixColumnEvent * newe);

	//// Config variables
	// Column this artifact occupies
	int	_column;
	
	// Current String Stuff
	char	_curstr[MC_MAX_STR];
	int	_curstrlen;

	// Current attributes for writing characters
	int	_curattrs;
	std::vector<int> _curattrvec;

	// Integer part describes which character
	// of _curstr was last displayed
	int	_curchar;
	float	_curfrac;

	// Current position of cursor in column
	int	_curpos;
	
	// Event Queue Stuff
	pthread_mutex_t	 _equeue_lock;	// Event queue access lock
	std::deque<MatrixColumnEvent *> _equeue;	// Event queue
        std::atomic_long _lru;

	friend class MCE_Delay;
	friend class MCE_Clear;
	friend class MCE_SetAttr;
	friend class MCE_SetString;
	friend class MCE_StringFill;
	friend class MCE_StringDrop;
	friend class MCE_RepScript;
};

class MCE_Delay : public MatrixColumnEvent {
public:
	MCE_Delay(int duration);

	virtual ~MCE_Delay() 	{ return; }

	virtual bool render(MatrixColumn * mc, Screen * curscr);

	virtual void compress(MatrixColumn * , Screen * )
				{ return; }

	virtual void reset()	{ _lastcycle = -1; }

protected:
	// How long the empty column should persist before
	// doing anything else
	int	_duration;
	
	// The number of the last cycle this event was at the top
	// of the queue for; -1 means it's never been rendered
	int	_lastcycle;
};

class MCE_Clear : public MatrixColumnEvent {
public:
	MCE_Clear() { return; }

	virtual ~MCE_Clear() { return; }

	virtual bool render(MatrixColumn * mc, Screen * curscr);
	
	virtual void compress(MatrixColumn * mc, Screen * curscr);

	virtual void reset() { return; }

protected:
	void doclear(MatrixColumn * mc, Screen * curscr);
	
};

class MCE_SetAttr : public MatrixColumnEvent {
public:
	MCE_SetAttr(int newattr);
	MCE_SetAttr(std::vector<int> & newattrvec);

	virtual ~MCE_SetAttr() { return; }

	virtual bool render(MatrixColumn * mc, Screen * curscr);
	
	virtual void compress(MatrixColumn * mc, Screen * curscr);

	virtual void reset() { return; }

protected:
	int	_newattr;
	std::vector<int> _newattrvec;
	
};

class MCE_SetString : public MatrixColumnEvent {
public:
	MCE_SetString(const char * newstr);

	virtual ~MCE_SetString() { return; }

	virtual bool render(MatrixColumn * mc, Screen * curscr);
	
	virtual void compress(MatrixColumn * mc, Screen * curscr);

	virtual void reset() { return; }

protected:

	// The new string to set
	char _newstr[MC_MAX_STR];
};

class MCE_StringFill : public MatrixColumnEvent {
public:
	MCE_StringFill() { return; }

	virtual ~MCE_StringFill() { return; }

	virtual bool render(MatrixColumn * mc, Screen * curscr);
	
	virtual void compress(MatrixColumn * mc, Screen * curscr);

	virtual void reset() { return; }

protected:

	void dofill(MatrixColumn * mc, Screen * curscr);
};

class MCE_StringDrop : public MatrixColumnEvent {
public:
	// How quickly to drop, number of characters to drop before
	// completion, whether to continue from previous drop,
	// and what the attributes of the head character should be
	MCE_StringDrop(float speed, int charcount, bool cont, int headchardrop);

	virtual ~MCE_StringDrop() { return; }
	
	virtual bool render(MatrixColumn * mc, Screen * curscr);

	virtual void compress(MatrixColumn * mc, Screen * curscr);

	virtual void reset();
protected:
	float	_speed;
	int	_charcount;
	int	_orig_charcount;
	bool	_cont;
	bool	_orig_cont;
	int	_headcharattr;
};

class MCE_RepScript : public MatrixColumnEvent {
public:
	MCE_RepScript(std::vector<MatrixColumnEvent *> & eventlist);

	virtual ~MCE_RepScript();

	virtual bool render(MatrixColumn * mc, Screen * curscr);
	
	virtual void compress(MatrixColumn * , Screen * )
				{ return; }

	virtual void reset();

protected:

	std::vector<MatrixColumnEvent *> _eventlist;
	std::vector<MatrixColumnEvent *>::iterator _curevent;

};
#endif
