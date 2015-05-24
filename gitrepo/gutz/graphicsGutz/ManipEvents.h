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

//ManipEvents.h
// Joe Kniss

#ifndef __MANIPULATOR_EVENTS_DOT_H
#define __MANIPULATOR_EVENTS_DOT_H

#include "Manip.h"
#include "ManipEventBase.h"

namespace gutz {

typedef ManipEventBase<Manip>        ManipEvent;
typedef gutz::SmartPtr< ManipEvent > ManipEventSP;

///////////////////////////////////////////////////////////////////////////
/// Rotate Manip Event.  Rotates a manipulator about its center of
/// rotation.  
///////////////////////////////////////////////////////////////////////////
class RotateManipEvent : public ManipEventBase<Manip> {
public:
   RotateManipEvent(float speed = 1.0f)
      : ManipEventBase<Manip>(speed), _tumbquat(quatf_id) {}
   RotateManipEvent(const RotateManipEvent &mre)
      : ManipEventBase<Manip>(mre), _tumbquat(mre._tumbquat)
   {}
   virtual ~RotateManipEvent() {}

   RotateManipEvent &operator=(const RotateManipEvent &mre)
   { ManipEventBase<Manip>::operator =(mre); return *this; 
     _tumbquat = mre._tumbquat;
   }

   virtual ManipEventBase<Manip> *clone() const { return new RotateManipEvent(*this); }

   ///@name Event Handlers
   ///@{

   /// should be called on a Mouse-down event, starts the event
   bool startEvent(Manip *m, const MouseEvent &me);
   /// should be called when the mouse moves
   bool handleEvent(Manip *m, const MouseMoveEvent &mme);
   /// called when the event is over, may or may not be on a Mouse-up event
   void endEvent(Manip *m, const MouseEvent &me);
   /// repeat last event
   bool tumble(Manip *m);
   ///@}

protected:
   float _rad;
   quatf _tumbquat;
};

typedef gutz::SmartPtr<RotateManipEvent> RotateManipEventSP;

///////////////////////////////////////////////////////////////////////////
/// Rotate Axis Manip Event.  Rotates a manipulator about a prescribed axis.  
///////////////////////////////////////////////////////////////////////////
class RotateAxisManipEvent : public ManipEventBase<Manip> {
public:
 RotateAxisManipEvent(const vec3f &axis = vec3f_x, float speed = 1.0f)
   : ManipEventBase<Manip>(speed), _axis(axis), _rad(0), _tumbquat(quatf_id)
  {}
 RotateAxisManipEvent(const RotateAxisManipEvent &mre)
   : ManipEventBase<Manip>(mre), 
    _axis(mre._axis), _rad(0), _tumbquat(mre._tumbquat)
    {}
  virtual ~RotateAxisManipEvent() {}
  
  RotateAxisManipEvent &operator=(const RotateAxisManipEvent &mre)
    { ManipEventBase<Manip>::operator =(mre);
      _axis = mre._axis;
      _rad = mre._rad;
      _tumbquat = mre._tumbquat;
      return *this; 
    }
  
  virtual ManipEventBase<Manip> *clone() const 
  { return new RotateAxisManipEvent(*this); }

  ///@name Event Handlers
  ///@{
  
  /// should be called on a Mouse-down event, starts the event
  bool startEvent(Manip *m, const MouseEvent &me);
  /// should be called when the mouse moves
  bool handleEvent(Manip *m, const MouseMoveEvent &mme);
  /// called when the event is over, may or may not be on a Mouse-up event
  void endEvent(Manip *m, const MouseEvent &me)
  {
    _tumbquat = quatf_id;
  }
   /// repeat last event
  bool tumble(Manip *m);
  ///@}
  
 protected:
  vec3f _axis;
  float _rad;
  quatf _tumbquat;
};

typedef gutz::SmartPtr<RotateAxisManipEvent> RotateAxisManipEventSP;

///////////////////////////////////////////////////////////////////////////
/// Translate XY Manip Event.  Translates the manipulator in X&Y relative
///  to the current view. This is also a base class for other translation
///  events.
///////////////////////////////////////////////////////////////////////////

class TransXYManipEvent : public ManipEventBase<Manip> {
public:
   TransXYManipEvent(float speed = 1.0f)
      : ManipEventBase<Manip>(speed), _lastTrans(vec3f_zero)
   {}
   TransXYManipEvent(const TransXYManipEvent &tme)
      : ManipEventBase<Manip>(tme), _lastTrans(vec3f_zero)
   {}
   virtual ~TransXYManipEvent() {}

