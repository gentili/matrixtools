#ifndef AUTOLOCK_H
#define AUTOLOCK_H

#include <pthread.h>

class AutoLock 
{
private:
	pthread_mutex_t* lck;
public:
	AutoLock(pthread_mutex_t* l)
	{
		lck = l;
		pthread_mutex_lock(lck);
	};

	~AutoLock()
	{
		pthread_mutex_unlock(lck);
	};

};

#endif
