// System Includes
#include <time.h>
#include <pthread.h>

// Local includes 
#include "MachineSingleton.h"

//// Static members

// Control
bool MachineSingleton::_running = false;
bool MachineSingleton::_shutdown = false;
bool MachineSingleton::_MachineLoadMonitor_running = false;
vector<Ref<Session>*> MachineSingleton::_MachineLoadMonitor_subscribers;

// Data
MachineInfo MachineSingleton::_curMI;
MachineLoadInfo MachineSingleton::_curMLI;

// Sync
pthread_mutex_t MachineSingleton::_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t MachineSingleton::_cond = PTHREAD_COND_INITIALIZER;

// Object state & control

bool MachineSingleton::startup(void)
{
	pthread_t tid;

	// Check that we're not already running
	// or in the middle of a shutdown
	if (_running || _shutdown)
	{
		return false;
	}

	// Start MachineLoadMonitor thread
	_MachineLoadMonitor_running = true;
	if (pthread_create(&tid, NULL, MachineLoadMonitor, NULL))
	{
		return false;
	}

	// OK, we're running, so indicate that
	_running = true;
	return true;
}

bool MachineSingleton::shutdown(void)
{
	// Check to make sure we're running and
	// not in the middle of a shutdown already
	if (!_running || _shutdown)
	{
		return false;
	}

	// OK, signal the shutdown
	_shutdown = true;

	// Shutdown MachineLoadMonitor thread
	while (_MachineLoadMonitor_running)
	{
		struct timespec wait;
		wait.tv_sec = 1;
		wait.tv_nsec = 0;
		nanosleep (&wait, NULL);
	}
	
	// We're all done shutting down, so 
	// set everything back to the uninit state
	_running = false;
	_shutdown = false;
	return true;
}

void * MachineSingleton::MachineLoadMonitor(void * arg)
{
	// Set up signal blocking
	sigset_t new_sig_mask;
	sigemptyset(&new_sig_mask);
	sigaddset(&new_sig_mask, SIGINT);
	sigaddset(&new_sig_mask, SIGPIPE);
	if (pthread_sigmask(SIG_BLOCK, &new_sig_mask, NULL) < 0)
		throw;
	if (pthread_detach(pthread_self()) < 0)
		throw;

	while (!_shutdown)
	{
		struct timespec wait;
		wait.tv_sec = 1;
		wait.tv_nsec = 0;
		nanosleep (&wait, NULL);

		// Go through the subscription list
		pthread_mutex_lock (&_lock);
		for (vector<Ref<Session>*>::iterator sitr = _MachineLoadMonitor_subscribers.begin();
				sitr != _MachineLoadMonitor_subscribers.end();
				sitr++)
		{
			// Send an update if there's been a change
			(**sitr)->update_MachineLoadInfo(_curMLI);
		}
		pthread_mutex_unlock (&_lock);
	}
	
	// Signal that we're done
	_MachineLoadMonitor_running = false;
	return NULL;
}

// Regular members

MachineInfo MachineSingleton::get_subs_MachineInfo(Ref<Session> & sessionref)
{
	if (!_running)
		throw MSE_shutdown();

	pthread_mutex_lock (&_lock);
	// Currently, the machineinfo doesn't change, so there's no
	// actual subscription that needs to take place.

	MachineInfo tempMI = _curMI;
	
	pthread_mutex_unlock (&_lock);

	return tempMI;
}

// NOTE: A subscriber may receive an update before these subscription
// functions return, so they should be ready for that.
MachineLoadInfo MachineSingleton::get_subs_MachineLoadInfo(Ref<Session> & sessionref)
{
	if (!_running)
		throw MSE_shutdown();

	// OK, we need to create a new ref and put it on the
	// MLI subscription list
	Ref<Session> * newsessionref = new Ref<Session> (sessionref);

	pthread_mutex_lock (&_lock);

	_MachineLoadMonitor_subscribers.push_back(newsessionref);
	
	// Now return the current load
	MachineLoadInfo tempMLI;
	
	pthread_mutex_unlock (&_lock);
	
	return tempMLI;
}
