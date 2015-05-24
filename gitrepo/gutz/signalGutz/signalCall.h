/*
* $Id: signalCall.h,v 1.4 2006/11/08 21:34:09 jmk Exp $
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

//signalCall.h

#ifndef __GUTZ_SIGNAL_CALLS_DOT_H
#define __GUTZ_SIGNAL_CALLS_DOT_H

#include <iostream>
#include <vector>

namespace gutz {

//// there is nothing in this header that should be of concern to users
////  of the gutz::signals API, you can ignore these classes, they are 
////  only for internal framework use.  "HAS_SLOTS" is declared in this
////  file but its use is discussed in <signalGutz.h>.  

/////////////////////////////////////////////////////////////////////////
/// the "Not Applicable" class, just an empty class for typeing and 
///  partial specialization... (Nil Argument?? :)
class _NA {};
typedef const _NA NA;
///
/////////////////////////////////////////////////////////////////////////

/// forward decl
class SignalIF;
/// ignore this! it is the name of a member variable in
///   classes supporting slots, used with HAS_SLOTS, and 
///   in "calls" to access the member variable.
#define G_SIG_TRACK _sigtrack_

/////////////////////////////////////////////////////////////////////////
///  Signal Tracker...
/// Users can ignore this class, NOT FOR GENERAL USE.
/// just some framework stuff related to insuring that
/// when an object deletes, we don't get seg faults because
/// a signal didn't know about it.
///
/// TODO: need to figure out how to copy signals!
///
///  This system works if an instance SignalTracker is declared in
///    a class recieving signals (has slots), since this object is destructed
///    with the owning object it will detatch all signals it recieves. 
///    This way, when an object with slots destructs, we won't get runtime
///    seg-faults when a signal is sent to an object that no longer exists.
///  This is why we have #HAS_SLOTS, it adds one of these to the class 
///    with a specific variable name that _Call will use when it is constructed.
class SignalTracker {
public:
   typedef std::vector<SignalIF*>  SignalPVec;
   typedef SignalPVec::iterator    SignalPVecIter;

   SignalTracker() {}
   SignalTracker(const SignalTracker &st) {}
   SignalTracker &operator=(const SignalTracker &st) {return *this;}
   ~SignalTracker();
   
   /// detach a signal 
   void detatchSignalIF(SignalIF *sig);
   void attachSignalIF(SignalIF *sig)
   { _sigs.push_back(sig); }

   void *callee;
protected:
   SignalPVec _sigs;   
};

/////////////////////////////////////////////////////////////////////////
/// if you use slots (functions called by signals) you must have:
/// HAS_SLOTS; 
/// inside the class, example:
///
/// class myClass {
/// public:
///    HAS_SLOTS;
/// }
///
/// Maybe we can all agree that this is better than requiring a 
///  baseclass just for supporting slots!
/////////////////////////////////////////////////////////////////////////

#define HAS_SLOTS  gutz::SignalTracker G_SIG_TRACK;

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/// The rest of this is also NOT FOR GENERAL USE, it is used in the 
///   signals & slots framework...
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
/// the call interface, to get rid of a few irrelevant types imposed by
///  the (more) concrete version below so that the Signal can keep a 
///  list/vector of these.  The number of arguments in the "call" classes
///  should match the number supported by signals.
template<class A1,class A2,class A3,class A4,
         class A5,class A6,class A7,class A8>
class _CallIF {
public:
   typedef A1 Arg1;

   virtual ~_CallIF() {}
   virtual void call(A1,A2,A3,A4,A5,A6,A7,A8) = 0;
   virtual void detatch(SignalIF *sig) = 0;
   virtual bool isCallee(void const *callee) const = 0;
   virtual bool isCall(_CallIF const *) const = 0;
   int _p;
protected:
   _CallIF(int priority) : _p(priority) {}

};

//////////////////////////////////////////////////////////////
/// an intermediate class so that we can speciallize one for the "call" function
///  based on types.  Needed since we will get compile errors if we declare a 
///  call() function with the wrong number of parameters, even if it is never
///  used.  (this is likely only an issue with some compilers but... need to support
///  at least a few of them :)
template<class CE,class F,class A1,class A2,class A3,class A4,
                          class A5,class A6,class A7,class A8>
class _Call : public _CallIF<A1,A2,A3,A4,A5,A6,A7,A8> {
public:
   typedef _CallIF<A1,A2,A3,A4,A5,A6,A7,A8> btype;

   _Call(CE *callee, F fptr, int priority, SignalIF *sig) 
      : btype(priority), _callee(callee), _func(fptr) 
   {
      /// if your compilation is dying here, this means
      /// that you forgot to add:
      /// HAS_SLOTS
      /// to a class that is recieving signals, ie has
      /// member functions that behave as slots.  The
      /// type of template parameter "CE" is the class 
      /// that needs HAS_SLOTS in its declaration.
      callee->G_SIG_TRACK.attachSignalIF(sig);
      /// sucks, but have to set this here since we don't want
      /// users bothered with the SignalTrackers constructor,
      /// I don't trust those diry users! :P
      callee->G_SIG_TRACK.callee = callee;
   }
   /// this one is just for creating a Call for comparisons
   ///  This class, by itself, doesn't implement the necessary
   ///  "call" function, the "Call" class below does.
   _Call(CE *callee, F fptr)
      : btype(0), _callee(callee), _func(fptr)
   {}
   ~_Call() {}
   
   /// this should NEVER be called, I implemented it so
   /// you can create a dummy _Call for comparisons, needed
   /// to delete a call by disconnect() from the Signal level.
   /// If you are creating one of these to actually be called,
   /// you need to create a "Call" instead!!!
   virtual void call(A1,A2,A3,A4,A5,A6,A7,A8) {};

   void detatch(SignalIF *sig)
   {
      _callee->G_SIG_TRACK.detatchSignalIF(sig);
   }
   bool isCallee(void const *callee) const
   {
      return _callee == callee; 
   }
   bool isCall(_CallIF<A1,A2,A3,A4,A5,A6,A7,A8> const *c) const
   {
      _Call const *tc = dynamic_cast<_Call<CE,F,A1,A2,A3,A4,A5,A6,A7,A8> const *>(c);
      if(!tc)
         return false;
      return (tc->_callee == _callee) && (tc->_func == _func);
   }
  

   CE *_callee;
   F   _func;
};

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//// declaration of Call, with implementation for 8 parameters
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
template<class CE,class F, 
         class A1,class A2,class A3,class A4,
         class A5,class A6,class A7,class A8>
class Call : public _Call<CE,F,A1,A2,A3,A4,A5,A6,A7,A8>
{
public:
   typedef _Call<CE,F,A1,A2,A3,A4,A5,A6,A7,A8> btype;
   Call(CE *callee, F fptr, int priority, SignalIF *sig)
      : btype(callee,fptr,priority,sig) 
   {}

   void call(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8)
   {
      (this->_callee->*(this->_func))(a1,a2,a3,a4,a5,a6,a7,a8);
   }
};

//////////////////////////////////////////////////////////////
/// implement/specialize call with 0 parameters
template<class CE, class F>
class Call<CE,F,NA,NA,NA,NA,NA,NA,NA,NA> 
   : public _Call<CE,F,NA,NA,NA,NA,NA,NA,NA,NA>
{
public:
   typedef _Call<CE,F,NA,NA,NA,NA,NA,NA,NA,NA> btype;
   Call(CE *callee, F fptr, int priority, SignalIF *sig)
      : btype(callee,fptr,priority,sig) 
   {}

   void call(NA,NA,NA,NA,NA,NA,NA,NA)
   {
      (this->_callee->*(this->_func))();
   }
};

//////////////////////////////////////////////////////////////
/// implement/specialize Call with 1 parameter
template<class CE,class F,class A1>
class Call<CE,F,A1,NA,NA,NA,NA,NA,NA,NA> 
   : public _Call<CE,F,A1,NA,NA,NA,NA,NA,NA,NA>
{
public:
   typedef _Call<CE,F,A1,NA,NA,NA,NA,NA,NA,NA> btype;
   Call(CE *callee, F fptr, int priority, SignalIF *sig)
      : btype(callee,fptr,priority,sig) 
   {}

   void call(A1 a1,NA,NA,NA,NA,NA,NA,NA)
   {
      (this->_callee->*(this->_func))(a1);
   }
};

//////////////////////////////////////////////////////////////
/// implement/specialize Call with 2 parameters
template<class CE,class F,class A1,class A2>
class Call<CE,F,A1,A2,NA,NA,NA,NA,NA,NA> 
   : public _Call<CE,F,A1,A2,NA,NA,NA,NA,NA,NA>
{
public:
   typedef _Call<CE,F,A1,A2,NA,NA,NA,NA,NA,NA> btype;
   Call(CE *callee, F fptr, int priority, SignalIF *sig)
      : btype(callee,fptr,priority,sig) 
   {}

   void call(A1 a1,A2 a2,NA,NA,NA,NA,NA,NA)
   {
      (this->_callee->*(this->_func))(a1,a2);
   }
};

//////////////////////////////////////////////////////////////
/// implement/specialize Call with 3 parameters
template<class CE,class F,class A1,class A2,class A3>
class Call<CE,F,A1,A2,A3,NA,NA,NA,NA,NA> 
   : public _Call<CE,F,A1,A2,A3,NA,NA,NA,NA,NA>
{
public:
   typedef _Call<CE,F,A1,A2,A3,NA,NA,NA,NA,NA> btype;
   Call(CE *callee, F fptr, int priority, SignalIF *sig)
      : btype(callee,fptr,priority,sig) 
   {}

   void call(A1 a1,A2 a2,A3 a3,NA,NA,NA,NA,NA)
   {
      (this->_callee->*(this->_func))(a1,a2,a3);
   }
};

//////////////////////////////////////////////////////////////
/// implement/specialize Call with 4 parameters
template<class CE,class F,class A1,class A2,class A3,class A4>
class Call<CE,F,A1,A2,A3,A4,NA,NA,NA,NA> 
   : public _Call<CE,F,A1,A2,A3,A4,NA,NA,NA,NA>
{
public:
   typedef _Call<CE,F,A1,A2,A3,A4,NA,NA,NA,NA> btype;
   Call(CE *callee, F fptr, int priority, SignalIF *sig)
      : btype(callee,fptr,priority,sig) 
   {}

   void call(A1 a1,A2 a2,A3 a3,A4 a4,NA,NA,NA,NA)
   {
      (this->_callee->*(this->_func))(a1,a2,a3,a4);
   }
};

//////////////////////////////////////////////////////////////
/// implement/specialize Call with 5 parameters
template<class CE,class F,class A1,class A2,class A3,class A4,
                          class A5>
class Call<CE,F,A1,A2,A3,A4,A5,NA,NA,NA> 
   : public _Call<CE,F,A1,A2,A3,A4,A5,NA,NA,NA>
{
public:
   typedef _Call<CE,F,A1,A2,A3,A4,A5,NA,NA,NA> btype;
   Call(CE *callee, F fptr, int priority, SignalIF *sig)
      : btype(callee,fptr,priority,sig) 
   {}

   void call(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,NA,NA,NA)
   {
      (this->_callee->*(this->_func))(a1,a2,a3,a4,a5);
   }
};

//////////////////////////////////////////////////////////////
/// implement/specialize Call with 6 parameters
template<class CE,class F,class A1,class A2,class A3,class A4,
                          class A5,class A6>
class Call<CE,F,A1,A2,A3,A4,A5,A6,NA,NA> 
   : public _Call<CE,F,A1,A2,A3,A4,A5,A6,NA,NA>
{
public:
   typedef _Call<CE,F,A1,A2,A3,A4,A5,A6,NA,NA> btype;
   Call(CE *callee, F fptr, int priority, SignalIF *sig)
      : btype(callee,fptr,priority,sig) 
   {}

   void call(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,NA,NA)
   {
      (this->_callee->*(this->_func))(a1,a2,a3,a4,a5,a6);
   }
};

//////////////////////////////////////////////////////////////
/// implement/specialize Call with 7 parameters
template<class CE,class F,class A1,class A2,class A3,class A4,
                          class A5,class A6,class A7>
class Call<CE,F,A1,A2,A3,A4,A5,A6,A7,NA> 
   : public _Call<CE,F,A1,A2,A3,A4,A5,A6,A7,NA>
{
public:
   typedef _Call<CE,F,A1,A2,A3,A4,A5,A6,A7,NA> btype;
   Call(CE *callee, F fptr, int priority, SignalIF *sig)
      : btype(callee,fptr,priority,sig) 
   {}

   void call(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,NA)
   {
      (this->_callee->*(this->_func))(a1,a2,a3,a4,a5,a6,a7);
   }
};

/// 8 parameters already defined in the declaration of Call!

} //< end namespace gutz

#endif


