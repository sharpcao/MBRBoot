#include "inc\functions.h"




void console_dump(stringbuf<>& cout_str, const Cmd_Parser& cmd)
{
	
	if (cmd.size() != 2) {
		cout_str <<"dump [address]\n";
		return;
	}

	stringbuf<> saddr = cmd[1];
	uint addr = 0;
	for(uint i = 0; i< saddr.size();++i){
		char c = saddr[0];
		if (c >= '0' && c <= '9'){
			addr =  addr *16 +  (c-'0');
		}else{
			return;
		}
	}
	cout_str << stringbuf<>::flag::hex;

	for(uint i = 0; i < 3; ++i){
		uchar c = ((uchar*) addr)[i];
		cout_str << (uint)c <<" ";
	}


}