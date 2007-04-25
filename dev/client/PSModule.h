#ifndef PSMODULE_H
#define PSMODULE_H

// Local Includes
#include "AbstractModule.h"

class PSModule : public AbstractModule {
public:
	PSModule() { return; }
	virtual ~PSModule() { return; }

	// regular members
	virtual void processchar(int c);

	virtual AbstractModule * execute(Screen & scr, std::vector<MatrixColumn *> & MClist);

private:
	bool _terminate;
};

#endif
