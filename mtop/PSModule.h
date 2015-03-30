#ifndef PSMODULE_H
#define PSMODULE_H

// System includes
#include <map>

// Foreign includes
#include <proc/readproc.h>

// Local Includes
#include "AbstractModule.h"

class Proc {
public:
	Proc(proc_t * ptsk) : _pnew(true), _palive(true), _ptsk(ptsk), _cpu(1) {}

	bool _pnew;
	bool _palive;
	proc_t * _ptsk;
	float _cpu;
	unsigned long long _tics;
	char _buf[1024];
};

class PSModule : public AbstractModule {
public:
	PSModule();
	virtual ~PSModule();

	// regular members
	virtual void processchar(int c);

	virtual AbstractModule * execute(Screen & scr, std::vector<MatrixColumn *> & MClist);

private:
	// Locking waffle
	pthread_mutex_t  _lock;
	pthread_cond_t   _cond;
	std::deque<char>	_charqueue;

	// indicate user termination request
	bool _terminate;

	// Pid sorted process list
	std::map<int, Proc> _pid_Proc_map;
	// Speed sorted process list
	std::multimap<float, Proc *> _cpu_Proc_map;

	// MColumn sorted Proc list
	std::map<MatrixColumn *, Proc *> _MC_Proc_map;
	// Pid sorted MColumn list
	std::map<int, MatrixColumn *> _pid_MC_map;
	// LRU sorted MColumn list
	std::multimap<int, MatrixColumn *> _LRU_MC_map;
	
};

#endif
