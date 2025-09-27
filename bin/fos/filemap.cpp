#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <string>

using uint = unsigned int;
using std::pair;
using std::string;
using std::vector;

int main()
{

	uint s_file, s_map, s_free, s_total;

	vector<pair<string, uint>> vec;

	std::cout << "Raw space(kb):";
	std::cin >> s_total;

	s_file = std::ceil( float(s_total -1) * 32 / 1060 );
	s_map =  std::ceil( float(s_total -1) * 4 / 1060 );
	s_free = s_total - s_file - s_map - 1;

	vec.push_back({"MBR", 1});
	vec.push_back({"REC", s_file});
	vec.push_back({"MAP", s_map});
	vec.push_back({"DAT", s_free});

	/*
	std::cout << "MBR space:1k\n" 
			  << "File record space:" << s_file <<"k, max item:" << s_file * 1024 / 32 <<"\n" 
			  << "File map space:" << s_map << "k, max item:" << s_map * 1024 / 4 << "\n"
			  << "Free space:" << s_free << "k\n";
	*/

	for(const auto& it : vec)
	{
		std::cout << it.first << ":" <<it.second << "k\n";
	}

}



/*

x*32/1024 + x*4 / 1024 +1+x = s


x(1060/1024) +1 = s
x = (s - 1)*1024/1060


32(s-1)/1060



*/