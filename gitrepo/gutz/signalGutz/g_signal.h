/*
* $Id: g_signal.h,v 1.4 2006/11/17 18:38:30 jmk Exp $
*/
///////////////////////////////////////////////////////////////////////////
//              _____________  ______________________    __   __  _____
//             /  ________  |  |   ___   ________   /   |  \ /  \   |
//            |  |       |  |_ |  |_ |  |       /  /    \__  |      |
//            |  |  ___  |  || |  || |  |      /  /        | |      |
//            |  | |   \ |  || |  || |  |     /  /      \__/ \__/ __|__
//            |  | |_@  ||  || |  || |  |    /  /          Institute
//            |  |___/  ||  ||_|  || |  |   /  /_____________________
//             \_______/  \______/ | |__|  /___________________________
//                        |  |__|  |
//                         \______/
//                    University of Utah       
//                           2002
//
// By Joe Kniss, with help from Yarden Livnat
///////////////////////////////////////////////////////////////////////////

//signal.h

#include <iostream>
#include <vector>
#include "signalCall.h"
#include "signalIF.h"

//#ifdef __linux__
#include <typeinfo>
//#endif

#ifndef __GUTZ_SIGNAL_DOT_H
#define __GUTZ_SIGNAL_DOT_H


namespace gutz {

// connect forward decl
template<class SIG, class CE, class F>
void connect( SIG &s,          //< the signal
              CE *callee,      //< pointer to class with slot
              F fncPtr,        //< member function of "callee" (the slot)
              int priority = 0 //< optional priority, higher = earlier called
             );

// disconnect forward decl
template<class SIG, class CE, class F>
void disconnect( SIG &s,       //< the signal
                 CE *callee,   //< pointer to class with slot
                 F fncPtr      //< member function of "callee" (the slot)
                );

////////////////////////////////////////////////////////////////////////////
/// An API for signals and slots. 
/// See also gutz::connect and gutz::disconnect in signalGutz.h
///
/// If a class has member functions that will be used as slots, ie, 
///   functions that recieve signals from other classes, you need to 
///   have this public declaration in that class:
///
///  \code HAS_SLOTS; \endcode
///
/// Other than that, a slot is simply any member function of a class, 
///  that's it!
///    here is an example class that... HAS_SLOTS;
/// \code
/// class MyClassB {
///    public: // blah blah constructors...
///       HAS_SLOTS;
///       void setTheFloat(float f) {...} /// a slot taking a float
///       void someEmptySlot();           /// a slot with 0 parameters...
///                                       /// ... defined elsewhere
/// };
/// \endcode
///  Notice that we only have "HAS_SLOTS;" declared once!  That's all you 
///   need no matter how many different slots you have.  Also notice that 
///   you don't need the "gutz::" namespace resolution, we took care of that 
///   for you.  HAS_SLOTS is a #define that declares a member variable in 
///   your class, it is used to keep track of any signal that connects to 
///   a slot.  This is needed so that you can delete the class whenever you 
///   need without having to worry about disconnecting every signal attached 
///   to it.  
///
/// If you want to create a signal, i.e. a "function" that emmits an event,
///   you need to use the Signal<*> that is appropriate for the number of 
///   arguments that it will need, ex:
/// \code
/// class MyClassA {
///    public: //...
///       gutz::Signal<float>  myFloatChanged;
/// }
/// \endcode
/// "myFloatChanged" can now be used like a function that takes 1 float
///    argument:
/// \code
///  myFloatChanged(1.0f);
/// \endcode
///
///  Remember that although you use myFloatChanged like a function, it is 
///    really a "functor", a class that has "operator()" overloaded so that
///    it behaves like a function.  For you this only affects the way it is
///    declared, as above, where the items in "<>"s are the types of the 
///    parameters used when it is called. \n
///  A signal that takes 0 parameters looks like:
/// \code
///  gutz::Signal<>  myEmptySignal;
/// \endcode
///
///  You can have up to 8 parameters in a signal right now, just declare your
///    signal with the number of parameters it will take, for example here is 
///    another signal that takes 4 parameters:
/// \code
///  gutz::Signal<float, double, vec3f, const char *>  mySignalThingy;
/// \endcode
///
/// If you want a member function of a class to recieve a signal, you 
///    "connect" them together like so:
/// \code
/// gutz::connect(a.myFloatChanged, &b, &MyClassB::setTheFloat)
/// \endcode
/// This means {connect "a"'s signal "myFloatChanged" to "b"'s  member 
///    function "setTheFloat"}, ( where "a" is an instance of "MyClassA", 
///    and "b" is an instance of class "MyClassB").  Notice that "a" is 
///    passed to "connect" by reference, "b" is passed as a pointer, and 
///    "setTheFloat" is passed as a pointer to a member function. \n
/// Remember that the class recieving the signal ("MyClassB" in this example) 
///    must have "HAS_SLOTS" inside it's declaration. 
/// connect() also takes a priority integer value, the higher the number the
///    earlier the connected function will be called when there are 
///    multiple slots recieving the signal, mostly usefull for debugging,
///    probably not a usefull extension since we have no "real" symantics
///    associated with this value. See gutz::connect for more details.
///
/// Signal Implementation Notes:\n 
///  an implementation of the Signal interface.
///  A generic signal that can take upto 8 parameters, the types are specified
///  by the template parameters A*, which default to the "not applicable" type,
///  which is simply an empty class.  The Call Type <signalCalls.h> is 
///  speciallized based on wether or not a parameter is a real type or NA to 
///  call the correct slot function. 
///
/// TODO: could setup some kind of default parameters for the signal, with
/// another huge list of template parameters, but is that really necessary?
///
/// argument types, default to "not applicable" (NA) 
///////////////////////////////////////////////////////////////////////////
template< class A1 = NA,  class A2 = NA,  class A3 = NA,  class A4 = NA,
          class A5 = NA,  class A6 = NA,  class A7 = NA,  class A8 = NA>
class Signal : public SignalIF {
public:
   typedef Signal<A1,A2,A3,A4,A5,A6,A7,A8>  MyType;
   typedef _CallIF<A1,A2,A3,A4,A5,A6,A7,A8> CallIFT;
   typedef std::vector<CallIFT*>            CallPVec;
   typedef typename CallPVec::iterator      CallPVecIter;
   Signal() {}
   ~Signal() 
   {
      /// notify all slots/calls that the signal is no longer valid
      for(int i=0; i<int(_calls.size()); ++i)
      {
         _calls[i]->detatch(this);
         delete _calls[i];
      }
   }

