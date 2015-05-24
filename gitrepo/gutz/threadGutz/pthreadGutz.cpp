///////////////////////////////////////////////////////////////////////////
//              _____________  ______________________ 
//             /  ________  |  |   ___   ________   / 
//            |  |       |  |_ |  |_ |  |       /  /  
//            |  |  ___  |  || |  || |  |      /  /   
//            |  | |   \ |  || |  || |  |     /  /    
//            |  | |_@  ||  || |  || |  |    /  /     
//            |  |___/  ||  ||_|  || |  |   /  /_____________________
//             \_______/  \______/ | |__|  /___________________________
//                        |  |__|  |
//                         \______/
//                    University of New Mexico
//                           2010
///////////////////////////////////////////////////////////////////////////

#include <pthreadGutz.h>
#include <exceptionGutz.h>

using gutz::Exception;

namespace impl
{
   void ThreadData::start(void)
   {
      static const std::string base = "pthread ThreadData::create failed: ";
      
      // If thread is already started, don't start it again
      if(_status != UNINITIALIZED)
      {
         throw Exception(base + "already initialized");
      }
      
      // Create the thread!
      _status = PENDING;
      
      switch(pthread_create(&_pthread, NULL, trampoline, this))
      {
         case 0:
            _status = RUNNING;
            break;
            
         case EAGAIN:
            _status = UNINITIALIZED;
            throw Exception(base + "The system lacked the necessary resources to create another thread, or the system-imposed limit on the total number of threads in a process [PTHREAD_THREADS_MAX] will be exceeded.");
            break;
            
         case EINVAL:
            _status = UNINITIALIZED;
            throw Exception(base + "invalid thread attributes. This is likely a bug in gutz::Thread");
            break;
            
         default:
            _status = UNINITIALIZED;
            throw Exception(base + "An undocumented error occurred when calling pthread_create. This will make debugging really hard. Sorry, this is totally pthread's fault.");
            break;
      }
   }
   
   ThreadData::~ThreadData()
   {
      static const std::string base = "pthread Thread::~Thread() failed: ";
      
      // Spin lock - wait for thread to finish creation before deleting
      while (_status == PENDING) {}
      
      if(_status == RUNNING)
      {
         switch(pthread_detach(_pthread))
         {
            case 0:
               _status = UNINITIALIZED;
               break;
               
            case EINVAL:
               throw Exception(base + "This is not a joinable thread. Why this caused pthread_detach to fail is unclear. Sorry for the crappy message, I wish this was more helpful");
               break;
               
            case ESRCH:
               //               std::cout << Thread::currentId() << std::endl;
               throw Exception(base + "No thread could be found corresponding to this thread id");
               break;
               
            default:
               throw Exception(base + "pthread_detach() returned an undocumented error code");
               break;
         }
      }
   }      
   
   //////////////////////////////////////////////////////////////////////////////////
   /// Join / wait
   bool ThreadData::wait(void)
   {
      static const std::string base = "pthread ThreadData::wait failed: ";
      
      _status = WAITING;
      switch(pthread_join(_pthread, NULL))
      {
         case 0:
            _status = UNINITIALIZED;
            break;
            
         case EDEADLK:
            throw Exception(base + "A deadlock was detected or Thread::wait was called by itself, which would cause a deadlock");
            break;
            
         case EINVAL:
            throw Exception(base + "This is not a joinable thread, which is what Thread::wait actually does. It does a thread join.");
            break;
            
         case ESRCH:
            throw Exception(base + "This is an invalid thread");
            break;
            
         default:
            throw Exception(base + "pthread_join returned an undocumented error code");
            break;
      }
      
      return _status == UNINITIALIZED;
   }
   
   //////////////////////////////////////////////////////////////////////////////////
   /// Default constructor
   Mutex::Mutex(void)
   : _status(UNINITIALIZED)
   {
      static const std::string base = "pthread::Mutex::Mutex() failed: ";
      
      int mutexType = PTHREAD_MUTEX_NORMAL;
      // Need to add support for recursive mutexes. 
#if 0
      if(attr == Recursive)
      {
         mutexType = PTHREAD_MUTEX_RECURSIVE;
      }
#endif
      // Initialize a pthread mutex attribute data structure
      switch (pthread_mutexattr_init(&_mutexAttr))
      {
         case 0:
            break;
         case ENOMEM:
            throw Exception(base + "Out of memory");
            break;
         default:
            throw Exception(base + "Undocumented pthread_mutexattr_init error");
            break;
      }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Set the type of mutex based on the template parameter MUTEX_TYPE
      switch(pthread_mutexattr_settype(&_mutexAttr, mutexType))
      {
         case 0:
            break;
         case EINVAL:
            throw Exception(base + "Invalid value for mutex attributes, or invalid value for mutex type");
            break;
         default:
            throw Exception(base + "Undocumented pthread_mutexattr_settype error");
            break;
      }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Initialize the mutex
      switch(pthread_mutex_init(&_mutex, &_mutexAttr))
      {
         case 0:
            break;
            
         case EAGAIN:
            throw Exception(base + "The system temporarily lacks the resources to create another mutex.");
            break;
            
         case EINVAL:
            throw Exception(base + "Invalid mutex attribute given to pthread_mutex_init");
            break;
            
         case ENOMEM:
            throw Exception(base + "The process cannot allocate enough memory to create another mutex.");
            break;
            
         default:
            throw Exception(base + "pthread_mutex_init returned an undocumented error code");
            break;
      }
      _status = INITIALIZED;
   }
   
