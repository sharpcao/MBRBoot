#ifndef TIMER_KIT_H
#define TIMER_KIT_H


#include "common.h"
#include "Message_kit.h"
#include "os_io.h"

class CTimerCtrl;

class CTimer{
public:

	using Timeout_Func = void(*)(uint tmr);
	void set_timeout(uint timeout, Timeout_Func p_fn = 0);
		
	bool is_timeout() { return _is_timeout;}
	void timeout() {
		_is_timeout = true;
	}
	CTimer* next() { return _next;}
	friend CTimerCtrl;

private:
	uint _id =0;
	uint _timeout = 0;
	bool _is_timeout = true;
	Timeout_Func _p_fn = 0;
	CTimer* _next = 0;
};


class CTimerCtrl{
protected:
	static constexpr uint _max = 64;
	uint _count =0;
	Message_mgr<>* p_event_buf = 0;
	CTimer _timers[_max];
	uint _last = 0;
	CTimer* _next_timer =0;
	void insert_timer(CTimer* tm, CTimer* from);

public:
	void init(Message_mgr<>* p_evt)  {
		p_event_buf=p_evt; 

		_timers[_last].set_timeout(0xFFFFFFFF);
		_next_timer = &_timers[_last++];

	}
	template <typename TEvent = Message_mgr<>> void inc();
	bool add_timer(uint timeout, CTimer::Timeout_Func p_fn = 0);
	void set_timer(uint idx, uint timeout, CTimer::Timeout_Func p_fn = 0);

	void timeout(uint idx){
		if(idx >0 && idx < _last){
			_timers[idx].timeout();
		}
	}
	void call_hander(uint idx){
		if(idx >0 && idx <_last ){
			if(_timers[idx]._p_fn) _timers[idx]._p_fn(idx);
		}
	}
};





#endif