#ifndef LOCK_KIT
#define LOCK_KIT

#include "os_io.h"

class Disable_Interrupt{
public:
	Disable_Interrupt(){
		_keep_flags = io_load_eflags();
		io_cli();
	}
	~Disable_Interrupt(){
		io_store_eflags(_keep_flags);
	}

private:
	uint _keep_flags;

};



#endif
