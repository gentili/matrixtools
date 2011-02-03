#ifndef CLOCKMODULE_H
#define CLOCKMODULE_H

// Local Includes
#include "AbstractModule.h"

class ClockModule : public AbstractModule {
public:
	ClockModule() { return; }
	virtual ~ClockModule() { return; }

	// regular members
	virtual void processchar(int c);

	virtual AbstractModule * execute(Screen & scr, std::vector<MatrixColumn *> & MClist);
};

#endif
