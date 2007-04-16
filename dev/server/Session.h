#ifndef SESSION_H
#define SESSION_H

// System Includes
#include <vector>

// Local includes

#include "MachineData.h"
#include "NetworkData.h"
#include "UserData.h"
#include "ProcessData.h"

class Session 
{
public:
   // Control
	Session() { _active = true; }
	virtual void deactivate() { _active = false; }
	virtual bool active() { return _active; }
	virtual ~Session() { };

   // Machine
	virtual bool		update_MachineInfo(MachineInfo&) = 0;
	virtual bool		update_MachineLoadInfo(MachineLoadInfo&) = 0;

   // Network
	virtual bool		update_NetworkInfo (NetworkInfo&) = 0;
	virtual bool		update_NetworkLoadInfo (NetworkLoadInfo&) = 0;
	virtual bool		update_NetworkConnectionInfo (std::vector < NetworkConnectionInfo > &) = 0;
	virtual bool		update_NetworkActivityInfo (std::vector < NetworkActivityInfo > &) = 0;

   // User
	virtual bool		update_UserInfo (UserInfo &) = 0;
	virtual bool		update_UserActivityInfo (UserActivityInfo &) = 0;

   // Process
	virtual bool		update_ProcessInfo(ProcessInfo &) = 0;
	virtual bool		update_ProcessActivityInfo(ProcessActivityInfo &) = 0;

private:
   // Control
	bool _active;
};

#endif
