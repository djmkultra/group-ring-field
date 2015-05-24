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
#include <exceptionGutz.h>

#include <time.h>

using namespace gutz;

class blah
{
public:
  blah();
};

int failedTests = 0;

/// Test for equality. If the test passes, don't output anything. Only output failures
template<class T>
void testEqual(const T& actual, const T& expected, const std::string& message = "")
{
   if(actual != expected)
   {
      std::cout << "Fail: " << message << " expected: \"" << expected
		<< "\", actual: \"" << actual << "\"" << std::endl;
      failedTests++;
   }
}

int numCounters = 0;
int numChangeCountInherited = 0;
int numChangeCountFunctor = 0;

// Just try and lock a mutex, see if any errors occur
class SimpleTestLock
{
public:
  
  SimpleTestLock() {}

  void test(void)
  {
    Lock lock(_mutex);
  }

  Mutex _mutex;
};

// Store data about the gutz in chickens
class Counter
{
public:
   Counter(void)
   : _count(0) { ++numCounters; }

   ~Counter() { --numCounters; }
   
   void change(int amount)
   {
      // Lock the mutex before changing the value
      {
         Lock lock(_mutex);
         int copy = _count;
         copy += amount;
         _count = copy;
      }
   }

   const int num(void) const { return _count; }

   
private:
   int _count;
   Mutex _mutex;
};

// Derived thread class for testing purposes only.
class ChangeCountInherited : public gutz::Thread
{
public:
   typedef SmartPtr<ChangeCountInherited> pointer;
   ChangeCountInherited(Counter* counter, int amount) 
   : _counter(counter),
     _amount(amount),
   _runEntered(false)
   {
      ++numChangeCountInherited;
   }
   
   virtual ~ChangeCountInherited()
   {
      --numChangeCountInherited;
   }

   const bool runEntered(void) const
   {
      return _runEntered;
   }
   
protected:
   // Loop till done. This is the thread entry point
   virtual void run(void)
   {
      _runEntered = true;
      for(int i = 0; i < 5000; i++)
      {
         _counter->change(_amount);
      }
   }

private:
   Counter* _counter;
   int _amount;
   bool _runEntered;
};

static int _functorEntered = 0;
class ChangeCountFunctor : public gutz::Counted
{
public:
   typedef SmartPtr<ChangeCountFunctor> pointer;
   typedef std::list<pointer> container;
   typedef std::list<pointer>::iterator iterator;
   
   ChangeCountFunctor(Counter* counter, int amount)
   : _counter(counter),
     _amount(amount)
   {
      ++numChangeCountFunctor;
   }
   
   ChangeCountFunctor(const ChangeCountFunctor& c)
   : _counter(c._counter),
   _amount(c._amount)
   {
      ++numChangeCountFunctor;
   }
   
   virtual ~ChangeCountFunctor()
   {
      --numChangeCountFunctor;
   }
   
   // Loop till done. This is the thread entry point
   void operator()()
   {
     {
       Lock lock(_mutex);
       _functorEntered++;
     }

      for(int i = 0; i < 5000; i++)
      {
         _counter->change(_amount);
      }
   }
   
private:
   Counter* _counter;
   int _amount;
   Mutex _mutex;
};

// Test creating a thread from an object that inherits
// gutz::Thread and implements the run() method
void testThreadCreateRun()
{
   Counter counter;
   int runEntered = 0;
   const int numThreads = 100;

   try
   {
      Thread::container pool;
      Thread::iterator thread;
      clock_t start;
      clock_t end;
      
      start = clock();
      
      for(int i = 0; i < numThreads; ++i)
      {
         if( i % 2 == 0)
         {
            pool.push_back(new ChangeCountInherited(&counter, 1));
         }
         else
         {
            pool.push_back(new ChangeCountInherited(&counter, -1));
         }

      }
      end = clock();
//      std::cout << "Allocated " << numThreads << " threads in: " << static_cast<float>(end - start) / CLOCKS_PER_SEC << "s" << std::endl;
         
      start = clock();
      for(thread = pool.begin(); thread != pool.end(); ++thread)
      {
         (*thread)->start();
      }
      end = clock();

//      std::cout << "Started threads in: " << static_cast<float>(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

      // Wait for threads to end
      for(thread = pool.begin(); thread != pool.end(); ++thread)
      {
         // Cast a Thread into ChangeCount. NULL if the cast fails
         ChangeCountInherited::pointer cth = gutz::down_cast<ChangeCountInherited>(*thread);
         if(cth)
         {
            cth->join();
         }
         
         if(cth->runEntered())
         {
            runEntered++;
         }
      }

      // Thread resources automatically freed when the pool goes out of scope.
      // If a mutex is still locked when the threads go out of scope, then
      // an exception will be thrown.
   }
   catch(Exception& e)
   {
      std::cerr << e.what() << std::endl;
   }

   testEqual(counter.num(), 0, "testThreadCreateRun");
   testEqual(numChangeCountInherited, 0, "memory leak: too many ChangeCountInherited objects");
   testEqual(runEntered, numThreads, "ChangeCountInherited::run() not entered enough times");
}

