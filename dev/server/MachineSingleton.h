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
	static vector<Ref<Session>*> _MachineLoadMonitor_subscribers;

	// Control members
	static bool _running;
	static bool _shutdown;

	// Data members
	static MachineInfo _curMI;
	static MachineLoadInfo _curMLI;

	// Data mutexes and conditionals
	static pthread_mutex_t _lock;
	static pthread_cond_t _cond;
};

class MachineSingletonException {
public:
	MachineSingletonException() { };
	virtual ~MachineSingletonException() { };
};

class MSE_shutdown : public MachineSingletonException {
public:
	MSE_shutdown() { };
	virtual ~MSE_shutdown() { };
};
#endif
