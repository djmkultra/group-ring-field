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

//Camera.cpp
//Joe Kniss

//#pragma warning(disable:4786) //dissable stl identifier too long warnings

#include "Camera.h"
#include <vectorGraphics.h>
#include <iostream>
#include <math.h>
// TODO: fix this!
//#include <serialize/SerializeUtil.h>
#include "../eventGutz/mouseEvent.h"

#ifdef KONG
#include <KokoBuffer.h>
#endif

#ifndef M_PI
#define M_PI 3.141592654f
#endif

#define m_PI 3.141592654f


using namespace gutz;
using namespace std;

//=========================================================================
//  Construction / Destruction
//=========================================================================
/// Construtor intiallizes class reasonably, but needs window dimensions. 
///
/// Intiallized values: Near Clip = 1.0, Far Clip = 100.0, Win size = (0,0),
/// Camera Matrix = identity, Object Matrix = identity, Eye pos = (0,0,20),
/// Lookat = (0,0,0), Up = (0,1,0), Frustum = (-1,1,-1,1), 
/// Left Mouse = Object rotation, Right mouse = Dolly
///
Camera::Camera()
:   BaseManip(),
    _screen(512,512),
    _clips(1.0f, 1000.0f),
    _cm(mat4f_id),
    _pm(mat4f_id),
    _eyePos(0,0,20),
    _lookat(0,0,0),
    _upvec(0,1,0),
    _camquat(0,0,0,1),
    _camtrackquat(0,0,0,1),
    _swivtrackquat(0,0,0,1),
    _ortho(false)
{

   // Default setup for camera =
   //_keymap[GUTZ_LEFT_MOUSE]   = CAM_ROT;
   //_speedmap[GUTZ_LEFT_MOUSE] = 1.0;

   //_keymap[GUTZ_RIGHT_MOUSE] = CAM_DOLLY;
   //_speedmap[GUTZ_RIGHT_MOUSE] = 0.02f;

   _setViewMatrix(look_at(_eyePos, _lookat, _upvec));
   setFrustum(-.5, .5, -.5, .5);

}

Camera::Camera(const Camera &cam)
:
   BaseManip(cam),
   _screen(cam._screen),
   _clips(cam._clips),
   _cm(cam._cm),
   _pm(cam._pm),
   _eyePos(cam._eyePos),
   _lookat(cam._lookat),
   _upvec(cam._upvec),
   _camquat(cam._camquat),
   _camtrackquat(cam._camtrackquat),
   _swivtrackquat(cam._swivtrackquat),
   _ortho(cam._ortho),
   _keymap(cam._keymap),
   _speedmap(cam._speedmap)
{

}


Camera::~Camera()
{

}


//=========================================================================
//  Get Event
//=========================================================================
unsigned int  Camera::getEvent(const MouseEvent &me) const
{
   if(!me.isButtonDown()) return 0;
   return _keymap[me.getButton()];
}

//=========================================================================
//  Get Speed
//=========================================================================
float         Camera::getSpeed(const MouseEvent &me) const
{
   return _speedmap[me.getButton()];
}


//=========================================================================
//  Set Matricies
//=========================================================================

/// get the viewport matrix
mat4f Camera::getViewportMatrix() const
{
   return mat4f(_screen[0]/2.0f, 0, 0, 0, 
                0, _screen[1]/2.0f, 0, 0,
                0, 0, .5, 0,
                _screen[0]/2.0f, 
                _screen[1]/2.0f,
                .5, 1);
}

mat4f Camera::getInvViewportMatrix() const
{
   return mat4f(2.0f/_screen[0], 0, 0, 0,
                0, 2.0f/_screen[1], 0, 0,
                0, 0, 2.0f, 0,
                -1, -1, -1, 1);
}

/// external set projection
void Camera::setProjectMatrix(const mat4f &m)
{
   _setProjectMatrix(m);
}

/// internal set projection
void Camera::_setProjectMatrix(const mat4f &m)
{
   _pm = m;
   _pinv = _pm.inv();
}

/// external set view, takes a guess at eye/at/up but probably a bad one
void Camera::setViewMatrix(const mat4f &m)
{
   _setViewMatrix(m);
   mat4f inv = getViewMatrix().inv();
   vec4f eye    = inv * vec4f(0.0, 0.0,  0.0, 1.0);
   vec4f up     = inv * vec4f(0.0, 1.0,  0.0, 1.0) - eye;
   vec4f lookat = inv * vec4f(0.0, 0.0, -1.0, 1.0);
   
   _eyePos =  vec3f(   eye.x(),    eye.y(),    eye.z());
   _upvec  =  vec3f(    up.x(),     up.y(),     up.z());
   _lookat =  vec3f(lookat.x(), lookat.y(), lookat.z());
   _camquat.set(_cm.rot());
}

