#ifndef MACHINESINGLETON_H
#define MACHINESINGLETON_H

// Foreign Includes
#include "Ref.h"
#include "MTObject.h"

// Local Includes
#include "Session.h"

// Represents a single machine monitoring point
class MachineSingleton : public MTObject
{
public:
	// Object state & control
	static bool		startup (void);
	static bool		shutdown (void);
	
	// Regular execution
	static MachineInfo	get_subs_MachineInfo (Ref<Session> &);
	static MachineLoadInfo	get_subs_MachineLoadInfo (Ref<Session> &);

	
private:
	// Monitor threads
	static void *		MachineLoadMonitor (void * arg);
	static bool		_MachineLoadMonitor_running;

	// Private members
	static bool _running;
	static bool _shutdown;
};

#endif
