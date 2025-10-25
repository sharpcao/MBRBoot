#include "inc\timer_kit.h"
#include "inc\winos.h"


void CTimer::set_timer(uint timeout,Timeout_Func p_fn)
{
	int eflags = io_load_eflags();
	io_cli();
	_timeout = timeout;
	_is_timeout = false;
	_p_fn = p_fn;

	io_store_eflags(eflags);

}



bool CTimerCtrl::add_timer(uint timeout, CTimer::Timeout_Func p_fn ,Message_mgr<>* p_evt)
{
	bool rtn = true;
	if (_last < _max){
		_timers[_last]._id = _last;
		_timers[_last].set_tm_event(p_evt);
		uint real_timeout = _count + timeout;
		_timers[_last].set_timer(real_timeout, p_fn);
		CTimer *it = _next_timer, *pre = 0;
		while(it->_timeout <= real_timeout) {
			pre = it;
			it = it->next();
		}
		if(pre) {
			pre->_next = &_timers[_last];
		}
		else{
			_next_timer = &_timers[_last];
		}
		_timers[_last]._next = it;
		++_last;
		
	}else{
		rtn = false;
	}
	return rtn;
}

void CTimerCtrl::insert_timer(CTimer* tm, CTimer* from)
{
	CTimer *it = from; 
	
	if((it==_next_timer) && (it->_timeout > tm->_timeout)){
		tm->_next = _next_timer;
		_next_timer = tm;
		return;
	}

	while(it && it->next()->_timeout <= tm->_timeout) it = it->next();
	tm->_next = it->next();
	it->_next = tm;
	
}

void CTimerCtrl::set_timer(uint idx, uint timeout, CTimer::Timeout_Func p_fn)
{
	
	int eflags = io_load_eflags();
	io_cli();

	if(idx>0 && idx < _last){
		uint real_timeout = _count+timeout;
		
		if(!_timers[idx].is_timeout()){
			_timers[idx].set_timer(real_timeout, p_fn);
			if(_timers[idx].next()->_timeout < real_timeout)
			{
				CTimer *it = _next_timer;
				if(it == &_timers[idx]){
					it = _next_timer = _timers[idx].next();
				}else{
					while(it && it->next() != &_timers[idx]) it = it->next();
					it->_next = _timers[idx].next();
					it = it->next();
				}
				insert_timer(&_timers[idx], it);

			}

		}else{

			_timers[idx].set_timer(real_timeout, p_fn);
			insert_timer(&_timers[idx], _next_timer);
		}

	}

	io_store_eflags(eflags);
}