/// internal set view
void Camera::_setViewMatrix(const mat4f &m)
{
   _cm = m;
   _cinv = _cm.inv();
}



////////////////////////////////////////////////////////////
// Serialization.
// write.
const string DATA_STR("Data{");
const string END_SECTION_STR("\n}\n");
const string EVENT_MAP_STR("EventMap{");
const string PARAM_MAP_STR("ParamMap{");

//=========================================================================
//  Uber Serialize
//=========================================================================
bool Camera::serialize(std::ostream &os, const std::string indent)
{
   string ind = indent;
   ind += "   ";

   serialState(os,indent);

   /// serial data
   os << indent << DATA_STR << " \n";
   serialData(os, ind);
   os << indent << END_SECTION_STR;

   bool status = false;

   return status;

}

//=========================================================================
//  Uber Unserialize
//=========================================================================
// read.
bool Camera::unserialize(std::istream &is)
{
   /// TODO: fix this!
#if 0
   string tstr;

   bool status = false;

   unserialState(is);

   /// unserial data
   is >> tstr;
   if(serial::unserialKeyCheck(DATA_STR, tstr, "baseManip, data")) return true;   
   if(unserialData(is)) return true;
   is >> tstr;
   if(serial::unserialKeyCheck(END_SECTION_STR,tstr,"baseManip, data")) return true;

   status = status || is.eof() || is.fail();
   return status;
#endif
   return true;
}

//=========================================================================
//  Serialize State
//=========================================================================
bool Camera::serialState(std::ostream &os, const std::string indent)
{
   /// TODO: fix this
#if 0
   string ind = indent;
   ind += "   ";
   
   /// serial event map
   os << indent << EVENT_MAP_STR << " \n";
   serialEventMap(os, ind);
   os << indent << END_SECTION_STR;

   /// serial param map
   os << indent << PARAM_MAP_STR << " \n";
   serialParamMap(os, ind);
   os << indent << END_SECTION_STR;

   return false;
#endif
   return true;
}

//=========================================================================
//  UnSerialize State
//=========================================================================
bool Camera::unserialState(std::istream &is)
{
   /// TODO: fix this
#if 0
   string tstr;
   
   /// unserial event map
   is >> tstr;
   if(serial::unserialKeyCheck(EVENT_MAP_STR, tstr, "BaseManip, event map")) return true;
   if(unserialEventMap(is)) return true;
   is >> tstr;
   if(serial::unserialKeyCheck(END_SECTION_STR, tstr, "BaseManip, event map")) return true;

   /// unserial param map
   is >> tstr;
   if(serial::unserialKeyCheck(PARAM_MAP_STR, tstr, "BaseManip, param map")) return true;
   if(unserialParamMap(is)) return true;
   is >> tstr;
   if(serial::unserialKeyCheck(END_SECTION_STR, tstr, "BaseManip, param map")) return true;

   return is.eof() || is.fail();   
#endif
   return true;
}

//=========================================================================
//  Serialize Event Map
//=========================================================================
bool Camera::serialEventMap(std::ostream &os, const std::string indent)
{
   /// TODO: fix this!
#if 0
   os << indent << _keymap.size() << " \n";
   EventKeyMap::iterator mi = _keymap.begin();
   while(mi != _keymap.end())
   {
      os << indent << (*mi).first << " " << (*mi).second << " ";
      ++mi;
   }
   return false;
#endif
   return true;
}

//=========================================================================
//  Serialize Param Map
//=========================================================================
bool Camera::serialParamMap(std::ostream &os, const std::string indent)
{
   /// TODO: fix this
#if 0
   os << indent << _keymap.size() << " \n";
   EventParamMap::iterator mi = _speedmap.begin();
   while(mi != _speedmap.end())
   {
      os << indent << (*mi).first << " " << (*mi).second << " ";
      ++mi;
   }
   return false;
#endif
   return true;
}

//=========================================================================
//  Unserialize Event Map
//=========================================================================
bool Camera::unserialEventMap(std::istream &is)
{
   /// TODO: fix this
#if 0
   _keymap = EventKeyMap();
   
   int keymapsize;
   is >> keymapsize;
   for(int i=0; i<keymapsize; ++i)
   {
      EventKeyMap::value_type mi;
      int key, value;
      is >> key;
      is >> value;
      _keymap[key] = value;
   }  
   return is.eof() || is.fail();
#endif
   return true;
}

