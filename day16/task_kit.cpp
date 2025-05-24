#include "inc\task_kit.h"
#include "inc\os_io.h"

Task_mgr::Task_mgr()
{

	GDTIDT gdtidt;
	for(uint i = 0; i < max_tasks; ++i)
	{
		_tasks[i].flag = Task_flag::unused;
		_tasks[i].sel = (task_gdt_start + i) * 8;
		gdtidt.add_gdt_task(task_gdt_start + i, &_tasks[i].tss);
	}		
	Task* p_task = add_task();

	set_active(p_task);
	io_load_tr(p_task->sel);

}

Task* Task_mgr::add_task(Task_func task_func, uint esp_addr)
{
	for(uint i = 0; i < max_tasks;++i)
	{
		if(_tasks[i].flag == Task_flag::unused)
		{
			Task& t = _tasks[i];
			t.flag = Task_flag::used;
			t.tss.eflags = 0x00000202;
			t.tss.eax = 0;
			t.tss.ecx = 0;
			t.tss.edx = 0;
			t.tss.ebx = 0;
			t.tss.ebp = 0;
			t.tss.esi = 0;
			t.tss.edi = 0;
			t.tss.cs = 1*8;
			t.tss.ds = 2*8;
			t.tss.es = 2*8;
			t.tss.fs = 2*8;
			t.tss.gs = 2*8;
			t.tss.ss = 3*8;
			t.tss.ldtr = 0;
			t.tss.iomap = 0x40000000;
			t.tss.esp = esp_addr;
			t.tss.eip = (int)task_func;
			return &t;
		}
	}
	return 0;
}

void Task_mgr::set_active(Task* p_task)
{
	p_task->flag = Task_flag::actived;
	_task_ptrs[_running_end++] = p_task;
}

void Task_mgr::switch_next()
{
	if (_running_end > 1){
		if( ++_cur == _running_end) _cur = 0;
		task_switch(0, _task_ptrs[_cur]->sel);
	}
}



void CMTTimerCtrl::mt_inc()
{
	inc();
	if (_count % 2){
		mt_taskswitch();
	}
}