#include "inc\functions.h"

Cmd_Parser::Cmd_Parser(const char* cmd) : _cmd(cmd), _args(0)
{
	uint i = 0, j = 0;
	char c = 0;
	bool pre_blank = true;

	while( (c = _cmd[i++]))
	{
		if(_args == N) break; 

		if( pre_blank )
		{
			if( c != ' '){
				_args_pos[_args++] = i - 1;
				pre_blank = false;
			}
		}else{
			if (c == ' ') pre_blank = true;
		}

	}
}

stringbuf<> Cmd_Parser::operator [] (uint ix) const
{
	stringbuf<> out;
	if (ix < _args){
		char c;		
		uint i = _args_pos[ix];
		while ( (c = _cmd[i++]) ){
			if (c == ' ') break;
			out << c;
		}
	}
	return out;

}

stringbuf<> Cmd_History::get_prev()
{
	--_cur;
	if (_cur < 0) {
		_cur = -1;
		return "";
	}else{
		return _data[_cur];	
	}		
}

stringbuf<> Cmd_History::get_next()
{	
	++_cur;
	if(_cur >= _total ) {
		_cur = _total;
		return "";
	}else{
		return _data[_cur];
	}
}

void Cmd_History::add_cmd(const stringbuf<>& scmd)
{
	if (_data.is_full()) {
		_data.reset();
		_cur = 0;
		_total = 0;
	}
	_data.push_back(scmd);
	_cur = ++_total;
}