//=========================================================================
//  Unserialize Param Map
//=========================================================================
bool Camera::unserialParamMap(std::istream &is)
{
   /// TODO: fix this
#if 0
   _speedmap = EventParamMap();
   
   int parammapsize;
   is >> parammapsize;
   for(int i=0; i<parammapsize; ++i)
   {
      EventParamMap::value_type mi;
      int key;
      float value;
      is >> key;
      is >> value;
      _speedmap[key] = value;
   }  
   return is.eof() || is.fail();
#endif
   return true;
}

//=========================================================================
//  Serialize Data
//=========================================================================
// these, the frustum (baseManip), and clips (baseManip) are all we 
// really need to save.
const string EYE_POS_STR("EyePos: ");
const string LOOK_AT_STR("LookAt: ");
const string UP_VEC_STR("UpVector: ");

bool Camera::serialData(std::ostream &os, const std::string indent)
{
   BaseManip::serialData(os,indent);
   
   os << indent << EYE_POS_STR << _eyePos << "\n";
   os << indent << LOOK_AT_STR << _lookat << "\n";
   os << indent << UP_VEC_STR  << _upvec  << "\n";
   
   return false;
}

//=========================================================================
//  UnSerialize Data
//=========================================================================
bool Camera::unserialData(std::istream &is)
{
   /// TODO: fix this
#if 0
   if(BaseManip::unserialData(is)) return true;
   
   string tstr;
   
   /// Eye
   is >> tstr;
   if(serial::unserialKeyCheck(EYE_POS_STR, tstr, "Camera, eyepos")) return true;
   is >> _eyePos;
   
   /// AT
   is >> tstr;
   if(serial::unserialKeyCheck(LOOK_AT_STR, tstr, "Camera, lookat")) return true;
   is >> _lookat;

   /// Up
   is >> tstr;
   if(serial::unserialKeyCheck(UP_VEC_STR, tstr, "Camera, upvector")) return true;
   is >> _upvec;

   setEyePos(_eyePos);
   setLookAt(_lookat);
   setUpVec(_upvec);
   setFrustum(_frustum[FRUST_LEFT], _frustum[FRUST_RIGHT], 
              _frustum[FRUST_BOTTOM], _frustum[FRUST_TOP]);

   return is.eof() || is.fail();
#endif
   return true;
}


//=========================================================================
//  Clipping planes
//=========================================================================

void  Camera::setClips(float nearClip, float farClip)
{
   _clips = vec2f(nearClip, farClip);
   setFrustum(_frustum[FRUST_LEFT],   _frustum[FRUST_RIGHT],
              _frustum[FRUST_BOTTOM], _frustum[FRUST_TOP]);
}

float Camera::getNearClip() const 
{
   return _clips[0];
}

float Camera::getFarClip() const
{
   return _clips[1];
}

//=========================================================================
//  Screen Size
//=========================================================================
void  Camera::setScreen(unsigned int x, unsigned int y)
{
   _screen = vec2ui(x,y);
}

//=========================================================================
//  The LOOK AT : Eye Position, look at, up vector
//=========================================================================
void Camera::setEyePos(vec3f eyePos)
{
   _eyePos=eyePos;
   _setViewMatrix( look_at(_eyePos, _lookat, _upvec) );
}

void Camera::setLookAt(vec3f lookat)
{
   _lookat = lookat;
   _setViewMatrix( look_at(_eyePos, _lookat, _upvec) );
}

void Camera::setUpVec(vec3f upvec)
{
   _upvec = upvec;
   _setViewMatrix( look_at(_eyePos, _lookat, _upvec) );
}

//=========================================================================
//=========================================================================
//  Event Handling
//=========================================================================
//=========================================================================

//=========================================================================
//  Mouse up/down
//=========================================================================
bool Camera::mouse(const gutz::MouseEvent &me)
{
   if( (_currentMode = getEvent(me)) == 0) {
     return false;
   }

   _currentSpeed  = getSpeed(me);
   _camtrackquat  = quatf_id;
   _swivtrackquat = quatf_id;
   _lastEyePos    = _eyePos;
   _lastLookat    = _lookat;

   if(isMouse(me.getButton()))
   {	
      return true;
   }

   else if(isArrow(me.getButton()))
   {
     vec3f pos(_screen[0]/2.0f, _screen[1]/2.0f, 0.0f);
     vec3f last(_screen[0]/2.0f, _screen[1]/2.0f, 0.0f);
      MouseMoveEvent mme(pos, last,
                         me.isButtonDown(), me.getButton());

      if(me.getButton() & GUTZ_UP_ARROW)
      {
         mme.setPos( mme.getPos() - vec3f_y );
      }
      else if(me.getButton() & GUTZ_DOWN_ARROW)
      {
         mme.setPos( mme.getPos() + vec3f_y );
      }
      else if(me.getButton() & GUTZ_RIGHT_ARROW)
      {
         mme.setPos( mme.getPos() + vec3f_x );
      }
      else if(me.getButton() & GUTZ_LEFT_ARROW)
      {
         mme.setPos( mme.getPos() - vec3f_x );
      }

      if( handleCamera(mme) )
      {
         _pinv = _pm.inv();
         _cinv = _cm.inv();
      }
   }

   return false;
}

