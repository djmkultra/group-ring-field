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

//cameraEvent.h

#ifndef __GUTZ_CAMERA_EVENT_DOT_H
#define __GUTZ_CAMERA_EVENT_DOT_H


#include <smartptr.h>
#include <Manip.h>
#include <Camera.h>

namespace gutz {
   
   
   class CameraEvent : public gutz::Counted
   {
   public:
      typedef gutz::SmartPtr<CameraEvent> ptr;
      typedef std::vector<ptr>            vec;
      typedef std::vector<ptr>::iterator  vec_itr;
      
      CameraEvent(const CameraSP camera, const ManipSP manipulator,
                  const double time=0, const double lastTime=0)
      : _cam(camera), _manip(manipulator), _time(time), _lastTime(lastTime)
      {}
      
      virtual ~CameraEvent() { _cam = 0; _manip = 0; }
      
      CameraEvent( const CameraEvent &ce )
      : _cam( ce._cam ), _manip( ce._manip ), 
      _time(ce._time), _lastTime(ce._lastTime)
      {}
      
      CameraEvent &operator=(const CameraEvent &ce)
      {
         _cam = ce._cam;
         _manip = ce._manip;
         _time = ce._time;
         _lastTime = ce._lastTime;
         return *this;
      }
      
      const CameraSP getCamera() const       { return _cam; }
      void           setCamera(CameraSP &cam) { _cam = cam; }
      const ManipSP  getManip() const        { return _manip; }
      void           setManip(ManipSP &manip) { _manip = manip; }
      
      double         getTime() const { return _time; }
      void           setTime(double time) { _time = time; }
      double         getLastTime() const { return _lastTime; }
      void           setLastTime(double lastTime) { _lastTime = lastTime; }
      
   protected:
      CameraSP _cam;
      ManipSP  _manip;
      
      double    _time;
      double    _lastTime;
      
   };
   
   typedef gutz::SmartPtr<CameraEvent> CameraEventSP;
   
} //< end namespace gutz

#endif


