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
///////////////////////////////////////////////////////////////////////////

//mouseEvent.h

#ifndef __GUTZ_MOUSE_EVENT_DOT_H
#define __GUTZ_MOUSE_EVENT_DOT_H

#include <mathGutz.h>
#include "gutzKeyMouse.h"
#include "cameraEvent.h"

namespace gutz {

///////////////////////////////////////////////////////////////////////////
/// MouseEvent, the mouse went down or up...
///   Don't forget, part of this interface is in the 
///   CameraEvent.
///////////////////////////////////////////////////////////////////////////

class MouseEvent : public CameraEvent {
public:
   typedef std::vector<unsigned int> DataVec;

   /// standard init. camera and manip are optional, but
   ///  they proabaly should be required!
   MouseEvent( const vec3f &pos, bool down, unsigned int button,
               const CameraSP camera=0, const ManipSP manip=0)
     : CameraEvent(camera,manip), _button(button),  _down(down), _pos(pos)
   {setWorldPos(); setButtonDown(down);}
   /// init with data vec
   MouseEvent(const vec3f &pos, bool down, unsigned int button, 
              const DataVec &dv,
              const CameraSP camera=0, const ManipSP manip=0)
     : CameraEvent(camera,manip), pickData(dv),_button(button), _down(down),_pos(pos)
   {setWorldPos(); setButtonDown(down);}
   /// init with data vec from unsigned int*
   MouseEvent(const vec3f &pos, bool down, unsigned int button, 
              const unsigned int *dv, int dataSize,
              const CameraSP camera=0, const ManipSP manip=0)
      : CameraEvent(camera,manip), _button(button), _down(down), _pos(pos)
   {
      setWorldPos();
      setButtonDown(down);
      pickData.resize(dataSize);
      for(int i=0; i<dataSize; ++i)
         pickData[i] = dv[i];
   }
   MouseEvent( const MouseEvent &me )
     : CameraEvent( me ), 
     pickData(me.pickData), _button(me._button), _down(me._down), _pos(me._pos) 
       {
	 setWorldPos();
       }
   /// using default copy and assignment
   virtual ~MouseEvent() {}

   ////////////////////////////////////////////////////////////
   /// @name GUTZ_MOUSE_BUTTON <gutzKeyMouse.h>
   ///@{
   unsigned int   getButton() const         { return _button; }
   void           setButton(unsigned int b) { _button = b; }
   ///@}

   ////////////////////////////////////////////////////////////
   /// @name is the button down or up?
   ///@{
   bool           isButtonDown() const    { return _down; }
   void           setButtonDown(bool yes) 
   { 
      _down = yes; 
      if(yes){ _button = _button & (~GUTZ_MOUSE_UP); }
      else   { _button |= GUTZ_MOUSE_UP;             }   
   }
   ///@}

   ////////////////////////////////////////////////////////////
   /// @name current SCREEN SPACE position
   ///@{
   float        x()      const           { return _pos.x(); }
   float        y()      const           { return _pos.y(); }
   float        z()      const           { return _pos.z(); }
   vec3f        getPos() const           { return _pos; }
   virtual void setPos(const vec3f &pos) { _pos = pos; setWorldPos(); }
   /// this is convenient since the z pos can't be known till
   /// something is "picked", unless you set it (z) to zero which
   /// indicates picking on the "near clip", or image plane.
   ///  usually, z is in the range [0,1]: 0 =near clip, 1 =far clip
   void         setZ(const float zp)     { _pos.z() = zp; setWorldPos(); }
   ///@}

   ////////////////////////////////////////////////////////////
   ///@name current WORLD SPACE position
   ///@{
   vec3f        getWorldPos() const      { return _wpos; }
   ///@}

   ////////////////////////////////////////////////////////////
   /// @name Pick Data.
   ///@{
   /// public data
   /// data/ids associated with mouse event, currently used for
   ///  gl style picking name ids.
   DataVec  pickData;

   void resetPickData()
   {
      pickData.resize(0);
   }
   void setPickData(const DataVec &dv)
   {
      pickData.resize(dv.size());
      for(int i=0; i< int(dv.size()); ++i)
         pickData[i] = dv[i];
   }
   void setPickData(const unsigned int *dv, int dvSize)
   {
      pickData.resize(dvSize);
      for(int i=0; i<dvSize; ++i)
         pickData[i] = dv[i];
   }
   ///@}
   ////////////////////////////////////////////////////////////

protected:
   /// set the world pos based on the screen pos
   void setWorldPos() 
   {
      if(!_cam)
      {
         _wpos = _pos;
         return;
      }
      _wpos = _cam->getPickPos(_pos);
   }

   MouseEvent(); /// not used
   unsigned int _button;
   bool         _down;
   gutz::vec3f  _pos;  ///< screen space postion
   gutz::vec3f  _wpos; ///< world space position
};

typedef gutz::SmartPtr<MouseEvent> MouseEventSP;

///////////////////////////////////////////////////////////////////////////
/// MouseMoveEvent, the mouse is moving.  
///   Be sure to look up the hierarchy for more interface stuff... 
///////////////////////////////////////////////////////////////////////////

class MouseMoveEvent : public MouseEvent {
public:
   MouseMoveEvent(const vec3f &pos, const vec3f &last,
                  bool down = true, unsigned int button = gutz::GUTZ_BUTTON_NONE,
                  const CameraSP cam=0, const ManipSP manip=0)
      : MouseEvent(pos, down, button, cam, manip), _last(last)
   {setWorldLast();}
   /// for initallizing from a preceeding MouseEvent
   MouseMoveEvent(const MouseEvent &me, const vec3f &curPos)
      : MouseEvent(curPos, me.isButtonDown(), me.getButton(), me.pickData, 
                   me.getCamera(), me.getManip()),
        _last(me.getPos())
   {setWorldLast();}
  MouseMoveEvent( const MouseMoveEvent &mme )
    : MouseEvent( mme ), _last(mme._last), _wlast(mme._wlast)
    { setWorldLast();}

   /// using default copy and assignment
   virtual ~MouseMoveEvent() {}

   /// update current position, copies old pos to last pos, then sets current.
   void setPos(const vec3f &pos)       
   { _last = getPos(); _wlast = getWorldPos(); MouseEvent::setPos(pos); }

   ////////////////////////////////////////////////////////////
   /// @name screen space delta from last move event
   ///@{
   float       dx()     const          { return _pos.x() - _last.x(); }
   float       dy()     const          { return _pos.y() - _last.y(); }
   float       dz()     const          { return _pos.z() - _last.z(); }
   vec3f       getDel() const          { return _pos - _last; }
   ///@}

   /// world space delta.
   vec3f       getWorldDel() const     { return _wpos - _wlast; }

   ////////////////////////////////////////////////////////////
   /// @name screen space position at last move event
   ///@{
   float       lx()     const          { return _last.x(); }
   float       ly()     const          { return _last.y(); }
   float       lz()     const          { return _last.z(); }
   vec3f       getLast()const          { return _last; }
   void        setLast(const vec3f &l) { _last = l; setWorldLast(); }
   ///@}

   /// world space last position
   vec3f       getWorldLast() const          { return _wlast; }

protected:
   void        setWorldLast() 
   {
      if(!_cam)
      {
         _wlast = _last;
         return;
      }
      _wlast = _cam->getPickPos(_last); 
   }


   MouseMoveEvent(); /// not used
   vec3f _last;
   vec3f _wlast;

};

typedef gutz::SmartPtr<MouseMoveEvent> MouseMoveEventSP;

} //< end namespace gutz

#endif


