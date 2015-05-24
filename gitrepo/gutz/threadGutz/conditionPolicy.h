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

#ifndef _WAIT_CONDITION_POLICY_H
#define _WAIT_CONDITION_POLICY_H

namespace policy
{
   /////////////////////////////////////////////////////////////////////////////////////
   /// Wait for a condition to be met. Provides a way of synchronizing threads.
   /// Example:
   /// \code
   /// #include <threadGutz.h>
   /// using gutz::Condition;
   /// using gutz::Mutex;
   /// using gutz::Thread;
   /// using gutz::Lock;
   /// 
   /// Condition waitSignal;
   /// Mutex mutex;
   /// int count = 0;
   /// 
   /// // Number of threads to spawn
   /// static const int numThreads = 50;
   /// 
   /// // Number of times a thread should increment the counter
   /// static const int numIncrements = 5;
   /// 
   /// // A thread that sits around waiting for a signal from another
   /// // thread. When it gets that signal, it wakes up and increments
   /// // a counter.
   /// class WaitForSignal
   /// {
   /// public:
   ///    WaitForSignal() : _die(false) {}
   ///    
   ///    void operator()()
   ///    {
   ///       for(int i = 0; i < 5; ++i)
   ///       {
   ///          Lock lock(mutex);
   ///          waitSignal.wait(&mutex);
   ///          int intermediate = count;
   ///          ++intermediate;
   ///          count = intermediate;
   ///       }
   ///    }
   ///    
   ///    void die(void) { _die = true; }
   ///    
   /// private:
   ///    bool _die;
   /// };
   /// 
   /// // Sends a signal to all of the threads to wake up and 
   /// // do their thing
   /// class SendSignal
   /// {
   /// public:
   ///    SendSignal() : _die(false) {}
   ///    
   ///   void operator()(void)
   ///    {
   ///       for(int i = 0; i < numThreads; ++i)
   ///       {
   ///          sleep(1);
   ///          waitSignal.wakeAll();
   ///       }
   ///    }
   ///    
   ///    void die(void) { _die = true; }
   ///    
   /// private:
   ///    bool _die;
   ///    
   /// };
   /// 
   /// static int failedTests = 0;
   /// 
   /// // Test for equality. If the test passes, don't output anything. Only output failures
   /// template<class T>
   /// void testEqual(const T& actual, const T& expected, const std::string& message = "")
   /// {
   ///   if(actual != expected)
   ///    {
   ///       std::cout << "Fail: " << message << " expected: \"" << expected << "\", actual: \"" << actual << "\"" << std::endl;
   ///       failedTests++;
   ///    }
   /// }
   /// 
   /// int main(void)
   /// {
   ///    WaitForSignal ws;
   ///    SendSignal ss;
   ///    Thread::container pool;
   ///    Thread::iterator  thread;
   ///    
   ///    for(int i = 0; i < numThreads; ++i)
   ///    {
   ///       pool.push_back(new Thread(ws));
   ///    }
   ///    
   ///    pool.push_back(new Thread(ss));
   ///    
   ///    for(thread = pool.begin(); thread != pool.end(); ++thread)
   ///    {
   ///       (*thread)->join();
   ///    }
   ///    
   ///    testEqual(count, numThreads * numIncrements, "conditionTest failed");
   ///    
   ///    if(failedTests > 0)
   ///    {
   ///       std::cout << "Failed test: " << failedTests << std::endl;
   ///       return 1;
   ///    }
   ///    
   ///    return 0;
   /// }
   /// \endcode
   template<typename impl>
   class Condition : public impl
   {
   public:
      //////////////////////////////////////////////////////////////////////////////////
      /// Wait for condition to be true
      void wait(gutz::Mutex* mutex)
      {
         impl::wait(mutex);
      }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Wait for condition to be true
      void wait(gutz::Lock& lock)
      {
         impl::wait(lock.mutexPtr());
      }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Wake all threads waiting on this condition
      void wakeAll(void)
      {
         impl::wakeAll();
      }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Wake a single thread waiting on this condition
      void wakeOne(void)
      {
         impl::wakeOne();
      }
   };
}

#endif
