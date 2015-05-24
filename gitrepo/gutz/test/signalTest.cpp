#include <iostream>
#include <signalGutz.h>

using namespace gutz;

class ClassA {
public:
	HAS_SLOTS;
	
	ClassA(float f)
	: _float(f) {}
	
	void setFloat(float f)
	{
		_float = f;
	}
	
	float getFloat(void)
	{
		return _float;
	}
	
	float _float;
};

class ClassB {
public:
	gutz::Signal<float> changeFloat;
};


bool test1(int i)
{
	
	ClassA a(9.0f);
	ClassB b;
	
	connect(b.changeFloat, &a, &ClassA::setFloat);
	b.changeFloat(20.0f);
	
	if(a.getFloat() == 20.0f)
	{
		std::cout << "Test " << i << " passed" << std::endl;
		return true;
	}
	else
	{
		std::cout << "Test " << i << " failed " << std::endl;
		return false;
	}
}

int main (int argc, char * const argv[])
{

	test1(1);
    return 0;
}


	
	

