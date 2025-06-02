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

Task* Task_mgr::add_task(Task_func task_func, uint esp_addr, uint param)
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
			t.tss.eip = (int)task_func;			
			t.tss.esp = 0;

			if (esp_addr){
				t.tss.esp = esp_addr-8;
				*(uint*)(esp_addr -4) = param;
			}

			return &t;
		}
	}
	return 0;
}

void Task_mgr::set_active(Task* p_task)
{
	if(p_task){
		p_task->flag = Task_flag::actived;
		
		for(uint i =0; i < _running_end; ++i){
			if(_task_ptrs[i] == p_task) return;
		}

		if (_need_clean) _clean_inactive();

		if(_running_end < max_tasks){
			_task_ptrs[_running_end++] = p_task;
		}
	}
}

void Task_mgr::set_inactive(Task* p_task)
{
	if (p_task) {
		p_task->flag = Task_flag::used;
		for(uint i = 0; i < _running_end; ++i)
		{
			if (_task_ptrs[i] == p_task){
				_task_ptrs[i] = 0;
				_need_clean = true;
				break;
			}
		}
	}
}

void Task_mgr::_clean_inactive()
{
	Task** p_start = &_task_ptrs[0], **p_end = &_task_ptrs[_running_end];
	bool need_fix_cur = (_task_ptrs[_cur])? true : false;

	Task** p1 = p_start;

	for(; p1 != p_end; ++p1){
		if (*p1 == 0) break;			
	}

	if (p1 != p_end){

		Task** p2 = p1 + 1;
		for(;p2 != p_end; ++p2)
		{
			if(*p2 != 0) break;
		}

		while(p2 != p_end){
			if(need_fix_cur && (p2 == &_task_ptrs[_cur])) {
				_cur = p1 - p_start;
				need_fix_cur = false;
			}
			*p1++ = *p2++;
			while((*p2 == 0) && p2 !=p_end) ++p2;
		}
		
	}
	_running_end = p1 - p_start;
	_need_clean = false;
}

void Task_mgr::switch_next()
{

begin:
	bool need_switch = false;
	if (_running_end > 1){
		
		for(uint next = _cur + 1; ;++next){
			if (next >= _running_end) {
				next = 0;

				// if(_need_clean){
				// 	_clean_inactive();
				// 	goto begin;
				// }

			}

			if( next == _cur) break;

			if( _task_ptrs[next] ){			
				_cur = next;
				need_switch = true;			
				break;
			}
		}

	}

	if(need_switch){
		task_switch(0, _task_ptrs[_cur]->sel);
	}

}



void CMTTimerCtrl::mt_inc()
{
	inc<Task_Message_mgr>();
	if (_count % 2){
		mt_taskswitch();
	}
}