   //////////////////////////////////////////////////////////////////////////////////
   /// Destructor
   Mutex::~Mutex() 
   {
      static const std::string base = "pthread::Mutex::~Mutex() failed: ";

      // If the mutex is initialized, destroy the mutex attribute structure and the mutex
      if(_status != UNINITIALIZED)
      {
         switch(pthread_mutexattr_destroy(&_mutexAttr))
         {
            case 0:
               break;
               
            case EINVAL:
               throw Exception(base + "Invalid mutex attribute");
               break;
               
            default:
               throw Exception(base + "pthread_mutexattr_destroy returned an undocumented error code");
               break;
         }
         
         switch(pthread_mutex_destroy(&_mutex))
         {
            case 0:
               _status = UNINITIALIZED;
               break;
               
            case EBUSY:
               throw Exception(base + "this mutex is locked by a thread");
               break;
               
            case EINVAL:
               throw Exception(base + "mutex invalid");
               break;
               
            default:
               throw Exception(base + "pthread_mutex_destroy returned an undocumented error code");
               break;
         }
      }
   }
   
   //////////////////////////////////////////////////////////////////////////////////
   /// Lock a mutex
   /// Returns true if the mutex is successfully locked
   bool Mutex::lock(void) 
   {
      static const std::string base = "pthread::Mutex::lock failed: ";
      bool locked = false;
      
      // Don't bother locking a mutex that is unitialized
      if(_status != UNINITIALIZED)
      {
         switch(pthread_mutex_lock(&_mutex))
         {
            case 0:
               locked = true;
               break;
            case EDEADLK:
               throw Exception(base + "a deadlock will occur");
               break;
            case EINVAL:
               throw Exception(base + "mutex invalid");
               break;
            default:
               throw Exception(base + "an undocumented error code was returned by pthread_mutex_lock");
               break;
         }
      }
      else
      {
         throw Exception(base + "mutex unitialized");
      }
      
      
      return locked == true;
   }
   
   //////////////////////////////////////////////////////////////////////////////////
   /// Unlock a mutex
   ///
   /// Returns trus if the mutex is successfully unlocked
   bool Mutex::unlock(void) 
   {
      static const std::string base = "pthread::Mutex::unlock failed: ";
      bool unlocked = false;
      if(_status != UNINITIALIZED)
      {
         switch(pthread_mutex_unlock(&_mutex))
         {
            case 0:
               unlocked = true;
               break;
            case EINVAL:
               throw Exception(base + "invalid mutex");
               break;
            case EPERM:
               throw Exception(base + "current thread does not hold a lock on this mutex");
               break;
         }
      }
      else
      {
         throw Exception(base + "mutex unitialized");
      }
      return unlocked == true;
   }
   
   //////////////////////////////////////////////////////////////////////////////////
   /// Default constructor
   Condition::Condition(void)
   : _status(UNINITIALIZED)
   {
      static const std::string base = "pthread::WaitCondition::WaitCondition() failed: ";
      
      // Initialize a pthread mutex attribute data structure
      switch (pthread_condattr_init(&_condAttr))
      {
         case 0:
            break;
         case ENOMEM:
            throw Exception(base + "Out of memory");
            break;
         default:
            throw Exception(base + "Undocumented pthread_condattr_init error");
            break;
      }
      
      //////////////////////////////////////////////////////////////////////////////////
      /// Initialize the mutex
      switch(pthread_cond_init(&_condition, &_condAttr))
      {
         case 0:
            break;
            
         case EAGAIN:
            throw Exception(base + "The system temporarily lacks the resources to create another condition.");
            break;
            
         case EINVAL:
            throw Exception(base + "Invalid condition attribute given to pthread_cond_init");
            break;
            
         case ENOMEM:
            throw Exception(base + "The process cannot allocate enough memory to create another condition.");
            break;
            
         default:
            throw Exception(base + "pthread_cond_init returned an undocumented error code");
            break;
      }
      _status = INITIALIZED;
   }
   
   //////////////////////////////////////////////////////////////////////////////////
   /// Destructor
   Condition::~Condition() 
   {
      static const std::string base = "pthread::WaitCondition::~WaitCondition() failed: ";
      
      // If the mutex is initialized, destroy the mutex attribute structure and the mutex
      if(_status != UNINITIALIZED)
      {
         switch(pthread_condattr_destroy(&_condAttr))
         {
            case 0:
               break;
               
            case EINVAL:
               throw Exception(base + "Invalid condition attribute");
               break;
               
            default:
               throw Exception(base + "pthread_condattr_destroy returned an undocumented error code");
               break;
         }
         
         switch(pthread_cond_destroy(&_condition))
         {
            case 0:
               _status = UNINITIALIZED;
               break;
               
            case EBUSY:
               throw Exception(base + "this condition is locked by a thread");
               break;
               
            case EINVAL:
               throw Exception(base + "condition invalid");
               break;
               
            default:
               throw Exception(base + "pthread_cond_destroy returned an undocumented error code");
               break;
         }
      }
   }
}
