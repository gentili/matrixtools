#ifndef SCREEN_H
#define SCREEN_H

#include "MTObject.h"

/* This class defines a screen in which lines may be put.  It is a 
 singleton (we don't have multiple screens) and grabs the default tty
 if any. */

class Screen : public MTObject
{
public:
	// Init grabs control of the tty, reads in all the specs,
	// and generally sets everything up to start updating the display
	bool init();

	bool shutdown();
	
protected:
	// This is the main update thread
	virtual void run();

	// This describes if the screen has been inited yet
	static bool _inited;

private:
};

#endif
