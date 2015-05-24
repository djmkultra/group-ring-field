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
#ifndef _PTHREAD_GUTZ_H
#define _PTHREAD_GUTZ_H

#include <pthread.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <exceptionGutz.h>
#include <list>
#include <smartptr.h>
#include <lock.h>

using gutz::Exception;

namespace impl
{
   class ThreadData
   {
   public:
      ThreadData() : _status(UNINITIALIZED) {}
      void start(void);
      bool wait(void);
      virtual void run() = 0;
      const bool isRunning(void) const { return _status == RUNNING; }
      virtual ~ThreadData();
      
   private:
      //////////////////////////////////////////////////////////////////////////////////
      /// Bounce point to get into the thread
      static void* trampoline(void* arg)
      {
         // Cast arg into something that implements run(). If typename F does not
         // implement run(), then there should be some sort of meaningful error
         // beyond a segfault
         ThreadData* obj = static_cast<ThreadData*>(arg);
         
         // Run the thread!
         obj->run();
         
         return NULL;
      }
      
      /// Thread status enum. For internal use
#ifdef Status
#undef Status
#endif
      typedef enum
      {
         UNINITIALIZED,
         PENDING,
         RUNNING,
         WAITING
      } Status;

      Status    _status;
      pthread_t _pthread;
   };
   
      
      //   typedef gutz::SmartPtr<ThreadDataBase> ThreadDataPtr;   
      //   typedef ThreadDataBase* ThreadDataPtr;   
   
   // Forward declaration so that Condition can be friendly.
   class Condition;
   
   ///////////////////////////////////////////////////////////////////////////
   /// Mutual Exclusion Lock - pthread implementation
   ///////////////////////////////////////////////////////////////////////////
   class Mutex
   {
      // Conditions need direct access to the pthread_mutex_t structure
      friend class Condition;
      
   public:
      //////////////////////////////////////////////////////////////////////////////////
      /// Default constructor
      Mutex(void);
      
   protected:
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Destructor
      virtual ~Mutex();
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Lock a mutex
      /// Returns true if the mutex is successfully locked
      bool lock(void);
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Check to see if a mutex is locked
      //      const bool isLocked(void) const { return _status == LOCKED; }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Unlock a mutex
      ///
      /// Returns trus if the mutex is successfully unlocked
      bool unlock(void);
      
      pthread_mutex_t* pthreadMutex(void) { return &_mutex; }
      
   private:
      //////////////////////////////////////////////////////////////////////////////////
      /// Copy constructor
      Mutex(const Mutex& m);
      
      /// Assignment operator
      Mutex& operator=(const Mutex& m);
      
      typedef enum
      {
         UNINITIALIZED,
         INITIALIZED
      } Status;
      
      Status _status;
      pthread_mutex_t     _mutex;
      pthread_mutexattr_t _mutexAttr;
   };
   
   /////////////////////////////////////////////////////////////////////////////////////
   /// Wait for a condition to be met. Provides a way of synchronizing threads
   class Condition
   {
   public:
      //////////////////////////////////////////////////////////////////////////////////
      /// Default constructor
      Condition(void);

   protected:
      //////////////////////////////////////////////////////////////////////////////////
      /// Destructor
      virtual ~Condition();

      //////////////////////////////////////////////////////////////////////////////////
      /// Wait for condition to be true
      void wait(Mutex* mutex)
      {
         pthread_cond_wait(&_condition, mutex->pthreadMutex());
      }

      //////////////////////////////////////////////////////////////////////////////////
      /// Wake a single thread waiting on this condition
      void wakeOne(void)
      {
         pthread_cond_signal(&_condition);
      }

      //////////////////////////////////////////////////////////////////////////////////
      /// Wake all threads waiting on this condition
      void wakeAll(void)
      {
         pthread_cond_broadcast(&_condition);
      }
      
   private:
      typedef enum
      {
         UNINITIALIZED,
         INITIALIZED
      } Status;
      
      Status _status;
      
      pthread_cond_t _condition;
      pthread_condattr_t _condAttr;
   };
}

#endif
