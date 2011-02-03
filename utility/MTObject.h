#ifndef MTOBJECT_H
#define MTOBJECT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <signal.h>
#include <pthread.h>

#define	MAXTHREADS 1000

class MTObject
{
public:
	MTObject();
	virtual ~MTObject();

	virtual int createThreads(int);
	virtual int createThreads();
	void	threadExit();
	int	threads();

	void	terminate()		{ _exitflag = true; }
	bool	shouldterminate()	{ return _exitflag; }

	virtual void	run();

protected:

	pthread_mutex_t	_lock;
	int	_threads;
	bool	_exitflag;

	pthread_t	tid[MAXTHREADS];

};

#endif 
