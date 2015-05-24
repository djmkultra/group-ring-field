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

#ifndef _THREAD_POLICY_H
#define _THREAD_POLICY_H

#ifdef WIN32
#	include <WinSock2.h>
#	include <windows.h>
#endif

#include <smartptr.h>
#include <exceptionGutz.h>
#include <iostream>
#include <list>

using gutz::Exception;

///////////////////////////////////////////////////////////////////////////
/// policy namespace
/// 
/// The Thread and Mutex classes defined in this namespace do not perform
/// any actual work. These classes are template classes and the policy
/// that performs the work is passed in at compile time.
///
/// This allows us to provide a consistent interface to multiple thread 
/// implementation, such as Qt's thread classes or POSIX threads.
namespace policy
{
   ///////////////////////////////////////////////////////////////////////////
   /// Wraps up any type into a concrete type. This is key to making
   /// gutz::Thread(arbitraryType) work
   template<typename F>
   class ThreadData : public impl::ThreadData
   {
   public:
      ThreadData(const F& f) : _f(f) {}
      virtual ~ThreadData() {}

      void run(void)
      {
         _f();
      }


   private:
      F _f;
   };
   
   ///////////////////////////////////////////////////////////////////////////
   /// Wraps up a pointer to any type into a concrete type. This is key to
   /// making gutz::Thread thread(arbitraryType) work
   template<typename F>
   class ThreadDataPtr : public impl::ThreadData
   {
   public:
      ThreadDataPtr(F* f) : _f(f) {}
      virtual ~ThreadDataPtr() {}
      
      void run(void)
      {
         _f->operator()();
      }
      
   private:
      F* _f;
   };
  
   ///////////////////////////////////////////////////////////////////////////
   /// Copy an instance of a type, any type, onto the heap. This is very 
   /// important to making gutz::Thread thread(arbType) work. arbType could
   /// be on the calling thread's stack and could go out of scope. The thread
   /// needs its own copy on the heap
   template<typename F>
   static inline impl::ThreadData* makeThreadData(const F& f)
   {
      return static_cast<impl::ThreadData*>(new ThreadData<F>(f));
   }
   
   
   ///////////////////////////////////////////////////////////////////////////
   /// Wrap up a pointer to any type into a concrete type. The assumption
   /// is that the pointer points to something that is already on the heap.
   /// If the pointer is to something that is on the stack, well, the caller
   /// is in for a surprise.
   template<typename F>
   static inline impl::ThreadData* makeThreadData(F* f)
   {
      return static_cast<impl::ThreadData*>(new ThreadDataPtr<F>(f));
   }
   
   ///////////////////////////////////////////////////////////////////////////
   /// Wrap up a smart pointer to any type into a concrete type. The assumption
   /// is that the pointer points to something that is already on the heap.
   /// If the pointer is to something that is on the stack, well, the caller
   /// is in for a surprise.
   template<typename F>
   static inline impl::ThreadData* makeThreadData(gutz::SmartPtr<F> f)
   {
      return static_cast<impl::ThreadData*>(new ThreadDataPtr<F>(f.getPtr()));
   }

   class ThreadWrapper;
   
