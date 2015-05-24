//////////////////////////////////////////////////////////////
//
//
//  by Joe Kniss
//  jmk@cs.utah.edu
//  June 2003
//
//////////////////////////////////////////////////////////////

#ifndef __GUTZ_SMARTPTR_DOT_H
#define __GUTZ_SMARTPTR_DOT_H

// Added by Jeff Bowles. Used for conditional compilation in other
// header files. Example: GL::Buffer inherits gutz::Counted if this
// define is in place. This allows the usage of gutz Smart Pointers
// if this include is in place, but support gracefully disappears
// if GL::Buffer is used without gutz smart pointers.
#define GUTZ_SMARTPTR

#include <new>

/// define this to be 0 and smart-pointers behave the same as before
///   otherwise, you will get an assert and error message if you grab
///   a smart pointer to a variable created on the stack
#ifndef STACK_VAR_COUNT_CODE 
#define STACK_VAR_COUNT_CODE 0xDEADBEEF
#endif

// Turns out this is the documentation for the namespace...

/// GUTZ, the innards of all my applications.
///  Tons of usefull stuff, that in my mind really should be in 
///  the C++ standard library, or a Graphics standard libarary.
namespace gutz {

// forward decl of SmartPtr
template <class T> class SmartPtr;

//////////////////////////////////////////////////////////////
/// Counted, Derive your class from it if you want SmartPtr s to it.
///\code
/// class CMyObject : public Counted { ... };
///\endcode
/// Remember, never create a counted object on the stack!
///   (unless you never have SmartPtr s to it)
///
/// TODO: add stack-heap check using operator new and a
///  a static pointer map.
   
/// new attempt to distinguish stact from heap, NOT THREAD SAFE YET, but
   /// the only concequence of this is you may accidentially query a heap var,
   /// and have it report itself as a stack, then make an extra copy, not so bad
static void* last_heap_addr;
   
class Counted {
public:

   virtual ~Counted()                     {}

   enum {  
      STACK_VAR_ERROR_COUNT = 0xBADBEEF5, 
      COUNT_IS_POINTLESS    = STACK_VAR_COUNT_CODE,
      HEAP_ALLOC_CODE       = 0x1EA7BEEF
   };
   
   bool isStackVar() const { return _count == (int)COUNT_IS_POINTLESS; }
   
   void* operator new(size_t size)
   {
      void *storage = malloc(size);
      if( !storage ) throw "gutz::Counted failed to allocate";
      last_heap_addr = storage;
      return storage;
   }   
   

   
protected:
   
   template< class T > friend class SmartPtr;
   template< class T > friend class SmartPtrRef;

   /// gutz::Counted interface, increment reference count by one.
   ///   Not generaly used by subclasses, mostly for 
   ///   collaboration with gutz::SmartPtr.  Sometimes
   ///   you need to call this though, see the 
   ///   documentation for gutz::SmartPtr
   virtual void _incCount()               
   {
      _count += _count != COUNT_IS_POINTLESS ? 1 : 0; 
   }
   /// gutz::Counted interface, decrement reference count by one.
   ///   Not generaly used by subclasses, mostly for 
   ///   collaboration with gutz::SmartPtr.  Sometimes
   ///   you need to call this though, see the 
   ///   documentation for gutz::SmartPtr
   virtual void _decCount()               
   {
      _count -= _count != COUNT_IS_POINTLESS ? 1 : 0; 
   }
   /// gutz::Counted interface, get the current reference count.
   ///   Not generaly used by subclasses, mostly for 
   ///   collaboration with gutz::SmartPtr.  
   virtual int  _getCount() const         
   {
      return _count != COUNT_IS_POINTLESS ? _count : STACK_VAR_ERROR_COUNT;
   }

