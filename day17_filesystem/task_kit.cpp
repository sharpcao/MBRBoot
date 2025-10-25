#include "inc\task_kit.h"
#include "inc\os_io.h"
#include "inc\functions.h"

void memset(char* const p_mem, uint size, char v)
{
	for(char* p = p_mem; p < p_mem + size; ++p) *p = v;
}

Task_mgr::Task_mgr()
{

	GDTIDT gdtidt;
	for(uint i = 0; i < max_tasks; ++i)
	{
		_tasks[i].flag = Task_flag::unused;
		_tasks[i].sel = (task_gdt_start + i) * 8;
		gdtidt.add_gdt_task(task_gdt_start + i, &_tasks[i].tss);
	}		

	TaskItem* p_task = add_task();
	_cur_ptask = p_task;
	set_active(p_task, PT::high, LV::level_4);
	io_load_tr(p_task->sel);


}

TaskItem* Task_mgr::add_task(Task_func task_func, uint esp_addr, uint param)
{
	for(uint i = 0; i < max_tasks; ++i)
	{
		if(_tasks[i].flag == Task_flag::unused)
		{
			TaskItem& t = _tasks[i];
			t.flag = Task_flag::used;
			t.priority = PT::normal;
			t.level = LV::level_2;
			memset((char*)&t.tss, sizeof(t.tss), (char)0);
			t.tss.eflags = 0x00000202; // IF = 1
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
			t.tss.iomap = sizeof(t.tss);
			t.tss.eip = (int)task_func;			
			t.tss.esp = 0x7c00;

			if (esp_addr){
				t.tss.esp = esp_addr-8;
				*(uint*)(esp_addr -4) = param;
			}

			return &t;
		}
	}
	return 0;
}

void Task_mgr::_insert_active(TaskItem* p_task)
{
	if(_running_end >= max_tasks) return;

	p_task->flag = Task_flag::actived;

	uint i = 0, lv = p_task->level;

	for(; i < _running_end; ++i){
		if( lv > _task_ptrs[i]->level) break;
	}

	if( i == _running_end ) {
		_task_ptrs[_running_end++] = p_task;
	}else{
		for(uint j = _running_end++; j > i; --j){
			_task_ptrs[j] = _task_ptrs[j-1];
		}
		_task_ptrs[i] = p_task;
	}

	if(_cur >=i && _cur < _running_end -1 ) ++_cur;
	
}

void Task_mgr::_reorder(TaskItem* p_task, Reorder direction)
{
	TaskItem** begin = &_task_ptrs[0];
	TaskItem** end = &_task_ptrs[_running_end];
	TaskItem** pos = begin;

	for(; pos < end; ++pos){
		if ( *pos == p_task) break;
	}
	if (pos == end) return;

	auto lv = p_task->level;

	if(direction == Reorder::down) 
	{
		TaskItem** apos = pos + 1;
		for(; apos < end; ++apos)
		{
			if ( lv < (*apos)->level ) {
				*(apos-1) = *apos;
			}else{
				break;
			}
		}
		*(apos -1) = p_task;

	}else if( direction == Reorder::up)
	{
		TaskItem** apos = pos -1;
		for(; apos >= begin; --apos)
		{
			if( lv > (*apos)->level ) {
				*(apos+1) = *apos;
			}else{
				break;
			}
		}
		*(apos + 1) = p_task;
	}

	// fix _cur index
	for(uint i = 0; i< _running_end; ++i){
		if(_task_ptrs[i] == _cur_ptask)
		{
			_cur = i;
			break;
		}
	}
	
}



void Task_mgr::set_active(TaskItem* p_task, uint new_priority, uint new_level)
{
	

	if( p_task == 0) return;

	if (new_priority) p_task->priority = new_priority;

	Reorder reorder = Reorder::no;
	if (new_level && new_level != p_task->level) 
	{
		reorder = (new_level > p_task->level)? Reorder::up : Reorder::down;
		p_task->level = new_level;
	}


	if (p_task->flag == Task_flag::actived)
	{
		if(reorder != Reorder::no)	_reorder(p_task, reorder);
		
	}else{
		_insert_active(p_task);
	}
	
	
}

void Task_mgr::set_inactive(TaskItem* p_task)
{
	if (p_task == 0)  return;

	p_task->flag = Task_flag::used;
	for(uint i = 0; i < _running_end; ++i)
	{
		if (_task_ptrs[i] == p_task)
		{

			for( uint j = i+1; j < _running_end; ++j){
				_task_ptrs[j-1] = _task_ptrs[j];
			}
			--_running_end;
			if ( _cur >= i && _cur > 0) --_cur;

			break;

		}
	}
}

/*
void Task_mgr::_clean_inactive()
{
	TaskItem** p_start = &_task_ptrs[0], **p_end = &_task_ptrs[_running_end];
	bool need_fix_cur = (_task_ptrs[_cur])? true : false;

	TaskItem** p1 = p_start;

	for(; p1 != p_end; ++p1){
		if (*p1 == 0) break;			
	}

	if (p1 != p_end){

		TaskItem** p2 = p1 + 1;
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
*/
void Task_mgr::switch_to(uint idx)
{
	task_switch(0, _tasks[idx].sel);
}
void Task_mgr::switch_next()
{

	bool need_switch = false;



	if (_running_end > 1){

		// if(_cur_ptask->flag == Task_flag::actived){
		// 	for(_cur = 0; _cur < _running_end; ++_cur) {
		// 		if(_task_ptrs[_cur] == _cur_ptask) break;
		// 	}
		// }

		for(uint next = _cur + 1; ; ++next){

			if ( next >= _running_end ) next = 0;
check_begin:
			if( _task_ptrs[next] == _cur_ptask ){
				_cur = next;
			 	break;
			}
			if( _task_ptrs[next] ){		

				//fix this problem
				// if( _task_ptrs[_cur] && _task_ptrs[next]->level < _task_ptrs[_cur]->level ) {
				// 	next = 0;
				// 	goto check_begin;
				// }

				_cur = next;
				need_switch = true;			
				break;
			}
		}

	}

	if(need_switch){
		_cur_ptask = _task_ptrs[_cur];

		task_switch(0, _cur_ptask->sel);
	}

}



void CMTTimerCtrl::mt_inc()
{
	static uint cnt = 0;
	uint p =  _ptask_mgr->get_cur_priority();
	if (p==0) p=1;

	inc<Task_Message_mgr*,Task_Message_mgr* >();
	if ((++cnt % p) == 0){
		cnt = 0;
		//mt_taskswitch();
	}

	
}

