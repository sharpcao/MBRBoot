#include <iostream>

void foo()
{
	std::cout << "foo";
}

int main()
{
	std::cout <<std::hex << (int)&foo << std::endl;
}