   /// protected constructor since this class does nothing by itself
   Counted(const Counted &c) { _count = _count == HEAP_ALLOC_CODE ? 0 : COUNT_IS_POINTLESS; }
   /// need to handle this correctly
   Counted &operator=(const Counted &c)   {return *this;}
   /// default constructor, protected!
   Counted()
   { 
      _count = static_cast<void*>(this) == last_heap_addr ? 0 : (int)COUNT_IS_POINTLESS; 
   }

private:
   /// Counted: reference count, if _getCount() == 0 then delete
   int _count;  
};



         

//////////////////////////////////////////////////////////////
/// Smart Pointer class.
///   Smart Pointers are FUN FUN FUN.
///
/// For all intents an purposes, SmartPtrs work just like regular
///  pointers, for instance you can:
/// \code
///   gutz::SmartPtr<MyClass> mcsp(); // create a "0" (Null) pointer by default
///   mcsp = new MyClass();           // assign a pointer
///   if( mcsp )                      // check if the pointer is non-zero
///   if( !mcsp )                     // check if the pointer is zero
///   if( mcsp == mcp )               // compare against another SmartPtr or raw pointer
///   mcsp->myFunction();             // call a member function
///   (*mcsp) = blah;                 // de-refrence
///
///   // declaration of some function taking a pointer to MyClass
///   void someFunc(MyClass *mcp);
///   // call that function, but let the SmartPtr auto-cast to a pointer
///   someFunc( mcsp ); // works fine too
/// \endcode
///  The difference is that you don't need to call delete on your
///   pointer, the SmartPtr<> will delete it when it gets deleted
///   AND holds the last reference to that object.  Just imagine a world
///   with no memory leaks. 
///
///  Const SmartPtrs work just like const ptrs. 
/// \code const SmartPtr<T> ~= T const* \endcode
///  You can always get at the "raw" pointer using:
/// \code mySP.getPtr(); \endcode;
///
///  There is one major caveaut: Any class that can have a SmartPtr
///   to it MUST be a subclass of gutz::Counted.  This insures that
///   we only have one (1) reference counter to any pointer, since it
///   is a member of the class rather than a "proxy-class".  One day I will 
///   implement an "AutoDeletPtr"  which allows you to do the same thing with 
///   non-Counted classes.  Forcing a Counted subclass, however, has advantages:
///  - You can cast to different levels of the hierarchy and still have valid SmartPtrs
///  - You can convert a pointer to a SmartPtr any time and still have valid refcounts
///  - A Counted Base class insures, via type checking, that we have only 1 refcounter per ptr
///
///  SmartPtr is itself a subclass of counted so that you can have
///  SmartPtrs to SmartPtrs and SmartPtrRef s.
///
///   Notes:  Having issues?
///   You shouldn't create a Counted object on the stack.
///    If you do, and somebody else gets a smart pointer to
///    that object, the smartptr will call delete on 
///    stack memory when the refcount drops to 0, which it
///    might if the function doesn't delete the object
///    first, both cases are bad.  
///    Here is an example that would cause an error.   
///\code
///  SmartPtr<T> newObjSP;
///  T newObj;
///  newObjSP = &newObj;
///\endcode
///   Notice that newObj is a local (stack) object.
///   The right way:
///\code
///  SmartPtr<T> newObjSP = new T;
///\endcode
///   the new object will get deleted when no one else has
///   a reference to it.
///
///  Another caveaut of smartptrs is that if an object is
///  using a smartptr to itself (why? who knows?), when 
///  the smartptr is deleted it might just delete the object
///  that is using it.  This happens especially when the 
///  constructor uses the smartptr, badness!  If 
///  you have to do this, you need to increment your own
///  reference counter first, then decrement when the ptr
///  is gone (to avoid memory leaks).  Again, be carefull,
///  if the smartptr is created on the stack, you're pretty 
///  much screwed, because you can't possibly decrement
///  after the smartptr is deleted.  This reference to self
///  situation can come up when iterators use smartptrs to the
///  object they are iterating through.  If the iterator is
///  used internally, then when it goes away, the object itself
///  might get deleted. Like say, when you need to iterate 
///  through yourself in the constructor :)  You know you are
///  getting into the danger zone when you do this 
///   (no pun intended):
///\code
///  // somewhere in a member function of "MyObj"
///  SmartPtr<MyObj> s(this);  /// this!!!
///\endcode
///   The right way would be:
///\code
///  // somewhere in a member function of "MyObj"
///  _incCount();
///  SmartPtr<MyObj> *sp = new SmartPtr<MyObj>(this);
///   <... do what you gota do ...>
///  delete sp;
///  _decCount();
///\endcode
///   Function calls are often the culprit, but this case is 
///   simpler, just inc and dec around the call:
///\code
///  // somewhere in a member function of "MyObj"
///  _incCount();
///  someFunctionUsingSPs(SmartPtr<MyObj>(this));
///  _decCount();
///\endcode
///  Remember, this only applies to use of a smartptr to "this" in
///  the objects member functions, or any function call using the "this" 
///  pointer or a SmartPtr to "this" in the objects constructors.
template <class T> 
class SmartPtr : public Counted 
{
public:
   typedef T type;