//=========================================================================
//  Move mouse
//=========================================================================
bool Camera::move(const gutz::MouseMoveEvent &mme)
{
   if( handleCamera(mme) )
   {
      _pinv = _pm.inv();
      _cinv = _cm.inv();
      return true;
   }
   return false;
}

//=========================================================================
//  Set Perspective
//=========================================================================
void Camera::setPerspective(float fov)
{
   _ortho = false;
   _frustum[FRUST_TOP]    = _clips[0] * tanf(fov * m_PI/180.0f * 0.5f);
   _frustum[FRUST_BOTTOM] = -_frustum[FRUST_TOP];  
   _frustum[FRUST_LEFT]   = _frustum[FRUST_BOTTOM] * _screen[0]/(float)_screen[1];
   _frustum[FRUST_RIGHT]  = _frustum[FRUST_TOP] * _screen[0]/(float)_screen[1];

   _setProjectMatrix( frustum(_frustum[FRUST_LEFT],   _frustum[FRUST_RIGHT], 
                              _frustum[FRUST_BOTTOM], _frustum[FRUST_TOP], 
                              _clips[0], _clips[1]) );
}

//=========================================================================
//  Set Frustum
//=========================================================================
void Camera::setFrustum(float left, float right, float bottom, float top)
{
   _ortho = false;
   _frustum[FRUST_LEFT]   = left;
   _frustum[FRUST_RIGHT]  = right;
   _frustum[FRUST_BOTTOM] = bottom;
   _frustum[FRUST_TOP]    = top;

   _setProjectMatrix( frustum(left, right, bottom, top, _clips[0], _clips[1]) );
}

void Camera::setOrtho(float left, float right, float bottom, float top)
{
   _ortho = true;
   _frustum[FRUST_LEFT]   = left;
   _frustum[FRUST_RIGHT]  = right;
   _frustum[FRUST_BOTTOM] = bottom;
   _frustum[FRUST_TOP]    = top;

   _setProjectMatrix( ortho(left,right,bottom,top,_clips[0],_clips[1]) );
}


//=========================================================================
//  Tumble
//=========================================================================
void Camera::tumble(float speed)
{
   if(!_tumble) return;
   if(1 >= _lastd) return;

   if((quatf_id != _camtrackquat) && (CAM_ROT != _currentMode))
   {
      //reset the track quaternion to the right speed
      float thet = _camtrackquat[3];
      thet = acos(thet);
      vec3f axis(_camtrackquat[0]/sin(thet),_camtrackquat[1]/sin(thet),_camtrackquat[2]/sin(thet));
      thet*=speed*2;
      quatf track(thet, axis);

      _camquat = _camquat.mult(track);//_camquatSav;

      mat3f tmpm3(_camquat);          //camera basis matrix
      _upvec = vec3f(tmpm3[3], tmpm3[4], tmpm3[5]); //up
      vec3f tmpv2(tmpm3[6], tmpm3[7], tmpm3[8]);  //Z
      vec3f tmpv = _lastEyePos - _lookat;                 //-view dir
      float tmpf = tmpv.norm();                           //lookat dist
      tmpv2  *= tmpf;                               //new view dir
      _eyePos = tmpv2 + _lookat;                    //new eye point

      _setViewMatrix( look_at(_eyePos, _lookat, _upvec) );
   }

   if((quatf_id != _swivtrackquat) && (CAM_SWIVEL != _currentMode))
   {
      //reset the track quaternion to the right speed
      float thet = _swivtrackquat[3];
      thet = acos(thet);
      vec3f axis(_swivtrackquat[0]/sin(thet),_swivtrackquat[1]/sin(thet),_swivtrackquat[2]/sin(thet));
      thet*=speed*2;
      quatf track(thet, axis);

      _camquat = _camquat.mult(track);//_camquatSav;

      mat3f tmpm3(_camquat);          //camera basis matrix
      _upvec = vec3f(tmpm3[3], tmpm3[4], tmpm3[5]); //up
      vec3f tmpv2(tmpm3[6], tmpm3[7], tmpm3[8]);  //Z

      vec3f tmpv = _lastLookat - _eyePos;                 //-view dir
      float tmpf = tmpv.norm();                           //lookat dist
      tmpv2  *= -tmpf;                               //new view dir
      _lookat = tmpv2 + _eyePos;                    //new lookat point

      _setViewMatrix( look_at( _eyePos, _lookat, _upvec) );
   }
}

