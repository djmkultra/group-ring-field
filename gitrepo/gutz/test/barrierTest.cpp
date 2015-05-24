///////////////////////////////////////////////////////////////////////////
//              _____________  ______________________    ^    ----  _
//             /  ________  |  |   ___   ________   /   / \  /    \ |
//            |  |       |  |_ |  |_ |  |       /  /   /   \|       |
//            |  |  ___  |  || |  || |  |      /  /   / --- \   --- |
//            |  | |   \ |  || |  || |  |     /  /   /       \____/ |_____|
//            |  | |_@  ||  || |  || |  |    /  /          
//            |  |___/  ||  ||_|  || |  |   /  /_____________________
//             \_______/  \______/ | |__|  /___________________________
//                        |  |__|  |
//                         \______/
//                 University of New Mexico       
//                           2010
///////////////////////////////////////////////////////////////////////////


#include <threadGutz.h>
#include <iostream>

using gutz::Barrier;
using gutz::Thread;

static const int THREADS = 100;

// Barrier variable
Barrier::pointer barrier;

// Counters
int counters[THREADS];
int finalCounters[THREADS];

class Calculation
{
public:
   Calculation(int index)
   : _index(index) 
   {
      counters[index] = 0;
   }

   // Thread entry point
   void operator()()
   {
      // Increment the counter associated with this thread
      ++counters[_index];
      
      // Synchronization point
      barrier->wait();

      // Once all the threads are done incrementing their counters,
      // sum them all up
      int count = 0;
      for(int i = 0; i < THREADS; ++i)
      {
         count += counters[i];
      }

      // What is the final tally? All of the counters should be the same.
      // Without the barrier, the counters will all be different
      finalCounters[_index] = count;
   }
   
private:
   int _index;
};

static int failedTests = 0;

/// Test for equality. If the test passes, don't output anything. Only output failures
template<class T>
void testEqual(const T& actual, const T& expected, const std::string& message = "")
{
   if(actual != expected)
   {
      std::cout << "Fail: " << message << " expected: \"" << expected << "\", actual: \"" << actual << "\"" << std::endl;
      failedTests++;
   }
}

int main(int argc, char **argv)
{
   // Create a barrier. 
   barrier = new Barrier(THREADS);
   
   Thread::container pool;
   Thread::iterator  thread;
   
   
   for(int i = 0; i < THREADS; ++i)
   {
      Calculation calc(i);
      pool.push_back(new Thread(calc));
   }
   
   for(thread = pool.begin(); thread != pool.end(); ++thread)
   {
      (*thread)->join();
   }
   
   for(int i = 0; i < THREADS; ++i)
   {
      testEqual(finalCounters[i], THREADS, "Wrong final counter");
   }
   
   if(failedTests > 0)
   {
      std::cout << "Failed test: " << failedTests << std::endl;
      return 1;
   }
   
   return 0;
}
