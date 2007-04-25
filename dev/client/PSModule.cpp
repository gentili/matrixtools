// System Includes
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <readproc.h>

// Local Includes
#include "Screen.h"
#include "MatrixColumn.h"
#include "PSModule.h"

// AbstractModule Interface specification
void PSModule::processchar (int c)
{
	if (c == 'x')
		_terminate = true;
}

AbstractModule * PSModule::execute(Screen & scr, std::vector<MatrixColumn *> & MClist)
{
	_terminate = false;
	sleep (1);
	PROCTAB* PT = openproc(PROC_FILLARG|PROC_FILLCOM|PROC_FILLMEM|PROC_FILLSTATUS);
	if (!PT)
	{
		return NULL;
	}
	proc_t * ptsk;
	int i = 1;
	while ((!_terminate) && 
			(ptsk = readproc(PT, NULL)) &&
			(i < scr.maxy()))
	{
		if (ptsk->cmdline == NULL)
		{
			free (ptsk);
			continue;
		}
		char buf[80];
		snprintf (buf, 80, "%d : %d : %c : %llu : %s",
				ptsk->tid,
				ptsk->ppid,
				ptsk->state,
				ptsk->start_time,
				ptsk->cmdline[0]);
		scr.curs_mvaddstr (i,1,buf);
		free (ptsk);
		i++;
	}
	closeproc(PT);
	while (!_terminate)
	{
		usleep (100000);
	}

	return NULL;
}