   ////////////////////////////////////////////////////////////////////////
   /// Basic thread class. Wraps up threads in a very simple fashion
   ///
   /// To create a thread, derive a class that implements the run function.
   /// Threads are not copyable
   ///
   /// Example for using the gutz Thread and Mutex classes:
   ///
   /// \code
   /// #include <threadGutz.h>
   /// #include <iostream>
   /// 
   /// using namespace gutz;
   /// 
   /// // Store data about the gutz in chickens
   /// class ChickenGutz
   /// {
   /// public:
   ///    ChickenGutz(void) : _entrails(0) {}
   ///    
   ///    void inc(void)
   ///    {
   ///       // Lock the mutex before incrementing
   ///       _mutex.lock();
   ///       _entrails++;
   ///       _mutex.unlock();
   ///    }
   ///    
   ///    const int num(void) const { return _entrails; }
   /// private:
   ///    int _entrails;
   ///    Mutex _mutex;
   /// };
   /// 
   /// /// Derived thread class for testing purposes only.
   /// class ChickenInc
   /// {
   /// public:
   ///    ChickenInc(ChickenGutz* chickenGutz) : _chickenGutz(chickenGutz), _die(false) {}
   ///    virtual ~ChickenInc() {}
   ///    
   ///    void operator()()
   ///    {
   ///       std::cout << "ChickenInc::operator()" << std::endl;
   ///       
   ///       // Keep incrementing until told to stop
   ///       while(!_die)
   ///       {
   ///          _chickenGutz->inc();
   ///       }
   ///    }
   ///    
   ///    void die(void)
   ///    {
   ///       _die = true;
   ///    }
   ///    
   /// private:
   ///    ChickenGutz* _chickenGutz;
   ///    bool _die;
   /// };
   /// 
   /// /// Entry point
   /// int main()
   /// {
   ///    ChickenGutz* chickenGutz = new ChickenGutz;
   ///    ChickenInc  inc1(chickenGutz);
   ///    ChickenInc  inc2(chickenGutz);
   //     gutz::Thread thr1(inc2);
   ///    gutz::Thread thr2(inc2);
   ///    
   ///    std::cout << "Started threads" << std::endl;
   ///    sleep(1);
   ///    std::cout << "Called sleep" << std::endl;
   ///    th1.join();
   ///    th2.join();
   ///    std::cout << "Die called" << std::endl;
   ///    std::cout << "num: " << chickenGutz->num() << std::endl;
   ///    delete chickenGutz;
   ///    return 0;
   /// }
   /// \endcode
   ///
   /// It is possible to create a class derived from Thread and overload
   /// the run() method as opposed to using the functor method described
   /// above. This isn't recommend as std::thread in C++0x uses the 
   /// functor method.
   ///
   /// It is also possible to create a derived class, overload operator()
   /// and then pass in an instance of that class to gutz::Thread.
   ///
   /// This is not supported and there's no reason to do it. Your program
   /// will segfault if you do this.
   ////////////////////////////////////////////////////////////////////////
   class Thread : public gutz::Counted
   {
   public:
      typedef gutz::SmartPtr<policy::Thread> pointer;
      typedef std::list<pointer> container;
      typedef std::list<pointer>::iterator iterator;
      
      ////////////////////////////////////////////////////////////////////////
      /// Create a thread instance, but do not start the thread
      Thread(void)
      {
         ThreadWrapper tw(this);
         _functor = makeThreadData(tw);
      }


      ////////////////////////////////////////////////////////////////////////
      /// Create a thread instance, start the thread. Type F needs to implement
      /// operator(). 
      // Top level thread
      template <class F>
      explicit Thread(const F& f)
      : _functor(makeThreadData(f))
      {
         start();
      }
      
      ////////////////////////////////////////////////////////////////////////
      /// Create a thread instance, start the thread. Type F needs to implement
      /// operator()
      template <class F>
      explicit Thread(F* f)
      : _functor(makeThreadData(f))
      {
         start();
      }
      
      ////////////////////////////////////////////////////////////////////////
      /// Destructor
      virtual ~Thread()
      {
         delete _functor;
      }

      ////////////////////////////////////////////////////////////////////////
      /// Start a thread
      void start(void)
      {
         if(!isRunning())
         {
            _functor->start();
         }
      }

      ////////////////////////////////////////////////////////////////////////
      /// Makes the calling thread wait until this child thread completes
      bool join(void)
      {
         return _functor->wait();
      }
      
      ////////////////////////////////////////////////////////////////////////
      /// Is this thread running
      const bool isRunning(void) const { return _functor->isRunning(); }
      
      ////////////////////////////////////////////////////////////////////////
      /// Sleep for msecs milliseconds
      void msleep(unsigned long msecs)
      {
        #ifdef WIN32
            Sleep(msecs * 1000);
        #else
           msleep(msecs * 1000);
        #endif
      }
      
   protected:
      virtual void run(void) {}
      
   private:
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Wraps up the Thread class. This is used when a Thread is created using
      /// the Thread inheritance method and overloads the run() function
      class ThreadWrapper
      {
      public:
         ThreadWrapper(Thread* thread)
         : _thread(thread) {}
         
         void operator()(void)
         {
            _thread->run();
         }
         
      private:
         Thread* _thread;
      };
      
      impl::ThreadData* _functor;
   };
   
}

#endif
