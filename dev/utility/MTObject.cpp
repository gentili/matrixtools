#include "MTObject.h"

static void *start_routine(void *arg)
{
	MTObject*	pseudo_this=(MTObject*) NULL;

	sigset_t        new_sig_mask;           //THREADS IGNORE SIGINT AND SIGTERM
	sigemptyset(&new_sig_mask);
	sigaddset(&new_sig_mask,SIGINT);
	sigaddset(&new_sig_mask,SIGPIPE);

	if(pthread_sigmask(SIG_BLOCK,&new_sig_mask,NULL)<0)
		throw;
		// cerr << "MTObject::start_routine - can't ignore signals; pthread_sigmask()" << endl;

	if(pthread_detach(pthread_self())<0)
		throw;
		// cerr << "MTObject::start_routine - can't detach threads; pthread_detach()" << endl;

	pseudo_this=(MTObject*)arg;
	pseudo_this->run();

	return NULL;
}

MTObject::MTObject()
{
	pthread_mutex_init(&_lock,NULL);
	_threads = 0;
	_exitflag = false;
}

int MTObject::createThreads(int num = 1)
{
	int errcode = 0;
	_threads = num;

	for(int i = 0; i < num; i++)
	{
		errcode=pthread_create(&tid[i],NULL,start_routine,(void *)this);
		if(errcode!=0)
		{
			throw;
			// Common::logError("MTObject::MTObject() - pthread_create returned: %d ", errcode);
			return -1;
		} 
	}

	return 0;
}

int MTObject::createThreads()
{
	return createThreads(1);
}

MTObject::~MTObject()
{
	pthread_mutex_destroy(&_lock);
}

void MTObject::run()
{
#if DEBUG
	Common::logDebug("MTObject::run()");
#endif
	return;
}		

void MTObject::threadExit()
{
	pthread_mutex_lock(&_lock);
	_threads--;
	pthread_mutex_unlock(&_lock);
}

int MTObject::threads()
{
	int tmp=0;
	pthread_mutex_lock(&_lock);
	tmp=_threads;
	pthread_mutex_unlock(&_lock);

	return tmp;
}