   TransXYManipEvent &operator=(const TransXYManipEvent &tme)
   {  ManipEventBase<Manip>::operator =(tme); 
      _lastTrans = vec3f_zero;
      return *this; 
   }

   /// be sure to override this in any sub-classes
   virtual ManipEventBase<Manip> *clone() const { return new TransXYManipEvent(*this); }

   ///@name Event Handlers
   ///@{

   /// should be called on a Mouse-down event, starts the event
   bool startEvent(Manip *m, const MouseEvent &me);
   /// should be called when the mouse moves
   bool handleEvent(Manip *m, const MouseMoveEvent &mme);
   /// called when the event is over, may or may not be on a Mouse-up event
   void endEvent(Manip *m, const MouseEvent &me);
   /// repeat last event
   bool tumble(Manip *m);
   ///@}

protected:
   /// returns the translaton for this local frame (actually defined in parent's space)
   vec3f getDelta(Manip *m, const MouseMoveEvent &mme) const;

   vec3f _lastTrans;
};

typedef gutz::SmartPtr<TransXYManipEvent> TransXYManipEventSP;

///////////////////////////////////////////////////////////////////////////
/// Translate Z Manip Event.  Translates the manipulator in Z relative
///  to the current view (along the view direction)
///////////////////////////////////////////////////////////////////////////

class TransZManipEvent : public TransXYManipEvent {
public:
   TransZManipEvent(float speed = 1.0f)
      : TransXYManipEvent(speed)
   {}
   TransZManipEvent(const TransZManipEvent &tme)
      : TransXYManipEvent(tme)
   {}
   virtual ~TransZManipEvent() {}

   TransZManipEvent &operator=(const TransZManipEvent &tme)
   {  TransXYManipEvent::operator =(tme); 
      return *this; 
   }

   virtual ManipEventBase<Manip> *clone() const { return new TransZManipEvent(*this); }

   ///@name Event Handlers
   ///@{
   /// should be called when the mouse moves
   bool handleEvent(Manip *m, const MouseMoveEvent &mme);
   ///@}

protected:
};

typedef gutz::SmartPtr<TransZManipEvent> TransZManipEventSP;

///////////////////////////////////////////////////////////////////////////
/// Translate in Plane.  Translates only in a user-defined plane.  
///  This class "HAS_SLOTS" so that the plane can be defined dynamically.
///  See gutz::Signal for more info on Signals & Slots.  The plane is 
///  defined in WORLD space.
///////////////////////////////////////////////////////////////////////////

class TransPlaneManipEvent : public TransXYManipEvent {
public:
   HAS_SLOTS;

   TransPlaneManipEvent(gutz::vec3f ppos = vec3f_zero, 
                        gutz::vec3f pnorm = vec3f_z, 
                        float speed = 1.0f)
                        : TransXYManipEvent(speed), _ppos(ppos), _pnorm(pnorm)
   {}
   TransPlaneManipEvent(const TransPlaneManipEvent &tpme)
      : TransXYManipEvent(tpme), _ppos(tpme._ppos), _pnorm(tpme._pnorm)
   {}
   virtual ~TransPlaneManipEvent() {}

   TransPlaneManipEvent &operator=(const TransPlaneManipEvent &tpme)
   {
      TransXYManipEvent::operator =(tpme);
      _ppos = tpme._ppos;
      _pnorm = tpme._pnorm;
      return *this;
   }

   virtual ManipEventBase<Manip> *clone() const { return new TransPlaneManipEvent(*this); }

   ///////////////////////////////
   ///@name Set/Get Plane 
   ///   The plane should be set in WORLD space 
   ///@{
   void setPlane(const gutz::planef &pln)
   {  _ppos = pln.p; _pnorm = pln.n; }
   void        setPlanePos( const gutz::vec3f &ppos)   { _ppos = ppos; }
   gutz::vec3f getPlanePos() const                     { return _ppos; }
   void        setPlaneNorm( const gutz::vec3f &pnorm) { _pnorm = pnorm; }
   gutz::vec3f getPlaneNorm()const                     { return _pnorm; }
   ///@}
   ///////////////////////////////

   ///@name Event Handlers
   ///@{
   bool startEvent(Manip *m, const MouseEvent &me);
   /// should be called when the mouse moves
   bool handleEvent(Manip *m, const MouseMoveEvent &mme);
   ///@}

protected:
   gutz::vec3f _start;
   gutz::vec3f _ppos;
   gutz::vec3f _pnorm;

};

typedef gutz::SmartPtr<TransPlaneManipEvent> TransPlaneManipEventSP;

///////////////////////////////////////////////////////////////////////
/// Translate along Line
///    Constrains interaction to be along a line defined by 2 points, 
///      could be dynamically defined
///////////////////////////////////////////////////////////////////////

class TransLineManipEvent : public ManipEventBase<Manip>
{
 public:
  HAS_SLOTS;
  typedef ManipEventBase<Manip> BaseType;

