// System Includes
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>

// Local Includes
#include "Screen.h"
#include "MatrixColumn.h"
#include "PSModule.h"

// AbstractModule Interface specification
void PSModule::processchar (int c)
{
}

AbstractModule * PSModule::execute(Screen & scr, std::vector<MatrixColumn *> & MClist)
{
	return NULL;
}