//=========================================================================
//  Handle Camera
//=========================================================================
bool Camera::handleCamera(const gutz::MouseMoveEvent &mme)
{
   if(getEvent(mme) == CAM_MODE_NONE) return false;

   bool ret = true;

   switch(getEvent(mme))
   {
      //--------------------------------------------------------
   case CAM_ROT:   //rotate around lookat point
      cameraRot(mme);
      break;
      //--------------------------------------------------------
   case CAM_ZOOM:
      cameraZoom(mme);
      break;
      //--------------------------------------------------------
   case CAM_DOLLY:
      cameraTrans(mme, vec3f_zero, vec3f_z);
      break;
      //--------------------------------------------------------
   case CAM_TRANS_XY:
      cameraTrans(mme, vec3f_x, vec3f_y);
      break;
      //--------------------------------------------------------
   case CAM_TRANS_XZ:
      cameraTrans(mme, vec3f_x, vec3f_z);
      break;
      //--------------------------------------------------------
   case CAM_TRANS_ZY:
      cameraTrans(mme, vec3f_z, vec3f_y);
      break;
      //--------------------------------------------------------
   case CAM_ROLL:
      //rotate about cameras z axis
      cameraRotAxis(mme, vec3f_z, vec3f_zero);
      break;
      //--------------------------------------------------------
   case CAM_PITCH:
      //rotate about cameras x axis
      cameraRotAxis(mme, vec3f_zero, vec3f_x);
      break;
      //--------------------------------------------------------
   case CAM_YAW:
      // rotate about y axis
      cameraRotAxis(mme, vec3f_neg_y, vec3f_zero);
      break;  
      //--------------------------------------------------------
   case CAM_SWIVEL:
      cameraSwivel(mme);
      break;
   default:
      ret = false;
      break;
   }

   /// last delta, for deciding if we should tumble
   _lastd = g_max( g_abs(mme.getDel().x()), g_abs(mme.getDel().y()) );

   return ret;
}


//=========================================================================
//  Interactor functions
//=========================================================================


void Camera::cameraZoom(const gutz::MouseMoveEvent &mme)
{
   /// scale is equal to the total move (dx + dy)
   float scale = (   ( mme.getDel().x()  )/float(_screen[0])  
		     + ( -mme.getDel().y() )/float(_screen[1]) )/2.0f;
 
   zoom( scale + 1 );
}

void Camera::zoom( float scale )
{
  /// compute current frustum's width
   vec2f fwidth(_frustum[FRUST_RIGHT] - _frustum[FRUST_LEFT],
                _frustum[FRUST_TOP] - _frustum[FRUST_BOTTOM]);
   /// compute the center of current frustum
   vec2f fcent(_frustum[FRUST_LEFT]   + fwidth.x()/2.0f,
               _frustum[FRUST_BOTTOM] + fwidth.y()/2.0f );
   /// compute new width (old width * scale) and divide by two
   vec2f fwidthN = fwidth * scale/2.0f;
   
   /// orthographic projection frustum... or:
   if(_ortho)
   {
      setOrtho(fcent.x() - fwidthN.x(), fcent.x() + fwidthN.x(), 
               fcent.y() - fwidthN.y(), fcent.y() + fwidthN.y());
   }
   else /// perspective frustum
   {
      setFrustum(fcent.x() - fwidthN.x(), fcent.x() + fwidthN.x(), 
                 fcent.y() - fwidthN.y(), fcent.y() + fwidthN.y());
   }
}

void  Camera::setZoom( float z )
{
  zoom( z/(_frustum[FRUST_RIGHT] - _frustum[FRUST_LEFT]) );
}

float Camera::getZoom() const
{
  return _frustum[FRUST_RIGHT] - _frustum[FRUST_LEFT];
}

//camera rotate ===========================================================

void Camera::cameraRot(const gutz::MouseMoveEvent &mme)
{
   /// scale and bias pick points (on screen) to (-1,1) range
   ///  pt = (2*pt - sz)/sz
   vec2f pt1(( 2.0f * mme.getLast().x() - _screen[0]) / (float)_screen[0], 
             ( _screen[1] - 2.0f * mme.getLast().y()) / (float)_screen[1]);
   vec2f pt2(( 2.0f * mme.getPos().x()  - _screen[0]) / (float)_screen[0],
             ( _screen[1] - 2.0f * mme.getPos().y() ) / (float)_screen[1]);

   quatf tmpquat = trackball(pt1, pt2, .8f).conj(); //trackball quaternion
   _camtrackquat = tmpquat;                         //tumble amount
   _camquat = _camquat.mult(tmpquat);               //_camquatSav;

   const mat3f tmpm3(_camquat);                     //camera basis matrix
   _upvec = vec3f(tmpm3[3], tmpm3[4], tmpm3[5]);    //upvector changed!
   const vec3f zvec(tmpm3[6], tmpm3[7], tmpm3[8]);  //Z (direction eye<-lookat)
   _eyePos = _lookat + zvec * getFocalLength();     //new eye point

   _setViewMatrix( look_at( _eyePos, _lookat, _upvec) );
}

