#ifndef ABSTRACTMODULE_H
#define ABSTRACTMODULE_H

// Local Includes
#include "Screen.h"
#include "MatrixColumn.h"

class AbstractModule {
public:
	// Constructor
	AbstractModule() { return; }
	// Destructor
	virtual ~AbstractModule() { return; }

	// regular members
	virtual void processchar(int c) = 0;
	
	virtual AbstractModule * execute(Screen & scr, std::vector<MatrixColumn *> & MClist) = 0;
};

#endif
