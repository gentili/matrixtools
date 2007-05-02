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
	int i = 0;
	for (std::vector<MatrixColumn *>::iterator MCitr = MClist.begin();
			MCitr != MClist.end();
			MCitr++)
	{
		// Mark odd columns as unassigned (NULL)
		if (i++ % 2)
			_MC_Proc_map.insert(std::pair < MatrixColumn *, Proc * > (*MCitr, NULL));
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
		// Regenerate the speed and pid sorted MColumn list
		_cpu_MC_map.clear();
		_pid_MC_map.clear();
		for (std::map<MatrixColumn *, Proc *>::iterator MCitr = _MC_Proc_map.begin();
				MCitr != _MC_Proc_map.end();
				MCitr++)
		{
			if (MCitr->second == NULL)
			{
				// If is no associated proc, give it a negative cpu
				// so it will be picked up before zero cpu procs
				
				_cpu_MC_map.insert(std::pair < float, MatrixColumn * > (-1.0,MCitr->first));

				
			} else if (!MCitr->second->_palive)
			{
				// If proc is dead, do a reset and flash fill of
				// the column, set the proc pointer to null
				// and give it zero cpu
				char buf[80];
				char cmd[80];
				int cmdlen = 80;
				escape_command(cmd,MCitr->second->_ptsk,80,&cmdlen,ESC_ARGS);
				snprintf (buf, 80, "%d %s          ",
						MCitr->second->_ptsk->tid,
						cmd);

				MCitr->first->add_clear_event(true, false, false);
				MCitr->first->add_setattr_event(false,false,false,scr.curs_attr_bold() | scr.curs_attr_reverse() | scr.curs_attr_red());
				MCitr->first->add_setstring_event(false,false,false,buf);
				MCitr->first->add_stringfill_event(false,false,true);
				MCitr->second = NULL;

				_cpu_MC_map.insert(std::pair < float, MatrixColumn * > (0,MCitr->first));
			} else
			{
				// Proc is alive so put it in both sorted lists
				_cpu_MC_map.insert(std::pair < float, MatrixColumn * > (MCitr->second->_cpu, MCitr->first));
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
			// Is this pid already in the MC list?
			std::map<int, MatrixColumn *>::iterator pidMCitr = 
				_pid_MC_map.find(procitr->second->_ptsk->tid);
			if (pidMCitr != _pid_MC_map.end())
			{
				// Yes, adjust the update rate accordingly
				pidMCitr->second->add_setattr_event(false,false,false, scr.curs_attr_green());
				pidMCitr->second->add_stringdrop_event(false,true,false,procitr->first*0.1,-1,true, scr.curs_attr_bold() | scr.curs_attr_white());
			} else
			{
				// No, get the lowest speed MC
				std::map<float, MatrixColumn *>::iterator cpuMCitr = 
					_cpu_MC_map.begin();
				assert (cpuMCitr != _cpu_MC_map.end());
				// Look it up in the MC sorted proc list and make the association
				std::map<MatrixColumn *, Proc *>::iterator MCProcitr =
					_MC_Proc_map.find(cpuMCitr->second);
				assert (MCProcitr != _MC_Proc_map.end());
				MCProcitr->second = procitr->second;

				// Now pop the lowest speed MC
				_cpu_MC_map.erase(_cpu_MC_map.begin());
				
				// Set the string
				char buf[1024];
				char cmd[1024];
				int cmdlen = 1024;
				escape_command(cmd,procitr->second->_ptsk,1024,&cmdlen,ESC_ARGS);
				snprintf (buf, 1024, "%d %s         ",
						procitr->second->_ptsk->tid,
						cmd);
				if (procitr->second->_pnew)
				{
					// New processes get a full speed drop
					MCProcitr->first->add_setattr_event(false,false,false, scr.curs_attr_bold() | scr.curs_attr_green());
					MCProcitr->first->add_setstring_event(false,false,false,buf);
					MCProcitr->first->add_stringdrop_event(false,false,false,1,scr.maxy() < strlen(buf) ? scr.maxy() : strlen(buf),false, scr.curs_attr_bold() | scr.curs_attr_white());
					MCProcitr->first->add_setattr_event(false,false,false, scr.curs_attr_green());
					MCProcitr->first->add_stringfill_event(false,false,false);
					
				} 
				else
				{
					// Old processes get a regular speed drop
					MCProcitr->first->add_setattr_event(false,false,false, scr.curs_attr_green());
					MCProcitr->first->add_clear_event(false,false,false);
					MCProcitr->first->add_setstring_event(false,false,false,buf);
					MCProcitr->first->add_stringdrop_event(false,true,false,procitr->first*0.1,-1,false, scr.curs_attr_bold() | scr.curs_attr_white());
				}
			}
			MCcount--;
		}
		/*
		int i=1;
		for (std::map<float, Proc*>::iterator procitr = _cpu_Proc_map.end();
				procitr != _cpu_Proc_map.begin(); 
				)
		{
			procitr--;
			ptsk = procitr->second->_ptsk;
//			if ((procitr->second->_tics == 0))
//				continue;
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
		*/
	}

	return NULL;
}