//camera translate ========================================================

void Camera::cameraTrans(const gutz::MouseMoveEvent &mme, 
                         vec3f dxAxis, vec3f dyAxis)
{

   quatf cam_rot(_cm.rot());
   cam_rot = cam_rot.conj();
   vec3f vecdx(cam_rot.rot(dxAxis));
   vec3f vecdy(cam_rot.rot(dyAxis));

   vecdx *= mme.getDel(). x() * _currentSpeed;
   vecdy *= -mme.getDel().y() * _currentSpeed;

   _eyePos = _eyePos + vecdx + vecdy;      //move eye
   _lookat = _lookat + vecdx + vecdy;      //move lookat

   _setViewMatrix( look_at(_eyePos, _lookat, _upvec) );//update camera

}

//rotate camera about axis ===============================================

void Camera::cameraRotAxis(const gutz::MouseMoveEvent &mme, 
                           vec3f dxAxis, vec3f dyAxis)
{
   if(vec3f_zero != dxAxis)
   {
      dxAxis.normalize();
      quatf dxrot((mme.getDel().x()*_currentSpeed/180.0f) * float(M_PI), dxAxis);
      _camquat = _camquat.mult(dxrot);
   }
   if(vec3f_zero != dyAxis)
   {
      dyAxis.normalize();
      quatf dyrot((-mme.getDel().y()*_currentSpeed/180.0f) * float(M_PI), dyAxis);
      _camquat = _camquat.mult(dyrot);
   }

   mat3f basis(_camquat);                              //get basis
   _upvec = vec3f(basis[3], basis[4], basis[5]);       //up
   vec3f    zaxis(basis[6], basis[7], basis[8]);       //Z
   float zdist = (_lookat - _eyePos).norm();           //lookat dist
   _lookat = _eyePos - zaxis * zdist;                  //lookat
   _setViewMatrix( look_at(_eyePos, _lookat, _upvec) );             //update camera

}


//camera swivel =========================================================

void Camera::cameraSwivel(const gutz::MouseMoveEvent &mme)
{
   vec2f pt1(( 2.0f * mme.getLast().x() - _screen[0])  / (float)_screen[0], 
             ( _screen[1] - 2.0f * mme.getLast().y() ) / (float)_screen[1]);
   vec2f pt2(( 2.0f * mme.getPos().x() - _screen[0])   / (float)_screen[0], 
             ( _screen[1] - 2.0f * mme.getPos().y())  / (float)_screen[1]);

   quatf tmpquat = trackball(pt1, pt2, 1.8f);    
   tmpquat = tmpquat.conj();
   _swivtrackquat = tmpquat;                     //tumble amount
   _camquat = _camquat.mult(tmpquat);   

   mat3f basis(_camquat);                        //camera basis matrix
   _upvec = vec3f(basis[3], basis[4], basis[5]); //up
   vec3f    zaxis(basis[6], basis[7], basis[8]); //Z

   vec3f  vdir(_lastLookat - _eyePos);           //view dir
   float  dist = vdir.norm();                    //lookat dist
   zaxis  *= -dist;                              //new view dir
   _lookat = zaxis + _eyePos;                    //new lookat point

   _setViewMatrix( look_at( _eyePos, _lookat, _upvec) );

}

gutz::Camera::ptr Camera::domeIntersection(const gutz::vec3f& pos, const float& radius, bool fcam) const