   SmartPtr() :_ref(0), _pucky(false)                  {  }
   SmartPtr(T* const ptr) :_ref(0),_pucky(false)       { _ref = 0; assign(ptr);}
   SmartPtr(const SmartPtr &sp):_ref(0),_pucky(false)  { _ref = 0; assign(sp._ref);}
   virtual ~SmartPtr()                                 { assign((T*)0); }

   //////////////////////////////////////////
   ///@name get the contained pointer
   ///@{
   T*       getPtr()                           { return _ref; }
   T* const getPtr() const                     { return _ref; }
   ///@}

   //////////////////////////////////////////
   ///@name assign another smart pointer, or a raw pointer
   ///@{
   SmartPtr & operator = (const SmartPtr &sp)  {assign(sp._ref); return *this;}
   // assign pointer or NULL
   T *operator = (T* const ptr)                {assign(ptr); return getPtr();}
   ///@}

   //////////////////////////////////////////
   ///@name access members of T
   ///    ex \code mySmartPtr->myFunction() \endcode
   ///@{
   T*       operator ->()                      { return getPtr(); }
   T* const operator ->() const                { return getPtr(); }
   ///@}

   /////////////////////////////////////////
   ///@name De-reference like a regular pointer
   ///     ex \code (*mySmartPtr).myFunction() \endcode
   ///@{
   T&       operator*()                        { return *getPtr(); }
   T&       operator*() const                  { return *getPtr(); }
   ///@}

   //////////////////////////////////////////
   ///@name Implicit conversion to T* 
   ///   good for function calls that take a 
   ///     regular pointer, for example: 
   /// \code 
   ///   ///declaration of some function, taking a pointer
   ///   void someFunc(someType *sc);
   ///   ///calling "someFunc" using a smartptr as the parameter
   ///   someFunc( mySP2someType );  /// converts SP to a raw pointer implicitly
   /// \endcode 
   ///   also needed for "if" tests:
   /// \code  if( mySP2someType ) /// test if ptr != 0 \endcode
   ///@{
   operator T* const () const                        { return getPtr();}
   //operator T*       ()                              { return getPtr();}
   ///@}

   //////////////////////////////////////////
   ///@name Casting for convenience
   ///  Handles dynamic cast for you.
   ///@{
   
   /// \code
   ///   mySubClassSP  = myBaseClassSP.cast<MySubClassType>();
   /// \endcode
   ///  You don't need this for an "up-cast" since this thing automatically
   ///  casts itself to a pointer which will then be up-casted by the compiler.
   template< class CT >
      SmartPtr<CT> cast() const 
   { return SmartPtr<CT>((CT*const)dynamic_cast<CT*const>(_ref)); }


   template< class CT >
     CT* cast()
   { return static_cast<CT*>(_ref); }
   ///@}

