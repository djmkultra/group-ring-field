///-----------------------------------------------------------------------------------
/// VarArgsTest.cpp
///-----------------------------------------------------------------------------------
#include <iostream>
#include <signalGutz.h>
#include <typeinfo>
#include <smartptr.h>
#include <list>
#include <threadGutz.h>

#ifdef WIN32
#	include <WinSock2.h>
#	include <Windows.h>
#define sleep Sleep
#else
#	include <unistd.h>
#define sleep usleep
#endif

using namespace std;
using namespace gutz;

///----------------------------------------------------------------------
/// Bogus class for baking clams
///----------------------------------------------------------------------
class Clam
{
	
public:
	HAS_SLOTS;
	
   Clam(void) {}

   
	void bake0(void)
	{
		std::cout << "Clam::bake0 called" << std::endl;
	}
   
	void bake1(int b1)
	{
		std::cout << "Clam::bake1 called with (b1)                      = ("
         << b1 
         << ")" << std::endl;
	}

	void bake2(int b1, int b2)
	{
		std::cout << "Clam::bake2 called with (b1,b2)                   = ("
         << b1 << "," << b2
         << ")" << std::endl;
	}

   void bake3(int b1, int b2, int b3)
	{
		std::cout << "Clam::bake3 called with (b1,b2,b3)                = ("
         << b1 << "," << b2 << "," << b3
         << ")" << std::endl;
	}

   void bake4(int b1, int b2, int b3, int b4)
	{
		std::cout << "Clam::bake4 called with (b1,b2,b3,b4)             = ("
         << b1 << "," << b2 << "," << b3 << "," << b4
         << ")" << std::endl;
	}

   void bake5(int b1, int b2, int b3, int b4, int b5)
	{
		std::cout << "Clam::bake5 called with (b1,b2,b3,b4,b5)          = ("
         << b1 << "," << b2 << "," << b3 << "," << b4 << ","
         << b5
         << ")" << std::endl;
   }

   void bake6(int b1, int b2, int b3, int b4, int b5, int b6)
	{
		std::cout << "Clam::bake6 called with (b1,b2,b3,b4,b5,b6)       = ("
         << b1 << "," << b2 << "," << b3 << "," << b4 << ","
         << b5 << "," << b6
      << ")" << std::endl;
   }

   void bake7(int b1, int b2, int b3, int b4, int b5, int b6, int b7)
	{
		std::cout << "Clam::bake7 called with (b1,b2,b3,b4,b5,b6,b7)    = ("
      << b1 << "," << b2 << "," << b3 << "," << b4 << ","
      << b5 << "," << b6 << "," << b7
      << ")" << std::endl;
   }

   void bake8(int b1, int b2, int b3, int b4, int b5, int b6, int b7, int b8)
	{
		std::cout << "Clam::bake8 called with (b1,b2,b3,b4,b5,b6,b7,b8) = ("
      << b1 << "," << b2 << "," << b3 << "," << b4 << ","
      << b5 << "," << b6 << "," << b7 << "," << b8
      << ")" << std::endl;
   }
};

int main(int argc, char** argv)
{
	Clam clam;
	ThreadWorker thread;
	
	cout << "Starting thread" << endl;
	thread.start();
   
   //	ArgHolder* arg_holder_tmp = new _ArgHolder<>();
	ArgHolder::pointer arg_holder = new ArgHolder;
	
   int i = 11;
//   const int sleepLen = 1000000;
   const int sleepLen = 1;
   thread.addCommand(new_command(&clam, &Clam::bake0));
   sleep(sleepLen);
   thread.addCommand(new_command(&clam, &Clam::bake1, i));                            i += 1;
   sleep(sleepLen);
   thread.addCommand(new_command(&clam, &Clam::bake2, i,i+1));                        i += 2;
   sleep(sleepLen);
   thread.addCommand(new_command(&clam, &Clam::bake3, i,i+1,i+2));                    i += 3;
   sleep(sleepLen);
   thread.addCommand(new_command(&clam, &Clam::bake4, i,i+1,i+2,i+3));                i += 4;
   sleep(sleepLen);
   thread.addCommand(new_command(&clam, &Clam::bake5, i,i+1,i+2,i+3,i+4));            i += 5;
   sleep(sleepLen);
   thread.addCommand(new_command(&clam, &Clam::bake6, i,i+1,i+2,i+3,i+4,i+5));        i += 6;
   sleep(sleepLen);
   thread.addCommand(new_command(&clam, &Clam::bake7, i,i+1,i+2,i+3,i+4,i+5,i+6));    i += 7;
   sleep(sleepLen);
   thread.addCommand(new_command(&clam, &Clam::bake8, i,i+1,i+2,i+3,i+4,i+5,i+6,i+7));
   sleep(sleepLen);
	
	thread.die(); // Won't die until command buffer is empty
	thread.join();
    
   int* the_int = new int(8);
   
   arg_value<int*> blah(the_int);
	
   std::cout << "arg_value<int> blah(0) = " << blah.getValue() << std::endl;
	return 0;
}

