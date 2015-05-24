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

//manip.cpp
//Joe Kniss

//#pragma warning(disable:4786) //dissable stl identifier too long warnings

#include "Manip.h"
#include "ManipEvents.h"
#include <iostream>
#include <math.h>
//#include <serialize/SerializeUtil.h>
#include "../eventGutz/mouseEvent.h"

#ifndef M_PI
#define M_PI 3.141592654f
#endif

#ifdef KONG
#include <KokoBuffer.h>
#endif

using namespace gutz;
using namespace std;

const float defaultRad = .8f;

////////////////////////////////////////////////////////////
/// Constructor

Manip::Manip(Camera *cam)
: BaseManip(),
  ManipEventMap(),
  _rad(defaultRad),
  _userRad(0),
  _center(vec3f_zero),
  _baked(mat4f_id),
  _scale(vec3f_one),
  _om(mat4f_id),
  _oinv(mat4f_id),
  _currquat(quatf_id),
  _objtrackquat(quatf_id),
  _parent(0)
{
}

Manip::Manip(const Manip &m)
: BaseManip(m),
  ManipEventMap(m),
  _rad(m._rad),
  _userRad(m._userRad),
  _center(m._center),
  _baked(m._baked),
  _scale(m._scale),
  _om(m._om),
  _oinv(m._oinv),
  _currquat(m._currquat),
  _objtrackquat(m._objtrackquat),
  _parent(m._parent)
{
   
}

Manip &Manip::operator=(const Manip &m)
{
   BaseManip::operator =(m);
   ManipEventMap::operator =(m);
   _objtrackquat = m._objtrackquat;
   _currquat = m._currquat;
   _om = m._om;
   _oinv = m._oinv;
   _parent = m._parent;
   _rad = m._rad;
   _userRad = m._userRad;
   _center = m._center;
   _baked = m._baked;
   _scale = m._scale;
   return *this;
}

//=========================================================================
//  (Mouse Event) Get Radius
//=========================================================================
float   Manip::getRad(const gutz::MouseEvent &me)
{
   ////////////////////////////////////////////
   /// determine radius for trackball rotation
   if(_userRad)/// user specifed a radius
   {     
      _rad = _userRad;
   }  
   else
   {
      if( me.getPos().z())
      {
         _rad = ( getLocalPosScreen(me.getPos()) - _center ).norm() * 2.0f;
      }
      else
      {
         return defaultRad;
      }
   }
   /// figure out the correct radius for rotation
   vec3f cscreen = getScreenPosLocal(_center); /// project center to screen
   vec3f crad = cscreen + vec3f_x;             /// add one increment in x direction
   crad = getLocalPosScreen(crad);             /// project back to local space
   crad -= _center;                            /// get local x vector
   crad.normalize();                           /// normalize that
   crad *= _rad;                               /// multiply by the radius
   crad = getScreenPosLocal(crad+_center);     /// project that point to the screen
   crad -= cscreen;                            /// compute the difference in screen space
   /// rad is the norm of this vector
   _rad = sqrt( crad.x() * crad.x() + crad.y() * crad.y() )/getScreenX(); 
   cerr << "rad = " << _rad << endl;
   ////////////////////////////////////////////

   return _rad;
}


//=========================================================================
//  Set/Get Matrix-components
//=========================================================================
void   Manip::setCenter(const vec3f &center)
{
   if(_center == center) return;
   setMatrix( _om );
   _center = center;
}

void Manip::setLocalQuat(const quatf &lq)
{
   setMatrix( mat4f( mat3f(lq), _om.getTrans() ) );
}

void Manip::setLocalOrient(float angle, vec3f axis)
{
  setLocalQuat( quatf(angle,axis) );
}

void Manip::setLocalPos(const vec3f &pos)
{
   if( pos == getLocalPos() ) return;
   _baked.setTrans( _baked.getTrans() + pos - getLocalPos() );
   updateMatrix();
}

void Manip::setLocalScale(const vec3f &scale)
{
  _scale = scale;
  updateMatrix();
}

void Manip::setLocalScale(const vec3f &scale, const vec3f center)
{
  _scale = scale;
  vec3f c = _center;
  _center = center;
  updateMatrix();
  _center = c;
}

void Manip::setLocalScale(float scale)
{
  _scale = vec3f(scale);
  updateMatrix();
}

mat4f Manip::buildMatrix() const         
{
  mat4f sm(_scale[0], 0,         0,         0,
           0,         _scale[1], 0,         0,
           0,         0,         _scale[2], 0,
           0,         0,         0,         1);
   return _baked * 
     mat4f(mat3f_id, _center) * 
     sm * 
     mat4f( mat3f( _currquat ), vec3f_zero ) * 
     mat4f( mat3f_id, -_center );
}