   //////////////////////////////////////////
   ///@name utilities, comparison ops
   /// just like raw ptrs, 
   /// \code
   /// if(SP != 0)... 
   /// if(SP == NULL)
   /// \endcode
   ///@{
   bool operator!()                       const { return getPtr()==0;}
#if 0 // these are completely redundant because of the auto cast
   bool operator==(const T *const ptr)    const { return getPtr()==ptr; }
   bool operator==(const SmartPtr<T> &sp) const { return getPtr()==sp.getPtr();}
   bool operator!=(const T *const ptr)    const { return getPtr()==ptr; }
   bool operator!=(const SmartPtr<T> &sp) const { return getPtr()!=sp.getPtr();}
   /// these may or maynot be what you want! be carefull
   /// they compare pointer values..
   bool operator<(const SmartPtr<T> &sp)  const { return getPtr()<sp.getPtr();}
   bool operator<=(const SmartPtr<T> &sp) const { return getPtr()<=sp.getPtr();}
   bool operator>(const SmartPtr<T> &sp)  const { return getPtr()>sp.getPtr();}
   bool operator>=(const SmartPtr<T> &sp) const { return getPtr()>=sp.getPtr();}
#endif
   /// just for convienence. access via,
   /// \code mySp.isNull() \endcode 
   bool isNull() const                           { return (getPtr()==0); }
   ///@}


protected:
   /// The only data we have is this pointer, so we have the same memory
   ///   foot print of a regular pointer.
   T *_ref;
   bool _pucky;

   void assign(T* const ref)
   {
      if( ref && ref->isStackVar() )
      {
         _pucky = true;
         /// fucked up here, smart pointer to stack variable
         std::cerr << " bad smart pointer assignment, idiot \n";
         assert( 0 );
      }
      else 
      {
         _pucky = false;
      }

      /// inc ref if non-zero
      if(ref!=0) ref->_incCount();
      /// save off old reference
      T *oldref = _ref;           
      /// assign _ref = ref
      _ref = ref;                 
      /// delete the old reference
      if(oldref!=0 && !_pucky){
         /// just a dec most of the time...
         oldref->_decCount();
         /// but if we hold the last reference, nuke it
         if((oldref->_getCount() <= 0))
         {
            delete oldref;
         }
      }
   }

};

//////////////////////////////////////////////////////////////
/// Smart Pointer Reference class, behaves like a 
///   reference to a pointer.
/// This class can be used for clean double indirection
/// of objects, ie. when an object changes all those with
/// one of these objects referencing it will see the 
/// changes.  Great for volitle pointers.
//////////////////////////////////////////////////////////////
template<class T>
class SmartPtrRef {
public:
   typedef SmartPtr<T> type;

   SmartPtrRef()                            { _ref = 0; assign(new type(0));}
   SmartPtrRef(T * ptr)                     { _ref = 0; assign(new type(ptr));}
   SmartPtrRef(const SmartPtrRef &dsp)      { _ref = 0; assign(dsp._ref); }
   SmartPtrRef(SmartPtr<T> *sp)             { _ref = 0; assign(sp);}
   virtual ~SmartPtrRef()                   { assign((type*)0); }

   //////////////////////////////////////////
   ///@name get the contained pointer
   ///@{
   T*       getPtr()                           { return _ref->getPtr(); }
   T const* getPtr() const                     { return _ref->getPtr(); }
   ///@}

   //////////////////////////////////////////
   ///@name assign smart pointer, remember this is 
   /// a reference to a pointer, not a pointer.
   ///@{ 
   SmartPtrRef & operator= (const SmartPtrRef &sp) { *_ref = *(sp._ref); return *this;}
   // assign pointer or NULL
   T *operator = (T * ptr)                     {*_ref = ptr; return ptr;}
   ///@}

   //////////////////////////////////////////
   ///@name access members of T
   ///   ex. T->myFunction()
   ///@{
   T*       operator ->()                      { return getPtr(); }
   T const* operator ->() const                { return getPtr(); }
   ///@}

   /////////////////////////////////////////
   ///@name De-reference like a regular pointer
   ///   ex (*mySmartPRef).myFunction()
   ///@{
   T&       operator*()                        { return *getPtr(); }
   T const& operator*() const                  { return *getPtr(); }
   ///@}

   //////////////////////////////////////////
   /// Implicit conversion to T* (for function calls)
   // Implicit conversions are a bit... scary. They introduce magic
   // that is difficult to debug. -jbowles 1/17/2012
   //   operator T* () { return _ref.getPtr();}