// Test creating a thread using a pointer to an object that implements operator()
void testFunctorThreadCreatePointer(void)
{
   Counter counter;
   const int numThreads = 100;
   
   try
   {
      Thread::container pool;
      Thread::iterator thread;
      
      ChangeCountFunctor** changeCounters = new ChangeCountFunctor*[numThreads];
      clock_t start;
      clock_t end;
      
      start = clock();

      for(int i = 0; i < numThreads; ++i)
      {
         int change = -1;
         if( i % 2 == 0)
         {
            change = 1;
         }

         changeCounters[i] = new ChangeCountFunctor(&counter, change);
         pool.push_back(new gutz::Thread(changeCounters[i]));
      }

      end = clock();
//      std::cout << "Allocated " << numThreads << " threads in: " << static_cast<float>(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

      
      // Wait for threads to end
      for(thread = pool.begin(); thread != pool.end(); ++thread)
      {
         (*thread)->join();
      }
      
      for(int i = 0; i < numThreads; i++)
      {
         delete changeCounters[i];
      }
      
      delete changeCounters;
      
      // Thread resources automatically freed when the pool goes out of scope.
      // If a mutex is still locked when the threads go out of scope, then
      // an exception will be thrown.
   }
   catch(Exception& e)
   {
      std::cerr << e.what() << std::endl;
   }
   
   
   testEqual(counter.num(), 0, "testFunctorThreadCreatePointer");
   testEqual(numChangeCountFunctor,   0, "memory leak: too many ChangeCountFunctor objects");
   testEqual(_functorEntered, numThreads, "ChangeCountFunctor::operator()() not entered enough times");

   numChangeCountFunctor = 0;
   _functorEntered = 0;
}

// Test creating a thread using an object that implements operator()
void testFunctorThreadCreate(void)
{
   Counter counter;
   const int numThreads = 10;
   _functorEntered = 0;
   try
   {
      Thread::container pool;
      Thread::iterator thread;

      clock_t start;
      clock_t end;
      
      start = clock();
      
      for(int i = 0; i < numThreads; ++i)
      {
         int change = -1;
         if( i % 2 == 0)
         {
            change = 1;
         }
         
         // Note - functor goes out of scope, but the thread still needs it!
         // Sounds scary, but the thread object makes a copy onto the heap.
         ChangeCountFunctor ccf(&counter, change);
         pool.push_back(new gutz::Thread(ccf));
      }
      
      end = clock();
      //      std::cout << "Allocated " << numThreads << " threads in: " << static_cast<float>(end - start) / CLOCKS_PER_SEC << "s" << std::endl;
      

      // Wait for threads to end
      for(thread = pool.begin(); thread != pool.end(); ++thread)
      {
         (*thread)->join();
      }
      
      // Thread resources automatically freed when the pool goes out of scope.
      // If a mutex is still locked when the threads go out of scope, then
      // an exception will be thrown.
   }
   catch(Exception& e)
   {
      std::cerr << e.what() << std::endl;
   }
   
   
   testEqual(counter.num(), 0, "testFunctorThreadCreate");
   testEqual(numChangeCountFunctor,   0, "testFunctorThreadCreate: memory leak: too many ChangeCountFunctor objects");
   testEqual(_functorEntered, numThreads, "ChangeCountFunctor::operator()() not entered enough times");
   numChangeCountFunctor = 0;
   _functorEntered = 0;
}

// Make sure that locking a mutex doesn't cause an exception
void simpleTestLock(void)
{
  bool passed = true;
  try
    {
      SimpleTestLock test;
      test.test();
    }
  catch(Exception& e)
    {
      passed = false;
      std::cerr << "Exception caught: " << e.what() << std::endl;
      failedTests++;
    }

  testEqual(passed, true, "simpleTestLock: Using Lock to lock a mutex causes an exception to be thrown.");
}

int main(void)
{
  simpleTestLock();
   testFunctorThreadCreate();
   testThreadCreateRun();
   testFunctorThreadCreatePointer();

   if(failedTests > 0)
   {
      std::cout << "Failed test: " << failedTests << std::endl;
      return 1;
   }

   return 0;
}

