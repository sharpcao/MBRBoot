#ifndef TASK_KIT_H
#define TASK_KIT_H

#include "gdtidt.h"
#include "timer_kit.h"


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
public:
	using  Task_func = void (*)() ;

	Task_mgr();
	Task* add_task(Task_func task_func=0, uint esp_addr=0);
	void set_active(Task* p_task);
	void switch_next();


};


class CMTTimerCtrl :public CTimerCtrl
{
private:
	using CTimerCtrl::inc;
	Task_mgr* _ptask_mgr = 0;
public:
	void set_task_mgr(Task_mgr* p)  {_ptask_mgr = p;};
	void mt_taskswitch() { if(_ptask_mgr) _ptask_mgr->switch_next();}
	void mt_inc();
	
};



#endif