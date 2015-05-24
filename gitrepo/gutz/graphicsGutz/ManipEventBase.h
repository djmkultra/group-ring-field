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
//                           2003
///////////////////////////////////////////////////////////////////////////

//ManipEventBase.h
// Joe Kniss

#ifndef __MANIP_EVENT_BASE_DOT_H
#define __MANIP_EVENT_BASE_DOT_H

#include <smartptr.h>
#include <map>


namespace gutz {

class MouseEvent;
class MouseMoveEvent;

///////////////////////////////////////////////////////////////////
/// Generic Manipulator Event Interface class.  Defines the functions
///  that MUST be defined for a Manipulator Constraint/Event handler.
///  All Events should sub-class from this guy.
///
/// Template parameter MT is the ManipType, right now this is either
/// a gutz::Manip or a gutz::Camera.  
/// See gutz::EventMap for a containter class.  
///
/// The role of this class is to provide a common interface for
/// events, this way we can map them etc... Notice that we have a 
/// pure virutal clone.  This means that we would like to copy
/// an event with out knowing anything about it.  Since the 
/// copy constructor just won't due, you have to clone. 
/// 
/// Although the name of this class is ManipEventBase, it is also
/// the base for Camera Events too.
///////////////////////////////////////////////////////////////////
template<class MT>
class ManipEventBase : public gutz::Counted
{
public:
   typedef MT ManipType; ///< either a Camera or a Manip (LIGHT??)

   virtual ~ManipEventBase() {}

   ///@name Clone
   ///@{
   virtual ManipEventBase<MT> *clone() const = 0;
   ///@}

   ///@name Event Handlers
   ///  Return true if the event had an error, false if everything was ok!
   ///@{

   /// should be called on a Mouse-down event, starts the event
   virtual bool startEvent(ManipType *m, const MouseEvent &me) = 0;
   /// should be called when the mouse moves
   virtual bool handleEvent(ManipType *m, const MouseMoveEvent &mme) = 0;
   /// called when the event is over, may or may not be on a Mouse-up event
   virtual void endEvent(ManipType *m, const MouseEvent &me) = 0;
   ///@}

   ///@name Tumble
   ///  optional event handler, means "repeat last event"
   ///@{
   virtual bool tumble(ManipType *m) {return false;}
   ///@}

   ///@name Speed
   ///@{
   float getSpeed() const      { return _speed; }
   void  setSpeed(float speed) { _speed = speed; }
   ///@}

protected:
   ManipEventBase(float speed = 1.0f): _speed(speed) {}
   ManipEventBase(const ManipEventBase &m): _speed(m._speed) {}
   ManipEventBase &operator=(const ManipEventBase &m) 
   { _speed = m._speed; return *this; }

   float _speed;
};

