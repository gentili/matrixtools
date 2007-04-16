// System Includes
#include <unistd.h>
#include <stdio.h>
#include <vector>

// Foreign includes
#include "Ref.h"

// Local includes
#include "Session.h"
#include "MachineSingleton.h"

class TestSession : public Session
{
public:
	virtual ~TestSession() { printf ("~Session()\n"); }

   // Machine
	virtual bool		update_MachineInfo(MachineInfo&);
	virtual bool		update_MachineLoadInfo(MachineLoadInfo&);

   // Network
	virtual bool		update_NetworkInfo (NetworkInfo&) 
		{ return _active; }
	virtual bool		update_NetworkLoadInfo (NetworkLoadInfo&)
		{ return _active; }
	virtual bool		update_NetworkConnectionInfo (std::vector < NetworkConnectionInfo > &)
		{ return _active; }
	virtual bool		update_NetworkActivityInfo (std::vector < NetworkActivityInfo > &)
		{ return _active; }

   // User
	virtual bool		update_UserInfo (UserInfo &)
		{ return _active; }
	virtual bool		update_UserActivityInfo (UserActivityInfo &)
		{ return _active; }

   // Process
	virtual bool		update_ProcessInfo(ProcessInfo &)
		{ return _active; }
	virtual bool		update_ProcessActivityInfo(ProcessActivityInfo &)
		{ return _active; }

private:
	bool	_active;

};

bool TestSession::update_MachineInfo(MachineInfo & newMinfo)
{
	return _active;
}

bool TestSession::update_MachineLoadInfo(MachineLoadInfo & newMLinfo)
{
	printf ("update_MachineLoadInfo() - got update!\n");
	return _active;
}

///////////////////////////////////////////
// MAIN entrypoint
///////////////////////////////////////////

int main(int argc, char * argv[])
{
	Session * ts = new TestSession();
	Ref<Session> * tsr = new Ref<Session> (ts);

	printf ("main() - Starting MachineSingleton\n");
	if (!MachineSingleton::startup())
	{
		printf ("ABORT: Unable to start MachineSingleton\n");
		return -1;
	}

	printf ("main() - Subscribing to MachineLoadInfo updates\n");
	MachineSingleton::get_subs_MachineLoadInfo(*tsr);

	printf ("main() - Sleeping 10 seconds\n");
	struct timespec wait;
	wait.tv_sec = 10;
	wait.tv_nsec = 0;
	nanosleep (&wait, NULL);

	printf ("main() - Shutting down MachineSingleton\n");
	if (!MachineSingleton::shutdown())
	{
		printf ("ABORT: Unable to shutdown MachineSingleton\n");
		return -1;
	}

	return 0;
}
