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

//renderEvent.h

#ifndef __GUTZ_RENDER_EVENT_DOT_H
#define __GUTZ_RENDER_EVENT_DOT_H

#include "cameraEvent.h"
#include "Light.h"

namespace gutz {

class RenderEvent : public CameraEvent {
public:
  RenderEvent()
    : CameraEvent(0,0), _picking(false), _update(false), _displayNum(0)
      {}
  RenderEvent(const CameraSP cam, const ManipSP manip, bool picking = false) 
    : CameraEvent(cam, manip), _picking(picking), _update(false), _displayNum(0)
    {}
   virtual ~RenderEvent() {}
   RenderEvent( const RenderEvent &re )
     : CameraEvent( re ), _lights( re._lights), _picking( re._picking ), _update(re._update), _displayNum(re._displayNum)
   {}
   RenderEvent &operator=( const RenderEvent &re )
   {
      CameraEvent::operator =(re);
      _picking    = re._picking;
      _lights     = re._lights;
      _update     = re._update;
      _displayNum = re._displayNum;
      return *this;
   }

   bool picking() const      { return _picking; }
   void setPicking(bool yes) { _picking = yes; }

   /// request that the scene be redrawn
   void requestUpdate() const { _update = true; }
   bool getUpdate() const { return _update; }
   void setUpdate(bool yesno) { _update = yesno; }

   /// Set the display number
   void setDisplayNum(int displayNum) { _displayNum = displayNum; }
   
   /// Get the display number
   int getDisplayNum(void) const { return _displayNum; }
   
   LightVec  _lights;

protected:
   bool _picking;
   mutable bool _update;  ///< some renderable requested a redraw
   int _displayNum;

};

typedef gutz::SmartPtr<RenderEvent> RenderEventSP;

} //< end namespace gutz


#endif