  //typedef ManipEventBase<Manip>          ManipEvent;
  //typedef ManipEventBase<Camera>         CameraEvent;
  //typedef gutz::SmartPtr<ManipEve>       ManipEventSP;
  //typedef gutz::SmartPtr<CameraEvent>    CameraEventSP;

///////////////////////////////////////////////////////////////////
/// Generic Manipulator Event Mapper.  It is simplest if manipulators
/// sub-class from this so that we are not duplicating the interface
/// all over the place. 
///
/// Template parameter MT is the ManipType, right now this is either
/// a gutz::Manip or a gutz::Camera. 
///
/// Only stores SmartPtrs not the acutal object, therefore copy
/// and asignment only copies the pointers, does not create 
/// new events. 
///
/// The role of this class is to provide the interface and 
/// event handling for all manipulator objects.  It maps event
/// handlers (see gutz::ManipEventBase) to buttons and keys
/// defined in gutzKeyMouse.h  It also handles mouse events
/// and forwards them to the appropriate event handler.  I chose
/// to make this class a base for Manip and Camera since I really
/// don't want to duplicate the interface in both of these classes
/// and it wouldn't make sense to have more than one of them in
/// any class.  Plus it facilitates building and copying events
/// without having to create a manipulator or a camera. 
///////////////////////////////////////////////////////////////////
template<class MT>
class EventMap : 
   public std::map<unsigned int, 
                   gutz::SmartPtr< ManipEventBase<MT> >, 
                   std::less<unsigned int> >
{
public:
   typedef MT                         ManipType;
   typedef ManipEventBase<MT>         ManipEventType;
   typedef SmartPtr< ManipEventType > ManipEventTypeSP;
   typedef std::map<unsigned int, ManipEventTypeSP, std::less<unsigned int> > BaseType;
   typedef typename BaseType::iterator Iter;
   typedef typename BaseType::const_iterator CIter;
   
   using BaseType::find;
   using BaseType::end;
   using BaseType::clear;

   EventMap() : BaseType() {}
   EventMap(const EventMap &em) : BaseType(em) {}
   virtual ~EventMap() {}
   EventMap &operator=(const EventMap &em)
   {
      BaseType::operator=(em);
      return *this;
   }

   /////////////////////////////////////////////////////
   ///@name [] Accessors
   ///@{

   /// const, returns a pointer to the event, only if it 
   /// is mapped!  This is good since we don't want to add 
   /// an event just to check if it exists!!
   ManipEventTypeSP operator[](unsigned int key) const 
   {
      CIter ei = find(key);
      if( ei == end() ) return ManipEventTypeSP(0);
      return (*ei).second;
   }

   /// standard non-const, same as the map [] operator
   ManipEventTypeSP &operator[](unsigned int key)
   { 
      return BaseType::operator[](key);
   }
   ///@}
   /////////////////////////////////////////////////////

   /////////////////////////////////////////////////////
   ///@name Event Interface
   ///@{
   ManipEventTypeSP getEvent(unsigned int button) const
   { 
      return operator[]( button );
   }
   
   void             mapEvent(unsigned int button, ManipEventType *met)
   {
      operator[](button) = met;
   }
   ///@}
   /////////////////////////////////////////////////////

   /////////////////////////////////////////////////////
   ///@name Event Interface
   ///@{
   bool mouse(ManipType *m, unsigned int button, const gutz::MouseEvent &me)
   {
      if( ! (_lastEvent = getEvent(button)) ) return false;
      //if( !me.isButtonDown() ) return false;
      return _lastEvent->startEvent(m, me);
   }
   bool move(ManipType *m, const gutz::MouseMoveEvent &mme)
   {
      if( (!_lastEvent) ) return false;
      return _lastEvent->handleEvent(m, mme);
   }
   void endEvent(ManipType *m, const gutz::MouseEvent &me)
   {
      if( _lastEvent ) _lastEvent->endEvent(m, me);
   }
   bool tumble(ManipType *m)
   {
      if( ! _lastEvent ) return false;
      return _lastEvent->tumble(m);
   }
   ///@}
   /////////////////////////////////////////////////////

   /////////////////////////////////////////////////////
   ///@name Event Management
   ///@{

   /// nuke all events
   void clearEvents() { clear(); }
   /// deep copy of events from some other map. Will likely be
   /// used like:
   /// \code
   ///   myManip.cloneEvents( someOtherManip );
   /// \endcode
   /// since Manips are (sub-classes of) EventMaps 
   void cloneEvents(const EventMap &em)
   {
      for(CIter ei = em.begin(); ei != em.end(); ++ei)
      {
         if( (*ei).second )
            mapEvent((*ei).first, (*ei).second->clone());
      }
   }
   /// shallow copy of events from some other map
   void shareEvents(const EventMap &em)
   {   
      for(CIter ei = em.begin(); ei != em.end(); ++ei)
      {
         if( (*ei).second )
            mapEvent((*ei).first, (*ei).second);
      }
   }
   ///@}
   /////////////////////////////////////////////////////

protected:
   ManipEventTypeSP _lastEvent;

};


} // end namespace gutz

#endif

