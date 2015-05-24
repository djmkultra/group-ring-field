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


#ifndef _THREAD_H
#define _THREAD_H

#ifdef WIN32
#  define USE_QTHREAD
#endif

//#define USE_QTHREAD

// Which thread implementation to use? Depends on your platform.
#ifdef USE_NOTHREAD
#  include <nothreadGutz.h>
#else
#  ifdef USE_QTHREAD
#    include <qthreadGutz.h>
#  else
#    include <pthreadGutz.h>  // Put Gutz on the end of the filename to avoid conflicts with pthread.h
#  endif
#endif

#include <lock.h>

///////////////////////////////////////////////////////////////////////////
// Define Thread class based on policy
///////////////////////////////////////////////////////////////////////////
#include <threadPolicy.h>
namespace gutz
{
   typedef policy::Thread Thread;
   
}   

//#define TEST_COOL_LOCKING_STUFF

///////////////////////////////////////////////////////////////////////////
// Define Mutex class based on policy
///////////////////////////////////////////////////////////////////////////
#include <mutexPolicy.h>
namespace gutz
{
   typedef policy::Mutex<impl::Mutex> Mutex;

   class Lock
   {
   public:
      Lock(gutz::Mutex& lockable)
      : _lockable(lockable)
      {
         _lockable.lock();
#ifdef TEST_COOL_LOCKING_STUFF
         std::cout << "Locked" << std::endl;
#endif
      }
      
      virtual ~Lock()
      {
         _lockable.unlock();
#ifdef TEST_COOL_LOCKING_STUFF
         std::cout << "Unlocked" << std::endl;
#endif
      }
      
      gutz::Mutex* mutexPtr(void) { return &_lockable; }
      
   private:
      gutz::Mutex& _lockable;
      
      Lock(const Lock&);
      Lock& operator=(const Lock&);
   };

}

///////////////////////////////////////////////////////////////////////////
// Define WaitCondition class based on policy
//
// The WaitCondition depends on a Mutex type being defined. This is 
// why there are separate ifdef sections for each class. Ugly, but
// it allows for a templatized class to refer to another concrete 
// policy based class.
///////////////////////////////////////////////////////////////////////////
#include <conditionPolicy.h>
namespace gutz
{
   typedef policy::Condition<impl::Condition> Condition;
}
   

namespace gutz
{
   ///////////////////////////////////////////////////////////////////////////
   /// Provides a barrier for thread synchronization
   ///
   /// Example code:
   /// \code
   /// #include <threadGutz.h>
   /// #include <iostream>
   ///    
   ///    using gutz::Barrier;
   ///    using gutz::Thread;
   ///    
   ///    static const int THREADS = 100;
   ///    
   ///    // Barrier variable
   ///    Barrier::pointer barrier;
   ///    
   ///    // Counters
   ///    int counters[THREADS];
   ///    int finalCounters[THREADS];
   ///    
   ///    class Calculation
   ///    {
   ///    public:
   ///       Calculation(int index)
   ///       : _index(index) 
   ///       {
   ///          counters[index] = 0;
   ///       }
   ///       
   ///       // Thread entry point
   ///       void operator()()
   ///       {
   ///          // Increment the counter associated with this thread
   ///          ++counters[_index];
   ///          
   ///          // Synchronization point
   ///          barrier->wait();
   ///          
   ///          // Once all the threads are done incrementing their counters,
   ///          // sum them all up
   ///          int count = 0;
   ///          for(int i = 0; i < THREADS; ++i)
   ///          {
   ///             count += counters[i];
   ///          }
   ///          
   ///          // What is the final tally? All of the counters should be the same.
   ///          // Without the barrier, the counters will all be different
   ///          finalCounters[_index] = count;
   ///       }
   ///       
   ///    private:
   ///       int _index;
   ///    };
   ///    
   ///    static int failedTests = 0;
   ///    
   ///    // Test for equality. If the test passes, don't output anything. Only output failures
   ///    template<class T>
   ///    void testEqual(const T& actual, const T& expected, const std::string& message = "")
   ///    {
   ///       if(actual != expected)
   ///       {
   ///          std::cout << "Fail: " << message << " expected: \"" << expected << "\", actual: \"" << actual << "\"" << std::endl;
   ///          failedTests++;
   ///       }
   ///    }
   ///    
   ///    int main(int argc, char **argv)
   ///    {
   ///       // Create a barrier. 
   ///       barrier = new Barrier(THREADS);
   ///       
   ///       Thread::container pool;
   ///       Thread::iterator  thread;
   ///       
   ///       
   ///       for(int i = 0; i < THREADS; ++i)
   ///       {
   ///          Calculation calc(i);
   ///          pool.push_back(new Thread(calc));
   ///       }
   ///       
   ///       for(thread = pool.begin(); thread != pool.end(); ++thread)
   ///       {
   ///          (*thread)->join();
   ///       }
   ///       
   ///       for(int i = 0; i < THREADS; ++i)
   ///       {
   ///          testEqual(finalCounters[i], THREADS, "Wrong final counter");
   ///       }
   ///       
   ///       if(failedTests > 0)
   ///       {
   ///          std::cout << "Failed test: " << failedTests << std::endl;
   ///          return 1;
   ///       }
   ///   
   ///       return 0;
   ///    }
   ///    
   /// \endcode
   class Barrier : public gutz::Counted
   {
   public:
      typedef gutz::SmartPtr<Barrier>      pointer;
      typedef std::list<pointer>           container;
      typedef std::list<pointer>::iterator iterator;
      
      ///////////////////////////////////////////////////////////////////////////
      /// Constructor
      ///
      /// @param count number of threads to wait for before the barrier is lifted
      Barrier(unsigned int count)
      : _threshold(count),
        _count(count)
      {
         if(count == 0)
         {
            throw gutz::Exception("Barrier count cannot be zero");
         }
      }
      
      ///////////////////////////////////////////////////////////////////////////
      /// Wait point for a barrier
      ///
      /// returns true if the caller is the one who caused the condition to be
      /// met, false otherwise
      bool wait(void)
      {
         Lock lock(_mutex); // Mutex unlocks when lock goes out of scope

         // If all threads have reached the barrier
         if(--_count == 0)
         {
            // Reset the barrier
            _count = _threshold;
            // Signal all threads to stop waiting
            _barrier.wakeAll();
            return true;
         }

         // Wait here until all threads have reached the barrier condition
         _barrier.wait(lock);
         
         return false;
      }
      
   private:
      Mutex        _mutex;      // Lock for the barrier counter and condition
      Condition    _barrier;    // Barrier condition - wait on this condition before proceeding
      unsigned int _threshold;  // Store original barrier count
      unsigned int _count;      // Number of threads still waiting
   };
   
   
}

#endif
