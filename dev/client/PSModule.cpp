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

PSModule::PSModule()
{
	pthread_mutex_init(&_lock, NULL);
	pthread_cond_init(&_cond, NULL);
}

PSModule::~PSModule()
{
	pthread_mutex_destroy(&_lock);
	pthread_cond_destroy(&_cond);
}

// AbstractModule Interface specification
void PSModule::processchar (int c)
{
	pthread_mutex_lock(&_lock);
	_charqueue.push_back(c);
	pthread_cond_signal(&_cond);
	pthread_mutex_unlock(&_lock);
}


AbstractModule * PSModule::execute(Screen & scr, std::vector<MatrixColumn *> & MClist)
{
	// FILE * log = fopen ("PSMatrix.log", "w+");
	// Seed the MC maps
	int i = 0;
	for (std::vector<MatrixColumn *>::iterator MCitr = MClist.begin();
			MCitr != MClist.end();
			MCitr++)
	{
		// Mark odd columns as unassigned (NULL)
		if (++i % 2)
			_MC_Proc_map.insert(std::pair < MatrixColumn *, Proc * > (*MCitr, NULL));
	}

	bool firstloop = true;
	///////////////////////
	// Main processing loop
	///////////////////////
	struct timeval oldtimev;
	struct timeval timev;
	_terminate = false;
	while (!_terminate)
	{
		// Figure out the elapsed time
		gettimeofday(&timev, NULL);
		float elapsed = (float) (timev.tv_sec - oldtimev.tv_sec)
			+ (float) (timev.tv_usec - oldtimev.tv_usec) / 1000000.0;
		oldtimev.tv_sec = timev.tv_sec;
		oldtimev.tv_usec = timev.tv_usec;
		float timescale = 100.0f / ((float)Hertz * (float)elapsed);

		// Clear speed sorted list
		_cpu_Proc_map.clear();
		// Scan the list and mark everyone as old and dead
		for (std::map<int, Proc>::iterator procitr = _pid_Proc_map.begin();
				procitr != _pid_Proc_map.end();
				procitr++)
		{
			//fprintf (log, "Mark OldDead: %d\n",procitr->first);
			assert (procitr->first == procitr->second._ptsk->tid);
			procitr->second._pnew = false;
			procitr->second._palive = false;
			procitr->second._cpu = 0;
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
				char cmd[1024];
				int cmdlen = 1024;
				escape_command(cmd,procitr->second._ptsk,1024,&cmdlen,ESC_ARGS);
				snprintf (procitr->second._buf, 1024, "%d %s *** ",
						procitr->second._ptsk->tid,
						cmd);
				//fprintf (log, "Mark New: %d\n",procitr->first);
			} else
			// if exists
			{
				//fprintf (log, "Mark Alive: %d\n",procitr->first);
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
		// OK, if this is our first time through the loop then we don't
		// need to do any of the following, so skip it.
		if (firstloop)
		{
			firstloop = false;
			continue;
		}
		// Regenerate the LRU and pid sorted MColumn list
		_LRU_MC_map.clear();
		_pid_MC_map.clear();
		for (std::map<MatrixColumn *, Proc *>::iterator MCitr = _MC_Proc_map.begin();
				MCitr != _MC_Proc_map.end();
				MCitr++)
		{
			if (MCitr->second == NULL)
			{
				//fprintf (log, "MC NULL: %p\n",MCitr->first);
				// If is no associated proc, give it a negative cpu
				// so it will be picked up before zero cpu procs
				
				_LRU_MC_map.insert(std::pair < int, MatrixColumn * > (MCitr->first->getLRU(),MCitr->first));

				
			} else if (!MCitr->second->_palive)
			{
				//fprintf (log, "MC DIED: %p : %d\n",MCitr->first,MCitr->second->_ptsk->tid);
				// If proc is dead, do a reset and flash fill of
				// the column, set the proc pointer to null
				// and give it zero cpu

				MCitr->first->resetLRU();
				MCitr->first->add_setattr_event(true,false,false,scr.curs_attr_reverse() | scr.curs_attr_red());
				MCitr->first->add_clear_event(false, false, false);
				MCitr->first->add_delay_event(false,false,false,5);
				MCitr->first->add_setattr_event(false,false,false,scr.curs_attr_bold() | scr.curs_attr_red());
				MCitr->first->add_setstring_event(false,false,false,MCitr->second->_buf);
				MCitr->first->add_stringfill_event(false,false,false);
				MCitr->second = NULL;

				// Make it just a little less likely to be picked up
				// than a halted running process, so we force walk about
				_LRU_MC_map.insert(std::pair < int, MatrixColumn * > (MCitr->first->getLRU(),MCitr->first));
			} else
			{
				//fprintf (log, "MC ALIVE: %p : %d : %f\n",MCitr->first,MCitr->second->_ptsk->tid, MCitr->second->_cpu);
				// Then adjust the update rate accordingly
				MCitr->first->add_setattr_event(false,false,false, 
						scr.curs_attr_green());
				MCitr->first->add_stringdrop_event(false,true,false,
						MCitr->second->_cpu*0.1,-1,true,
						scr.curs_attr_bold() | scr.curs_attr_white());
				// If the process is sleeping, don't reset the column's LRU
				if (MCitr->second->_cpu > 0)
					MCitr->first->resetLRU();
				// Proc is alive so put it in both sorted lists
				_LRU_MC_map.insert(std::pair < int, MatrixColumn * > (MCitr->first->getLRU(), MCitr->first));
				_pid_MC_map.insert(std::pair < int, MatrixColumn * > (MCitr->second->_ptsk->tid, MCitr->first));
			}
		}
		// Cull dead processes
		for (std::map<int, Proc>::iterator procitr = _pid_Proc_map.begin();
				procitr != _pid_Proc_map.end(); )
		{
			// If dead then remove
			std::map<int, Proc>::iterator deaditr = procitr;
			procitr++;
			if (!deaditr->second._palive)
			{
				//fprintf (log, "PROC DEAD: %d\n",procitr->first);
				// Safe to do this as we're guaranteed not to be
				// in the cpu sorted list because we're dead
				free ((void*)*(deaditr->second._ptsk)->cmdline);
				free (deaditr->second._ptsk);
				_pid_Proc_map.erase(deaditr);
			}
		}

		// OK, go through the top CPU users
		int MCcount = _MC_Proc_map.size();
		for (std::map<float, Proc*>::iterator procitr = _cpu_Proc_map.end();
				(procitr != _cpu_Proc_map.begin()) && (MCcount); 
				)
		{
			procitr--;
			MCcount--;
			//fprintf (log, "EXAMINING: %d : %f\n",procitr->second->_ptsk->tid, procitr->first);
			// Are we down in the zero cpu processes?
			if ((procitr->second->_cpu <= 0) && (!procitr->second->_pnew))
				// Then to avoid bottom feeder thrashing
				// we get out.
				break;
			// Is this pid already in the MC list?
			if (_pid_MC_map.end() != _pid_MC_map.find(procitr->second->_ptsk->tid))
				// Then it's already been dealt with, so skip it
				continue;

			// OK, so get the LRU column
			std::map<int, MatrixColumn *>::iterator LRUMCitr = 
				_LRU_MC_map.end();
			assert (LRUMCitr != _LRU_MC_map.begin());
			LRUMCitr--;
			// Look it up in the MC sorted proc list and make the association
			std::map<MatrixColumn *, Proc *>::iterator MCProcitr =
				_MC_Proc_map.find(LRUMCitr->second);
			assert (MCProcitr != _MC_Proc_map.end());
			MCProcitr->second = procitr->second;

			// Now pop the lowest speed MC
			_LRU_MC_map.erase(LRUMCitr);
			
			// Set the attributes
			int newattr = scr.curs_attr_bold();
			if (procitr->second->_pnew)
			{
				newattr |= scr.curs_attr_green();
			} else {
				newattr |= scr.curs_attr_blue();
			}
			
			// Brand new processes get a full speed drop
			MCProcitr->first->resetLRU();
			MCProcitr->first->add_setattr_event(true,false,false, newattr);
			MCProcitr->first->add_clear_event(false,false,false);
			MCProcitr->first->add_setstring_event(false,false,false,MCProcitr->second->_buf);
			MCProcitr->first->add_stringdrop_event(false,false,false,
					1,(int) scr.maxy() < (int) strlen(MCProcitr->second->_buf) ? scr.maxy() : strlen(MCProcitr->second->_buf),
					false, scr.curs_attr_bold() | scr.curs_attr_white());
				
		}
		// OK, now do the checkin for input and sleepin thing
		struct timespec timeout;
		struct timeval now;
		gettimeofday (&now, NULL); 
		timeout.tv_sec = now.tv_sec + 1;
		timeout.tv_nsec = now.tv_usec * 1000;
		pthread_mutex_lock(&_lock);

		pthread_cond_timedwait(&_cond,
				&_lock,
				&timeout);

		std::deque<char> _curchars;
		if (!_charqueue.empty())
		{
			_curchars = _charqueue;
			_charqueue.clear();
		}

		pthread_mutex_unlock(&_lock);
		
		while (!_curchars.empty())
		{
			// Exit char
			if (_curchars.front() == 'x')
				_terminate = true;
			// Clear dead processes
			bool cleandead = false;
			if (_curchars.front() == 'd')
				cleandead = true;
			// Redraw all processes
			bool redraw = false;
			if (_curchars.front() == 'r')
				redraw = true;
			// Clear all inactive processes
			bool cleaninactive = false;
			if (_curchars.front() == 'c')
				cleaninactive = true;

			// Redraw processing
			if (redraw)
			{
				for (std::map<MatrixColumn *, Proc *>::iterator MCitr = _MC_Proc_map.begin();
						MCitr != _MC_Proc_map.end();
						MCitr++)
				{
					if (!MCitr->second)
						continue;

					float speed = (float) random() / (float) RAND_MAX * 0.5 + 0.5;
					MCitr->first->resetLRU();
					MCitr->first->add_clear_event(true,false,false);
					MCitr->first->add_stringdrop_event(false,false,false,
							speed,
							(int) scr.maxy() < (int) strlen(MCitr->second->_buf) ? scr.maxy() : strlen(MCitr->second->_buf),
							false,scr.curs_attr_bold() | scr.curs_attr_white());
				}
			}
			// Clean processing
			if (cleaninactive || cleandead)
			{
				for (std::map<MatrixColumn *, Proc *>::iterator MCitr = _MC_Proc_map.begin();
						MCitr != _MC_Proc_map.end();
						MCitr++)
				{
					if (MCitr->second)
					{
						if (!cleaninactive || MCitr->second->_cpu != 0)
							continue;
						MCitr->second = NULL;
					}
					float speed = (float) random() / (float) RAND_MAX * 0.5 + 0.5;
					MCitr->first->resetLRU();
					MCitr->first->add_setattr_event(false,false,false, scr.curs_attr_green());
					MCitr->first->add_setstring_event(false,false,false," ");
					MCitr->first->add_stringdrop_event(false,false,false,
							speed,(int) scr.maxy(),false, scr.curs_attr_white());
				}
			}
					
			_curchars.pop_front();
		}
	}

	return NULL;
}