void  Manip::setMatrix(const gutz::mat4f &m)
{
   _baked = m;
   _currquat = quatf_id;
   _om = m;
   _scale = vec3f_one;
   _oinv = _om.inv();
   //_currquat = _om.rot();
}

void Manip::updateMatrix()
{
   _om   = buildMatrix();
   _oinv = _om.inv();
}

//=========================================================================
//  Set Parent
//=========================================================================
void Manip::setParent(const BaseManipSP parent)
{
   _parent = parent;
}

//=========================================================================
//  Insert Parent
//=========================================================================
void Manip::insertParent(const BaseManipSP parent)
{
   /// can't insert a "0" parent, use setParent()
   if( !parent ) return; 

   if( _parent ) /// we currently have a parent
   {
      if( parent->isCamera() )  /// if our new parent is a camera
      {
         if( _parent->isCamera() ) /// and our current parent is a camera 
         {
            _parent = parent;  /// exchange old camera for new one
            return;
         }  
         else  // current parent isn't a camera, so send new camera/parent up the chain
         {
            _parent->insertParent( parent );
            return;
         }
      }
      /// new parent isn't a camera, just insert new parent between current and us
      parent->setParent(_parent);
      _parent = parent;
      return;
   }
   _parent = parent;
}

//=========================================================================
//  get camera
//=========================================================================
Camera *Manip::getCamera() const
{
   if(_parent)
   {
      if(_parent->isCamera())
      {
         return _parent.cast<Camera>();
      }
      else
      {
         return _parent->getCamera();
      }
   }
   return CameraSP(0);

}

////////////////////////////////////////////////////////////
/// Serialization.
/// write.
//=========================================================================
//  Uber Serialize
//=========================================================================
const string DATA_STR("Data{");
const string END_SECTION_STR("\n}\n");

bool Manip::serialize(std::ostream &os, const std::string indent)
{
   string ind = indent;
   ind += "   ";

   /// serial data
   os << indent << DATA_STR << " \n";
   serialData(os, ind);
   os << indent << END_SECTION_STR;

   bool status = false;

   return status;
}

/// read.
//=========================================================================
//  Uber UnSerialize
//=========================================================================
bool Manip::unserialize(std::istream &is)
{
   /// TODO: fix this!
#if 0
   string tstr;

   bool status = false;

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

/// serialize the data parts of this guy for momentos & transmission
//=========================================================================
//  Serialize Data
//=========================================================================
const string MAT_STR("Matrix: \n");

bool Manip::serialData(std::ostream &os, const std::string indent)
{
   BaseManip::serialData(os,indent);

   os << indent << MAT_STR << _om;

   return false;
}

//=========================================================================
//  UnSerialize Data
//=========================================================================
bool Manip::unserialData(std::istream &is)
{
   /// TODO: fix this
#if 0
   BaseManip::unserialData(is);

   string tstr;
   is >> tstr;
   if(serial::unserialKeyCheck(MAT_STR, tstr, "Manip, matrix")) return true;
   is >> _om;

   _currquat = _om.rot();

   return is.eof() || is.fail();
#endif
   return true;
}

//=========================================================================
//  Mouse/move/tumble
//=========================================================================
bool Manip::mouse(const gutz::MouseEvent &me)
{
   setCamera(me.getCamera());
   return ManipEventMap::mouse(this, me.getButton(), me);
}

bool Manip::move(const gutz::MouseMoveEvent &mme)
{
   return ManipEventMap::move(this, mme);
}

void Manip::tumble(float speed)
{
   ManipEventMap::tumble(this);
}

//=========================================================================
//  Kong
//=========================================================================
#ifdef KONG
// manip pack and unpack methods =====================================

// ----------------
// --- packSize ---
// ----------------
//
unsigned int Manip::packSize(void)
{
  // How big of a buffer do we need to make a packed version
  // of the camera?
  return( sizeof(mat4f) + sizeof(vec3f) );
}


// ------------
// --- pack ---
// ------------
//
void Manip::pack(KokoBuffer &kbuf, bool reset)
{
  if (reset) 
    kbuf.Pack();  // Put the buffer in pack mode. 
  kbuf.Pack((unsigned char *)&_om,  sizeof(mat4f));      
  kbuf.Pack((unsigned char *)&_center,  sizeof(vec3f));      
}


// --------------
// --- unpack ---
// --------------
//
void Manip::unpack(KokoBuffer &kbuf, bool reset) 
{
  if (reset)
    kbuf.Unpack();  // Put the buffer in extraction mode.
  kbuf.Unpack((unsigned char *)&_om,  sizeof(mat4f));    
  kbuf.Unpack((unsigned char *)&_center,  sizeof(vec3f));    
  setMatrix( _om );
}

#endif
//=========================================================================
//  end Kong
//=========================================================================



