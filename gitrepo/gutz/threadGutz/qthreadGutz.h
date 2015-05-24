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

#ifndef _QTHREAD_GUTZ_H
#define _QTHREAD_GUTZ_H

#include <smartptr.h>
#include <list>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>


namespace impl
{
   typedef QThread ThreadData;
   
   ///////////////////////////////////////////////////////////////////////////
   /// Mutual Exclusion Lock - QThread implementation
   ///////////////////////////////////////////////////////////////////////////
   class Mutex : public gutz::Counted, protected QMutex
   {
      friend class Condition;
      
   public:
      //////////////////////////////////////////////////////////////////////////////////
      /// Default constructor
      Mutex(void)
      : QMutex(),
      _status(UNINITIALIZED) {}

      //////////////////////////////////////////////////////////////////////////////////
      /// Destructor
      virtual ~Mutex() { }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Lock a mutex
      /// Returns true if the mutex is successfully locked
      bool lock(void)
      {
         QMutex::lock();
         _status = LOCKED;
         return true;
      }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Check to see if a mutex is locked
      const bool isLocked(void) const { return _status == LOCKED; }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Unlock a mutex
      ///
      /// Returns trus if the mutex is successfully unlocked
      bool unlock(void)
      {
         QMutex::unlock();
         _status = UNLOCKED;
         return true;
      }
      
   private:
      Mutex(const Mutex& m);
      Mutex& operator=(const Mutex& m);

#ifdef Status
#undef Status
#endif

      enum Status
      {
         UNINITIALIZED,
         LOCKED,
         UNLOCKED,
      };
      
      Status _status;
   };

   class Condition : protected QWaitCondition
   {
   public:
      void wait(Mutex* mutex)
      {
         QMutex* qmutex = dynamic_cast<QMutex*>(mutex);
         QWaitCondition::wait(qmutex);
      }
      
      void wakeAll(void)
      {
         QWaitCondition::wakeAll();
      }
      
      void wakeOne(void)
      {
         QWaitCondition::wakeOne();
      }
   };
}

#endif
