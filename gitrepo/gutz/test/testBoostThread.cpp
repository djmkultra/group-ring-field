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

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/exceptions.hpp>

#include <smartptr.h>
#include <iostream>
#include <exceptionGutz.h>

using gutz::SmartPtr;
using gutz::Exception;

//
// Increments a variable by the requested amount.
//
// Only one Counter class is instantiated and that
// instance is shared between all threads
// 
class Counter
{
public:
   Counter(void)
   : _count(0) {}
   
   // Changes the internal int by the specified amount
   // in a thread safe manner
   void change(int amount)
   {
      try 
      {
         // When the lock goes out of scope, it automatically unlocks.
         // One benefit is exception safe locking - if an exception occurs after
         // the lock is obtained, the lock goes out of scope, the mutex unlocks
         // and a deadlock doesn't occur. Nice.
         boost::mutex::scoped_lock lock(_mutex);
         int copy = _count;
         copy += amount;
         _count = copy;
      }
      catch (std::exception& e)
      {
         std::cout << "change: " << e.what() << std::endl;
      }
   }      
   
   const int num(void) const { return _count; }
   
private:
   int _count;
   boost::mutex _mutex;
};

//
// Thread entry point
//
class ChangeCount
{
public:

   ChangeCount(Counter* counter, int amount) : _counter(counter), _amount(amount) 
   {
      std::cout << "Brand new ChangeCount" << std::endl;
   }
   virtual ~ChangeCount() { std::cout << "ChangeCount destructor" << std::endl; }
   
   
   // Loop till done. This is the thread entry point
   void operator()()
   {
      for(int i = 0; i < 50; i++)
      {
         _counter->change(_amount);
      }
   }
   
   ChangeCount(const ChangeCount& cc)
   : _counter(cc._counter),
   _amount(cc._amount) {
      std::cout << "ChangeCount copy constructor" << std::endl;
   }
   
private:
   Counter* _counter;
   int _amount;
};

int main(int, char**)
{
   Counter counter;
   boost::thread_group pool;

   const int numThreads = 10;

//   ChangeCount** changeCount;
//   changeCount = new ChangeCount*[numThreads];
   boost::thread** thread = new boost::thread*[numThreads];
   
   for(int i = 0; i < numThreads; ++i)
   {
      int change;
      if( i % 2 == 0)
      {
         change = 1;
      }
      else
      {
         change = -1;
      }
      
      std::cout << "Creating a ChangeCount" << std::endl;
      thread[i] = new boost::thread(ChangeCount(&counter, change));
      std::cout << "Done creating a ChangeCount" << std::endl;
   }

#if 0
   try
   {
      
      clock_t start;
      clock_t end;
      
      start = clock();
      for(int i = 0; i < numThreads; ++i)
      {
         int change;
         if( i % 2 == 0)
         {
            change = 1;
         }
         else
         {
            change = -1;
         }

         // ChangeCount regular constructor called once, copy constructor called 4 times,
         // destructor called 4 times right here. The remaining ChangeCount object
         // is destroyed when the pool goes out of scope.
         pool.create_thread(ChangeCount(&counter, change));
//         pool.add_thread(changeCount[i]);
      }
      
      end = clock();
      std::cout << "Allocated " << numThreads << " threads in: " << static_cast<float>(end - start) / CLOCKS_PER_SEC << "s" << std::endl;
      
      // Thread resources automatically freed when the pool goes out of scope.
      // If a mutex is still locked when the threads go out of scope, then
      // an exception will be thrown.
   }
   catch(Exception& e)
   {
      std::cerr << "yo: " << e.what() << std::endl;
      return 1;
   }

   // Wait for threads to end
   pool.join_all();
#endif
   
   std::cout << "num: " << counter.num() << std::endl;
   
   return 0;
}
