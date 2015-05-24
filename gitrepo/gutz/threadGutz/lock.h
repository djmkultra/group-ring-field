#ifndef _LOCKABLE_H
#define _LOCKABLE_H

#if 0
namespace gutz
{
   class Lockable
   {
   public:
      virtual bool lock() = 0;
      virtual bool unlock() = 0;
      virtual ~Lockable();
      
   protected:
      Lockable() {}
      
   private:
      Lockable(const Lockable&);
      Lockable& operator=(const Lockable&);
   };
}
#endif

#endif