  TransLineManipEvent(bool clampToEnds = false) 
    : _lstart( gutz::vec3f_zero ), _lend( gutz::vec3f_x ), _clamp(clampToEnds)
    {}
  TransLineManipEvent( const TransLineManipEvent &lm )
    : BaseType( lm ), _lstart( lm._lstart ), _lend( lm._lend ), 
    _clamp( lm._clamp)
      {}

  TransLineManipEvent &operator=(const TransLineManipEvent &lm)
    {
      BaseType::operator=(lm);
      _lstart = lm._lstart;
      _lend = lm._lend;
      _clamp = lm._clamp;
      return *this;
    }

  virtual TransLineManipEvent *clone() const 
  { return new TransLineManipEvent(*this); }

  ///@name Line Point set/get
  ///@{
  gutz::vec3f getStartPoint() const { return _lstart; }
  void setStartPoint(const gutz::vec3f start) { _lstart = start; }
  gutz::vec3f getEndPoint() const { return _lend; }
  void setEndPoint(const gutz::vec3f end)     { _lend = end; }
  ///@}

  ///@name Event Handlers
  ///@{
  bool startEvent(Manip *m, const MouseEvent &me);
  /// should be called when the mouse moves
  bool handleEvent(Manip *m, const MouseMoveEvent &mme);
  /// event is over
  void endEvent(Manip *m, const MouseEvent &me);
  ///@}

  ///@name Clamp to endpoints of line?
  ///@{
  bool getClamp() const { return _clamp; }
  void setClamp(bool yesNo) { _clamp = yesNo; }
  ///@}

 protected:
  gutz::vec3f _lstart;
  gutz::vec3f _lend;
  bool _clamp;

};

 typedef gutz::SmartPtr<TransLineManipEvent> TransLineManipEventSP;

///////////////////////////////////////////////////////////////////////////
/// Scale in the X Manip Event.  Scales the manipulator in X
/// The clamp value clamps the scale to clamp (default zero) not allowing negative scales
///////////////////////////////////////////////////////////////////////////

class ScaleXManipEvent : public TransXYManipEvent {
public:
   ScaleXManipEvent(float speed = 0.01f, float clamp = 1.f)
      : TransXYManipEvent(speed), _clamp(clamp)
   {}
   ScaleXManipEvent(const ScaleXManipEvent &tme)
      : TransXYManipEvent(tme)
   {}
   virtual ~ScaleXManipEvent() {}

   ScaleXManipEvent &operator=(const ScaleXManipEvent &tme)
   {  TransXYManipEvent::operator =(tme); 
      return *this; 
   }

   virtual ManipEventBase<Manip> *clone() const { return new ScaleXManipEvent(*this); }

   ///@name Event Handlers
   ///@{
   /// should be called when the mouse moves
   bool handleEvent(Manip *m, const MouseMoveEvent &mme);
   ///@}

protected:
  float _clamp;

};

typedef gutz::SmartPtr<ScaleXManipEvent> ScaleXManipEventSP;


///////////////////////////////////////////////////////////////////////////
/// Scale in the X and Y Manip Event.  Scales the manipulator in X and Y uniformly
/// The clamp value clamps the scale to clamp (default zero) not allowing negative scales
///////////////////////////////////////////////////////////////////////////

class UniformScaleXYManipEvent : public TransXYManipEvent {
public:
   UniformScaleXYManipEvent(float speed = 0.01f, float clamp = 1.f)
      : TransXYManipEvent(speed), _clamp(clamp)
   {}
   UniformScaleXYManipEvent(const UniformScaleXYManipEvent &tme)
      : TransXYManipEvent(tme)
   {}
   virtual ~UniformScaleXYManipEvent() {}

   UniformScaleXYManipEvent &operator=(const UniformScaleXYManipEvent &tme)
   {  TransXYManipEvent::operator =(tme); 
      return *this; 
   }

   virtual ManipEventBase<Manip> *clone() const { return new UniformScaleXYManipEvent(*this); }

   ///@name Event Handlers
   ///@{
   /// should be called when the mouse moves
   bool handleEvent(Manip *m, const MouseMoveEvent &mme);
   ///@}

protected:
  float _clamp;

};

typedef gutz::SmartPtr<UniformScaleXYManipEvent> UniformScaleXYManipEventSP;



} /// end namespace gutz


#endif