   ///////////////////////////////////////////////////////////////
   ///@name Functor call
   ///  Takes up to 8 args.
   ///  You only need to call this with the number of args
   ///  that are valid for the signal.
   ///@{
   void operator() (A1 a1=NA(), A2 a2=NA(), A3 a3=NA(), A4 a4=NA(), 
                    A5 a5=NA(), A6 a6=NA(), A7 a7=NA(), A8 a8=NA()) const
   {
      for(int i=0; i<int(_calls.size()); ++i)
      {
         dbg("operator()", i);
         _calls[i]->call(a1,a2,a3,a4,a5,a6,a7,a8);
      }
   }
   ///@}
   ///////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////
   ///@name Utilities
   ///@{
   
   /// check if the slot has anyone attached to it.
   ///  You might want to know if calling the slot will have any 
   ///  affect.  This is especially important if the parameters
   ///  you are passing are not by reference and are "heavy", ie
   ///  large objects.
   bool slotsAttached() const { return _calls.size() != 0; }
   
   ///@}
   ///////////////////////////////////////////////////////////////


protected:

   ///////////////////////////////////////////////////////////////
   ///@name Framework stuff.
   ///  You can ignore these functions, they are used by 
   ///   the signals and slots framework.  \n
   ///  Hiding these was easy using template friends, however,
   ///  sucky because we needed a full template declaration, even
   ///  though we would have liked a partial specialization of 
   ///  class where the signal is always us, ie MyType.
   ///@{

   template<class SIG, class CE, class F> 
      friend void connect(SIG&,CE*,F,int); 

   /// add a slot/call at users behest, used by gutz::connect
   template<class CE, class F>
   void addCall(CE *callee, F fncPtr, int priority)
   {
      dbg("addCall, priority", priority);
      dbg(typeid(CE).name());
      dbg(typeid(fncPtr).name());
      CallPVecIter i = _calls.begin();
      while( ( i != _calls.end() ) && ((*i)->_p > priority) ) ++i;
      _calls.insert(i, new Call<CE,F,A1,A2,A3,A4,A5,A6,A7,A8>(callee, 
                                                              fncPtr, 
                                                              priority, 
                                                              this));
   }

   template<class SIG, class CE, class F> 
      friend void disconnect(SIG&,CE*,F);

   /// remove slot/call at users behest. used by gutz::disconnect
   template<class CE, class F>
   void delCall(CE *callee, F fptr)
   {
      _Call<CE,F,A1,A2,A3,A4,A5,A6,A7,A8> tc(callee, fptr);
      CallPVecIter i = _calls.begin();
      
      while( (i != _calls.end()) && (*_calls.begin())->isCall(&tc) )
	  {
	      delete (*i);
	      (*i) = 0;
	      _calls.erase(i);
	      i= _calls.begin();
	  }
      if( i == _calls.end() ) return;
      ++i;
      while((i != _calls.end()) && ( _calls.size() ))
      {
         CallPVecIter tmpi = i;
	     --i;
         if( (*tmpi)->isCall(&tc) )
         {
            delete (*tmpi);
            (*tmpi) = 0;
            _calls.erase(tmpi);
         }
	     else ++i;
	     ++i;
      }
   }

   friend class SignalTracker;

   /// remove slot/call when owner destructs...
   ///  from SignalIF interface, actually called by a
   ///  gutz::SignalTracker.
   void detatchSlotIF(void const *callee)
   {
      CallPVecIter i = _calls.begin();
      while( ( i != _calls.end() ) && (*_calls.begin())->isCallee(callee) )
      {
        delete( *i );
        (*i)=0;
        _calls.erase(i);
        i=_calls.begin();
      }
      if( i == _calls.end() ) return;
      ++i;
      while( (_calls.size()) && (i != _calls.end()) )
      {
         CallPVecIter tmpi = i;
         --i;
         if( (*tmpi)->isCallee(callee) )
         {
            delete (*tmpi);
            (*tmpi) = 0;
            _calls.erase(tmpi);
         }
         ++i;
      }
   }

   ///@}
   ///////////////////////////////////////////////////////////////

   /// printing debug statements
   inline void dbg(const char *where=0, int i1=-8888, int i2=-8888) const
   {
      #if 0 /// switch debug off, even in debug mode
      #ifdef _DEBUG
        std::cerr << "Signal::";
        if(where)
           std::cerr << where; 
        if(i1 != -8888)
           std::cerr << " : " << i1;
        if(i2 != -8888)
           std::cerr << ", " << i2; 
        std::cerr << std::endl;
      #endif
      #endif
   }

   CallPVec  _calls;   
};




} //< end namespace gutz

#endif


