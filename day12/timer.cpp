#include "inc\timer.h"
#include "inc\global.h"


void CTimer::set_timeout(uint timeout,Timeout_Func p_fn)
{
	int eflags = io_load_eflags();
	io_cli();
	_timeout = timeout;
	_is_timeout = false;
	_p_fn = p_fn;
	io_store_eflags(eflags);

}

void CTimerCtrl::inc() 
{
	
	++_count;
	for(uint i=0; i<_last; ++i)
	{
		if( !_timers[i].is_timeout() && _timers[i]._timeout == _count)
		{ 
			//_timers[i].timeout();
			p_event_buf->set_char(i);
		}
	}

}