{
#if 0
   {
      // Find lookat, up and right vectors for the camera, and frustum
      gutz::mat4f inv    = getViewMatrix().inv();
      gutz::vec4f eye    = inv * gutz::vec4f(0.0, 0.0, 0.0, 1.0);
      gutz::vec4f lookAt = inv * gutz::vec4f(0.0, 0.0, -1.0f, 1.0f);
      gutz::vec4f up     = inv * gutz::vec4f(0.0, 1.0,  0.0f, 1.0f);
      gutz::vec4f right  = inv * gutz::vec4f(1.0, 0.0,  0.0f, 1.0f);
      std::cout 
      << "  eye:     " << eye    << std::endl
      << "  lookAt:  " << lookAt << std::endl
      << "  up:      " << up     << std::endl
      << "  right:   " << right  << std::endl
      << "  frustum: " << getFrustumLeft() << "," << getFrustumRight() << "," 
      << getFrustumBottom() << "," << getFrustumTop() << "," << getClips()[0] << std::endl;
   }
#endif
   
   // Define the meaning of the array indices for domeIsec, domeVec and isec.
   enum POINTS
   {
      LEFT_BOTTOM  = 0, // Left - bottom of the frustum
      LEFT_TOP     = 1, // Left - top of the frustum
      RIGHT_BOTTOM = 2, // Right - bottom of the frustum
      RIGHT_TOP    = 3, // right - top of the frustum
      CENTER       = 4  // Center of the frustum
   };
   
   // Dome intersection points
   std::vector<gutz::vec3f> domeIsec(5);

   // Vectors that point from the projector position to the dome.
   std::vector<gutz::vec3f> domeVec(5);
   
   // Get the vectors that define the frustum and the view direction. These vectors point
   // from the camera to the dome.
   std::vector<gutz::vec2f> pt(5);
   pt[LEFT_BOTTOM]  = gutz::vec2f( 0.0f,  0.0f); // left bottom
   pt[LEFT_TOP]     = gutz::vec2f( 0.0f,  1.0f); // left top
   pt[RIGHT_BOTTOM] = gutz::vec2f( 1.0f,  0.0f); // right bottom
   pt[RIGHT_TOP]    = gutz::vec2f( 1.0f,  1.0f); // right top
   pt[CENTER]       = gutz::vec2f( 0.5f,  0.5f); // center of the frustum.
   
   gutz::vec3f up      = getUpVec();   // Projector up direction
   gutz::vec3f xd      = getXVec();    // Projector x direction
   gutz::vec3f view    = getViewDir(); // Projector view direction
   gutz::vec3f projPos = getEyePos();  // Projector position

   ////////////////////////////////////////////////////////////////////////////////////////
   // Find eye, at and up for the new camera
   ////////////////////////////////////////////////////////////////////////////////////////

   // Find vectors that point from the projector position towards the dome
   for(int i = 0; i < 5; ++i)
   {
      gutz::vec3f screenPos = (projPos + getNearClip() * view);
      screenPos += (getFrustumBottom() + getFrustumHeight() * pt[i].y()) * up;
      screenPos += (getFrustumLeft()   + getFrustumWidth()  * pt[i].x()) * xd;
      domeVec[i] = screenPos - projPos;
   }
   
   // Find where those vectors intersect the dome
   for(int i = 0; i < 5; ++i)
   {
      // Find the two intersections
      gutz::vec2f time = gutz::intersectRaySphere(projPos, domeVec[i],
                                                  gutz::vec3f(0.0f, 0.0f, 0.0f), radius);
      // Take the farthest
      float t = time.x() > time.y() ? time.x() : time.y();
      // Evaluate the ray at time t to get the intersection
      domeIsec[i] = projPos + domeVec[i] * t;
   }

   // The "F" cam needs a special up vector
   gutz::vec3f camUp = getUpVec();
   if(fcam == true)
   {
      camUp = gutz::vec3f(1,0,0);
   }

   // Set up the intersection camera with our eye at and up vectors
   gutz::Camera::ptr cam = new gutz::Camera();
   cam->setEyePos(pos);
   cam->setLookAt(domeIsec[CENTER]);
   cam->setUpVec(camUp);
   cam->setClips(getNearClip(), getFarClip());

   
   ////////////////////////////////////////////////////////////////////////////////////////
   // Calculate the frustum
   //
   // The dome intersection points define the frustum, but they need to be projected
   // back onto the near clip plane.
   ////////////////////////////////////////////////////////////////////////////////////////

   // Find the projector-eye camera up, view and right (xd) directions
   gutz::mat4f inv  = cam->getViewMatrix().inv();
   gutz::vec4f peUp   = inv * gutz::vec4f(0.0, 1.0, 0.0, 1.0) - gutz::vec4f(pos, 1.0f);
   gutz::vec4f peView = inv * gutz::vec4f(0.0, 0.0,-1.0, 1.0) - gutz::vec4f(pos, 1.0f);
   gutz::vec4f peXd   = inv * gutz::vec4f(1.0, 0.0, 0.0, 1.0) - gutz::vec4f(pos, 1.0f);

   // Downcast those directions from gutz::vec4f to gutz::vec3f
   up   = gutz::vec3f(peUp.x(),   peUp.y(),   peUp.z());
   view = gutz::vec3f(peView.x(), peView.y(), peView.z());
   xd   = gutz::vec3f(peXd.x(),   peXd.y(),   peXd.z());
   
   // Normalize those vectors
   up.normalize();
   view.normalize();
   xd.normalize();
   
   // Compute intersection points on new near clip plane
   gutz::vec3f planePoint = pos + getNearClip() * view;
   std::vector<gutz::vec2f> isec(4);

   for(int i = 0; i < 4; ++i)
   {
      // Find intersection time
      vec3f dir = domeIsec[i] - pos;
      float t = gutz::intersectRayPlane<float>(pos, dir, planePoint, view);
      vec3f planeIsec = pos + dir * t;

      // planeIsec is in world space. We need the intersection values in
      // frustum space (left, right, top, bottom)

      // Get a vector from the origin on the plane to the plane intersection
      vec3f dir2corner = planeIsec - planePoint;
      
      // How much is that vector in the xd direction?
      isec[i].v[0] = dir2corner.dot(xd);
      
      // How much is that vector in the up direction?
      isec[i].v[1] = dir2corner.dot(up);
   }
   
   // Sort the frustum values to find left,right,top,bottom
   float left   = isec[0].x();
   float right  = isec[0].x();
   float top    = isec[0].y();
   float bottom = isec[0].y();
   
   for(int i = 0; i < 4; ++i)
   {
      if(isec[i].x() < left)   left   = isec[i].x();
      if(isec[i].x() > right)  right  = isec[i].x();
      if(isec[i].y() < bottom) bottom = isec[i].y();
      if(isec[i].y() > top)    top    = isec[i].y();
   }
   
   // Set up the frustum
   cam->setFrustum(left,right,bottom,top);
   
#if 0
   {
      // Find lookat, up and right vectors for the camera, and frustum
      gutz::vec4f eye    = cam->getViewMatrix().inv() * gutz::vec4f(0.0, 0.0, 0.0, 1.0);
      gutz::vec4f lookAt = cam->getViewMatrix().inv() * gutz::vec4f(0.0, 0.0, -1.0f, 1.0f);
      gutz::vec4f up     = cam->getViewMatrix().inv() * gutz::vec4f(0.0, 1.0,  0.0f, 1.0f);
      gutz::vec4f right  = cam->getViewMatrix().inv() * gutz::vec4f(1.0, 0.0,  0.0f, 1.0f);
      std::cout 
      << "  eye:     " << eye    << std::endl
      << "  lookAt:  " << lookAt << std::endl
      << "  up:      " << up     << std::endl
      << "  right:   " << right  << std::endl
      << "  frustum: " << cam->getFrustumLeft() << "," << cam->getFrustumRight() << "," 
      << cam->getFrustumBottom() << "," << cam->getFrustumTop() << "," << cam->getClips()[0] << std::endl;
   }
#endif
   
   return cam;
}


