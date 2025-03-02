#ifndef TIMER_H
#define TIMER_H


#include "common.h"
#include "CEventBuf.h"
#include "os_io.h"

class CTimerCtrl;

class CTimer{
public:

	using Timeout_Func = void(*)();
	void set_timeout(uint timeout, Timeout_Func p_fn = 0);
		
	bool is_timeout() { return _is_timeout;}
	void timeout() {
		_is_timeout = true;
		if(_p_fn) _p_fn();
	}
	friend CTimerCtrl;

private:
	uint _timeout = 0;
	bool _is_timeout = true;
	Timeout_Func _p_fn = 0;
};


class CTimerCtrl{
private:
	static constexpr uint _max = 64;
	uint _count =0;
	CEventBuf<64>* p_event_buf =0;
	CTimer _timers[_max];
	uint _last = 0;
	
public:
	void init(CEventBuf<64>* p_evt)  {p_event_buf=p_evt; }
	void inc();
	void add_timer(uint timeout, CTimer::Timeout_Func p_fn = 0) {
		if (_last < _max){
			_timers[_last++].set_timeout(_count + timeout, p_fn);
		}
	}
	void set_timer(uint idx, uint timeout, CTimer::Timeout_Func p_fn = 0)
	{
		if(idx < _last){
			_timers[idx].set_timeout(_count+timeout, p_fn);
		}
	}
	void timeout(uint idx){
		if(idx < _last){
			_timers[idx].timeout();
		}
	}
};







#endif