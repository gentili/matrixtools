// System Includes
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>

// Foreign includes
#include "readproc.h"
#include "sysinfo.h"
#include "escape.h"

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
	// Seed the MC maps
	for (std::vector<MatrixColumn *>::iterator MCitr = MClist.begin();
			MCitr != MClist.end();
			MCitr++)
	{
		/*
		i--;
		// Mark all of the columns as unassigned (pid 0)
		std::pair <std::map<int, MatrixColumn *>::iterator, bool> pidMCitrboolpair = 
			_pid_MC_map.insert(std::pair < int, MatrixColumn * > (0,*MCitr));
		// Give all columns negative cpu so they will be first to be consumed
		_cpu_pid_MC_map.insert(std::pair <double, std::multimap<int, MatrixColumn *>::iterator> (-1, pidMCitrboolpair.first));
		*/
	}

	///////////////////////
	// Main processing loop
	///////////////////////
	struct timeval oldtimev;
	struct timeval timev;
	_terminate = false;
	while (!_terminate)
	{
		sleep (1);
		// Figure out the elapsed time
		gettimeofday(&timev, NULL);
		float elapsed = (timev.tv_sec - oldtimev.tv_sec)
			+ (float)(timev.tv_usec - oldtimev.tv_usec) / 1000000.0;
		oldtimev.tv_sec = timev.tv_sec;
		oldtimev.tv_usec = timev.tv_usec;
		float timescale = 100.0f / ((float)Hertz * (float)elapsed);

		{
			char buf[80];
			snprintf (buf, 80, "STATS: %f",
					timescale);
			scr.curs_mvaddstr (0,1,buf);
		}

		// Clear speed sorted list
		_cpu_Proc_map.clear();
		// Scan the list and mark everyone as old and dead
		for (std::map<int, Proc>::iterator procitr = _pid_Proc_map.begin();
				procitr != _pid_Proc_map.end();
				procitr++)
		{
			procitr->second._pnew = false;
			procitr->second._palive = false;
		}

		// Do proc list processing
		PROCTAB* PT = openproc(PROC_FILLARG|PROC_FILLCOM|PROC_FILLMEM|PROC_FILLSTAT);
		proc_t * ptsk;
		while ( (ptsk = readproc(PT, NULL)) )
		{
			if (ptsk->cmdline == NULL)
			{
				free (ptsk);
				continue;
			}
			// Look up proc in Full list
			std::map <int, Proc>::iterator procitr = _pid_Proc_map.find(ptsk->tid);
			// if doesn't exist
			if (procitr == _pid_Proc_map.end())
			{
				procitr = _pid_Proc_map.insert(std::pair<int, Proc>(ptsk->tid,Proc(ptsk))).first;
			} else
			// if exists
			{
				// marked as alive
				procitr->second._palive = true;
				// do speed calc 
				procitr->second._tics = ptsk->utime - procitr->second._ptsk->utime;
				procitr->second._tics += ptsk->stime - procitr->second._ptsk->stime;
				procitr->second._cpu = (float) procitr->second._tics * timescale;
				procitr->second._ptsk->utime = ptsk->utime;
				procitr->second._ptsk->stime = ptsk->stime;
				// delete this ptsk as we already have one
				free ((void*)*ptsk->cmdline);
				free (ptsk);
			}
			// insert in Cpu list
			_cpu_Proc_map.insert (std::pair<float, Proc *> (procitr->second._cpu, &procitr->second));
		}
		closeproc(PT);

		// Do display processing
		int i=1;
		for (std::map<float, Proc*>::iterator procitr = _cpu_Proc_map.begin();
				procitr != _cpu_Proc_map.end();
				procitr++)
		{
			ptsk = procitr->second->_ptsk;
			if ((procitr->second->_tics == 0))
				continue;
			char buf[80];
			char cmd[80];
			int cmdlen = 80;
			escape_command(cmd,ptsk,80,&cmdlen,ESC_ARGS);
			snprintf (buf, 80, "%d : %03f : %08llu : %s",
					ptsk->tid,
					procitr->second->_cpu,
					procitr->second->_tics,
					cmd);
			scr.curs_mvaddstr (i,1,buf);
			i++;
			if (i > scr.maxy())
				break;
		}
		// OK, go through the top CPU users
			// Is this pid still in the list?
				// Yes, adjust the update rate accordingly
				// No, pop the bottom 
		// New processes get a full speed drop
		// Dead processes on screen get a flashfill
		// The rest are sorted by cpu
	}

	return NULL;
}
