// System Includes
#include <time.h>
#include <pthread.h>

// Local includes 
#include "TestSingleton.h"

// Static members

bool TestSingleton::_running = false;
bool TestSingleton::_shutdown = false;
bool TestSingleton::_TestThread_running = false;

// Object state & control

bool TestSingleton::startup(void)
{
	pthread_t tid;

	// Check that we're not already running
	// or in the middle of a shutdown
	if (_running || _shutdown)
	{
		return false;
	}

	// Start TestThread
	_TestThread_running = true;
	if (pthread_create(&tid, NULL, TestThread, NULL))
	{
		return false;
	}

	// OK, we're running, so indicate that
	_running = true;
	return true;
}

bool TestSingleton::shutdown(void)
{
	// Check to make sure we're running and
	// not in the middle of a shutdown already
	if (!_running || _shutdown)
	{
		return false;
	}

	// OK, signal the shutdown
	_shutdown = true;

	// Shutdown TestThread
	while (_TestThread_running)
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

void * TestSingleton::TestThread(void * arg)
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
	}
	
	// Signal that we're done
	_TestThread_running = false;
	return NULL;
}
