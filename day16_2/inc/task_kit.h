#ifndef TASK_KIT_H
#define TASK_KIT_H

#include "gdtidt.h"
#include "timer_kit.h"
#include "message_kit.h"


struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};


enum Task_flag{
	unused,
	used,
	actived
};

struct Task {
	int sel;
	Task_flag flag;
	TSS32 tss;

};


class Task_mgr{
private:
	static constexpr int max_tasks = 1000;
	static constexpr int task_gdt_start = 4;
	Task* _task_ptrs[max_tasks];
	Task  _tasks[max_tasks];
	int _running_end = 0;
	int _cur = 0;
	bool _need_clean = false;
	void _clean_inactive();
public:
	using  Task_func = void (*)(uint param) ;

	Task_mgr();
	Task* add_task(Task_func task_func = 0, uint esp_addr = 0, uint param = 0);
	void set_active(Task* p_task);
	void set_inactive(Task* p_task);

	void switch_next();
	Task* get_task(uint id) {return &_tasks[id]; }
	

};


class CMTTimerCtrl : public CTimerCtrl
{
private:
	using CTimerCtrl::inc;
	Task_mgr* _ptask_mgr = 0;
public:
	void set_task_mgr(Task_mgr* p)  {_ptask_mgr = p;};
	void mt_taskswitch() { if(_ptask_mgr) _ptask_mgr->switch_next();}
	void mt_inc();
	
};

class Task_Message_mgr : public Message_mgr<>
{
private:
	Task* _p_msg_task = 0;
	Task_mgr* _p_task_mgr = 0;

public:
	bool push_message(uint p1, uint p2){
		bool result = Message_mgr<>::push_message(p1,p2);
		act_msg_task();
		return result;
	}
	void set_msg_task(Task* ptask, Task_mgr* ptmgr){
		_p_msg_task = ptask;
		_p_task_mgr = ptmgr;
	}


	void act_msg_task()
	{
		if(_p_msg_task && _p_task_mgr){
			_p_task_mgr->set_active(_p_msg_task);
		}
	}
	void deact_msg_task()
	{
		if(_p_msg_task && _p_task_mgr)
		{
			_p_task_mgr->set_inactive(_p_msg_task);
		}
	}

};


template <typename TEvent>
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
					p->_p_tm_evt->push_message(EVENT::Timer, p->_id);
				}else{
					static_cast<TEvent*>(p_event_buf)->push_message(EVENT::Timer, p->_id);
				}
			}
		}else{
			break;
		}
	}


}

#endif