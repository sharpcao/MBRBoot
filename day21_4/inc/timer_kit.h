#ifndef TIMER_KIT_H
#define TIMER_KIT_H


#include "common.h"
#include "Message_kit.h"
#include "os_io.h"
#include "lock_kit.h"

class CTimerCtrl;

class CTimer{
public:
	friend CTimerCtrl;
	using Timeout_Func = void(*)(uint tmr);

	void set_timer(uint timeout, Timeout_Func p_fn = 0);
		
	bool is_timeout()	{ return _is_timeout;}
	void timeout() 		{ _is_timeout = true;}
	CTimer* next() 		{ return _next;	}
	void set_tm_event(Message_mgr<>* pmsgr) { _p_tm_evt = pmsgr; }
	
private:
	uint _id = 0;
	uint _timeout = 0;
	bool _is_timeout = true;
	Timeout_Func _p_fn = 0;
	CTimer* _next = 0;
	Message_mgr<>* _p_tm_evt = 0;
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
	void init_timers(Message_mgr<>* p_evt)  {
		p_event_buf=p_evt; 

		_timers[_last].set_timer(0xFFFFFFFF);
		_next_timer = &_timers[_last++];

	}

	template <typename T1 = decltype(p_event_buf), typename T2 = decltype(CTimer::_p_tm_evt) > 
	void inc();

	bool add_timer(uint timeout, CTimer::Timeout_Func p_fn = 0, Message_mgr<>* p_evt = 0);
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

template <typename T1,typename T2>
void CTimerCtrl::inc() 
{
	if(++_count <_next_timer->_timeout) return;

	for(CTimer* p = _next_timer; p->_next!=0; p=p->_next)
	{
		if( _count >= p->_timeout  ){
			if (!p->is_timeout()) {
				_next_timer = p->next();
				p->timeout();
				if(p->_p_tm_evt){
					static_cast<T2>(p->_p_tm_evt)->push_message(EVENT::Timer, p->_id);
				}else{
					static_cast<T1>(p_event_buf)->push_message(EVENT::Timer, p->_id);
				}
			}
		}else
		{
			break;
		}
	}


}




#endif