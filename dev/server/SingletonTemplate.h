#ifndef TESTSINGLETON_H
#define TESTSINGLETON_H

// Foreign Includes
#include "Ref.h"

// Local Includes
#include "Session.h"

// Represents a single machine monitoring point
class TestSingleton
{
public:
	// Object state & control
	static bool		startup (void);
	static bool		shutdown (void);
	
private:
	// Monitor threads
	static void *		TestThread (void * arg);
	static bool		_TestThread_running;

	// Private members
	static bool _running;
	static bool _shutdown;
};

#endif