   //////////////////////////////////////////
   ///@name utilities, comparison ops
   ///    just like raw ptrs, if(T != 0)... if(T == NULL)
   ///@{
   bool operator!()                       const { return getPtr()==0;}
   bool operator==(const T* ptr)          const { return getPtr()==ptr; }
   bool operator==(const SmartPtrRef &sp) const { return getPtr()==sp.getPtr();}
   bool operator!=(const T* ptr)          const { return getPtr()==ptr; }
   bool operator!=(const SmartPtrRef &sp) const { return getPtr()!=sp.getPtr();}
   /// these may or maynot be what you want! be carefull
   bool operator<(const SmartPtrRef &sp)  const { return getPtr()<sp.getPtr();}
   bool operator<=(const SmartPtrRef &sp) const { return getPtr()<=sp.getPtr();}
   bool operator>(const SmartPtrRef &sp)  const { return getPtr()>sp.getPtr();}
   bool operator>=(const SmartPtrRef &sp) const { return getPtr()>=sp.getPtr();}
   /// for convienence since if(mySP) is always true,
   /// use if(!myPtr.isNull())...
   ///@}
   bool isNull() const                          { return (getPtr()==0); }

   //////////////////////////////////////////
   /// Private from here down
protected:
   type *_ref;

   void assign(type *ref)
   {
      
      /// add a reference to the new guy
      if(ref!=0) ref->_incCount(); 
      type *oldref = _ref;           //< save off old reference
      _ref = ref;                    //< assign new guy
      /// deleting the old guy
      if(oldref!=0){
         /// just a dec most of the time
         oldref->_decCount();
         /// but if we are the last reference, nuke it
         if(oldref->_getCount() <= 0)
         {
            delete oldref;
         }
      }
   }
};


//////////////////////////////////////////////////////////////
///  SmartPointer/ raw pointer unified casting
///
//////////////////////////////////////////////////////////////

template< class T, typename TOLD >
T dyn_cast( TOLD ptr ) 
{ return dynamic_cast<T>(ptr); }

template< class T, typename TOLD >
T dyn_cast( SmartPtr<TOLD> ptr ) 
{ return dynamic_cast<T>(ptr.getPtr()); }

/// Down cast a base type to a derived type. For this example, this
/// casts a smart pointer to an instance of Base to a smart pointer
/// to an instance of Derived
template<class DERIVED, class BASE>
gutz::SmartPtr<DERIVED> down_cast(gutz::SmartPtr<BASE>& base)
{
   DERIVED* derivedPtr = dynamic_cast<DERIVED*>(base.getPtr());
   gutz::SmartPtr<DERIVED> derived = derivedPtr;
   return derived;
}
   
/// Up cast a derived type to a base type. For this example, this
/// casts a smart pointer to an instance of Derived to a smart pointer
/// to an instance of Base
template<class BASE, class DERIVED>
gutz::SmartPtr<BASE> up_cast(gutz::SmartPtr<DERIVED>& derived)
{
   BASE* basePtr = static_cast<BASE*>(derived.getPtr());
   gutz::SmartPtr<BASE> base = basePtr;
   return base;
}

//////////////////////////////////////////////////////////////
/// Pointer Attribute classes. 
///   for use with template types where you only care about
///   how the pointer type is handled.  Here is an example,
///   notice how it uses template-templates 
/// \code
///   template< template<typename T> class PtrType >
///   class MyClass {
///       typedef PtrType< SomeClass >::PTR  SomeClassPtr;
///   };
///   typedef MyClass<StdPtrAttrib> MyClass_std; /// SomeClassPtr is a "SomeClass*"
///   typedef MyClass<SmartPtrAttrib> MyClass_SP; /// SomeClassPtr is a "SmartPtr<SomeClass>"
/// \endcode
//////////////////////////////////////////////////////////////

template< class T >
class StdPtrAttrib {
public:
   typedef T* PTR;
   static T* rawPtr( T* t) { return t; } 
};

template< class T >
class SmartPtrAttrib {
public:
   typedef SmartPtr<T> PTR;
   static T* rawPtr( PTR &t ) { return t.getPtr(); }
};

template< class T >
class SmartPtrRefAttrib {
public:
   typedef SmartPtrRef<T> PTR;
   static T* rawPtr( PTR &t ) { return t.getPtr(); }
};


} /// end namespace gutz

#endif
