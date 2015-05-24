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

#ifndef _MUTEX_POLICY_H
#define _MUTEX_POLICY_H

namespace policy
{
   
   ///////////////////////////////////////////////////////////////////////////
   /// Mutex template class.
   template<typename implementation>
   class Mutex : public implementation
   {
   public:
      //////////////////////////////////////////////////////////////////////////////////
      /// Default constructor
#if 1
      Mutex(void) : implementation() {}
#else
      // Need to add a recursive mutex without adding a bunch of code. There's a bunch
      // of ways to do this, but I haven't figured out how to do it in a nice clean 
      // manner.
      Mutex(const gutz::MutexAttr::Recursion& mode = NonRecursive) : implementation(mode) {}
#endif
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Destructor
      virtual ~Mutex() 
      {
      }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Lock a mutex
      /// Returns true if the mutex is successfully locked
      bool lock(void) 
      {
         return implementation::lock();
      }
      
      //////////////////////////////////////////////////////////////////////////////////
      // Check to see if a mutex is locked
      // Removed - this is meaningless. By the time the locked status is returned,
      // it may have changed. Does this mean a mutex needs a mutex? Probably. Do we really
      // need to know the status? I don't think so.
      //      const bool isLocked(void) const
      //      {
      //         return implementation::isLocked();
      //      }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Unlock a mutex
      ///
      /// Returns true if the mutex was successfully unlocked
      bool unlock(void)
      {
         return implementation::unlock();
      }
   private:
      /// Copy constructor
      Mutex(const Mutex& m) : implementation(m) {}
      
      /// Assignment operator
      Mutex& operator=(const Mutex& m)
      {
         return implementation::operator=(m);
      }
   };
}

#endif