#ifdef KONG
// camera pack and unpack methods =======================================

// ----------------
// --- packSize ---
// ----------------
//
unsigned int Camera::packSize(void)
{
  // How big of a buffer do we need to make a packed version
  // of the camera?
  return( sizeof(vec2f) + sizeof(mat4f) * 2 + sizeof(vec3f) * 3);
}

// ------------
// --- pack ---
// ------------
//
void Camera::pack(KokoBuffer &kbuf, bool reset)
{
  if (reset)
    kbuf.Pack();  // Put the buffer in pack mode. 
  kbuf.Pack((unsigned char *)&_clips,  sizeof(vec2f));      // clips
  kbuf.Pack((unsigned char *)&_cm,     sizeof(mat4f));      // camera matrix.
  kbuf.Pack((unsigned char *)&_pm,     sizeof(mat4f));      // proj. matrix.
  kbuf.Pack((unsigned char *)&_eyePos, sizeof(vec3f));      // eye position.
  kbuf.Pack((unsigned char *)&_lookat, sizeof(vec3f));      // look at point.
  kbuf.Pack((unsigned char *)&_upvec,  sizeof(vec3f));      // up vector. 
}


// --------------
// --- unpack ---
// --------------
//
void Camera::unpack(KokoBuffer &kbuf, bool reset) 
{
  if (reset)
    kbuf.Unpack();  // Put the buffer in extraction mode.
  kbuf.Unpack((unsigned char *)&_clips,  sizeof(vec2f));     // clips
  kbuf.Unpack((unsigned char *)&_cm,     sizeof(mat4f));     // camera matrix.
  kbuf.Unpack((unsigned char *)&_pm,     sizeof(mat4f));     // proj. matrix.
  kbuf.Unpack((unsigned char *)&_eyePos, sizeof(vec3f));     // eye position.
  kbuf.Unpack((unsigned char *)&_lookat, sizeof(vec3f));     // look at point.
  kbuf.Unpack((unsigned char *)&_upvec,  sizeof(vec3f));     // up vector. 
  //look_at(_cm, _eyePos, _lookat, _upvec);
  _setViewMatrix(look_at(_eyePos, _lookat, _upvec));
  _camquat.set(_cm.rot());  
}

#endif
