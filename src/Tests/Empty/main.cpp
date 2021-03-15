#include <iostream>

template<typename T>
class A
{
	using A = A<T>;

public:
	A(){}
	
};

int main()
{
	A<int>();
	return 0;
}
