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

//ManipEvents.cpp
// Joe Kniss


#include "ManipEvents.h"
#include "../eventGutz/mouseEvent.h"
#include <iostream>
#include <vectorGraphics.h>

const float defaultRad = .8f;

using namespace gutz;
using namespace std;

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/// RotateManipEvent
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

bool RotateManipEvent::startEvent(Manip *m, const MouseEvent &me)
{
   if(me.isButtonDown())
   {
      _rad = m->getRad(me);
      _tumbquat = quatf_id;
   }
   return true;
}

bool RotateManipEvent::handleEvent(Manip *m, const MouseMoveEvent &mme)
{
   vec2f pt1 = mme.getLast();
   vec2f pt2 = mme.getPos();
   /// compute the projection of the center onto the screen
   vec3f scrC = m->getScreenPosLocal(m->getCenter());
   /// flip the y position of screen center
   scrC.y() = m->getScreenY() - scrC.y();

   /// offset screen center to the objects center projected to the screen,
   /// and scale-bias so that we are in the [-1,1] range
   pt1.x() =   (pt1.x() - scrC.x()) * 2.0f/float(m->getScreenX());
   pt1.y() =   -(pt1.y() - scrC.y()) * 2.0f/float(m->getScreenY());
   pt2.x() =   (pt2.x() - scrC.x()) * 2.0f/float(m->getScreenX());
   pt2.y() =   -(pt2.y() - scrC.y()) * 2.0f/float(m->getScreenY());

   quatf tmpquat = trackball(pt1, pt2, _rad);     //quat for small rotation
   tmpquat.normalize();

   /// set the trackquat, for tumble
   _tumbquat = m->getInvCamQuat().mult( tmpquat.mult( m->getCamQuat() ) );
   _tumbquat.normalize();
   m->setCenterQuat( _tumbquat.mult( m->getCenterQuat() ) );
 
  return true;
}

void RotateManipEvent::endEvent(Manip *m, const MouseEvent &me)
{
  _tumbquat = quatf_id;
}

/// really this should keep a time signature an scale based on that
bool RotateManipEvent::tumble(Manip *m)
{
   if( g_abs(_tumbquat[3]) > .9999 ) return false;

   //reset the track quaternion to the right speed
   float thet = _tumbquat[3];
   thet = acos(thet);
   vec3f axis(_tumbquat[0]/sin(thet),
              _tumbquat[1]/sin(thet),
              _tumbquat[2]/sin(thet));
   thet*=_speed*2;
   quatf tumb(thet, axis);

   m->setCenterQuat( tumb.mult( m->getCenterQuat() ) ); 
   return true;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/// RotateAxisManipEvent
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

/// should be called on a Mouse-down event, starts the event
bool RotateAxisManipEvent::startEvent(Manip *m, const MouseEvent &me)
{  
  if(me.isButtonDown() )
    {
      _rad = m->getRad(me);
      _tumbquat = quatf_id;
    }
    return true;
}

bool RotateAxisManipEvent::handleEvent(Manip *m, const MouseMoveEvent &mme)
{
   vec2f pt1 = mme.getLast();
   vec2f pt2 = mme.getPos();
   /// compute the projection of the center onto the screen
   vec3f scrC = m->getScreenPosLocal(m->getCenter());
   /// flip the y position of screen center
   scrC.y() = m->getScreenY() - scrC.y();

   /// offset screen center to the objects center projected to the screen,
   /// and scale-bias so that we are in the [-1,1] range
   pt1.x() =   (pt1.x() - scrC.x()) * 2.0f/float(m->getScreenX());
   pt1.y() =   -(pt1.y() - scrC.y()) * 2.0f/float(m->getScreenY());
   pt2.x() =   (pt2.x() - scrC.x()) * 2.0f/float(m->getScreenX());
   pt2.y() =   -(pt2.y() - scrC.y()) * 2.0f/float(m->getScreenY());

   quatf tmpquat = trackball(pt1, pt2, _rad);     //quat for small rotation
   tmpquat.normalize();
   
   /// set the trackquat, for tumble
   _tumbquat = m->getInvCamQuat().mult( tmpquat.mult( m->getCamQuat() ) );
   _tumbquat.normalize();

   vec4f aa = _tumbquat.getAngleAxis();
   
   float dot = aa[0]*_axis[0]+aa[1]*_axis[1]+aa[2]*_axis[2];
   float angle = aa[3]*dot;

   _tumbquat.set( angle, _axis );

   m->setCenterQuat( _tumbquat.mult( m->getCenterQuat() ) );
 
  return true;
}


/// really this should keep a time signature an scale based on that
bool RotateAxisManipEvent::tumble(Manip *m)
{
   if( g_abs(_tumbquat[3]) > .9999 ) return false;

   //reset the track quaternion to the right speed
   float thet = _tumbquat[3];
   thet = acos(thet);
   vec3f axis(_tumbquat[0]/sin(thet),
              _tumbquat[1]/sin(thet),
              _tumbquat[2]/sin(thet));
   thet*=_speed*2;
   quatf tumb(thet, axis);

   m->setCenterQuat( tumb.mult( m->getCenterQuat() ) ); 
   return true;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/// TransXYManipEvent
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

bool TransXYManipEvent::startEvent(Manip *m, const MouseEvent &me)
{
   if(me.isButtonDown())
   {
      _lastTrans = vec3f_zero;
   }
   return true;
}

vec3f TransXYManipEvent::getDelta(Manip *m, const MouseMoveEvent &mme) const
{
   vec3f delta;

   if(mme.z() == 0 && m->getParent()) /// didn't have a picked point
   {
      /// use ray-plane intersection to compute the delta vector
      /// get the ray through the screen points (curr&last) from the eye
      const vec3f lastDir = m->getParent()->getLocalEyeRayScreen(mme.getLast());
      const vec3f currDir = m->getParent()->getLocalEyeRayScreen(mme.getPos());
      const vec3f lastPoint = m->getParent()->getLocalPosScreen(mme.getLast());
      const vec3f currPoint = m->getParent()->getLocalPosScreen(mme.getPos());
      /// intersect rays with plane at our local position oriented with view direction
      const float tl = 
         intersectRayPlane(lastPoint,lastDir,
                           m->getLocalPos(),m->getParent()->getLocalViewDir());
      const float tc = 
         intersectRayPlane(currPoint,currDir,
                           m->getLocalPos(),m->getParent()->getLocalViewDir());
      /// compute delta based on the current & last intersections
      // delta = current point - last
      // current point = curren point + tc * current direction vector (ray equation)
      delta = (currPoint + tc * currDir) - (lastPoint + tl * lastDir);
   }
   else if(m->getParent()) /// we DID have a valid pick point
   {
      /// therfore the points from the event just need to be translated to
      ///  our (parents) LOCAL space
      delta = m->getParent()->getLocalPosScreen(mme.getPos())
              - m->getParent()->getLocalPosScreen(mme.getLast());
   }
   else /// else we didn't have a parent, we just have to use the points...
   {
      delta = mme.getPos() - mme.getLast();
   }
   return delta;
}


bool TransXYManipEvent::handleEvent(Manip *m, const MouseMoveEvent &mme)
{

   _lastTrans = getDelta(m, mme);
   m->setLocalPos( m->getLocalPos() + _lastTrans );

   return true;
}

void TransXYManipEvent::endEvent(Manip *m, const MouseEvent &me)
{
   _lastTrans = vec3f_zero;
}

bool TransXYManipEvent::tumble(Manip *m)
{
   if( _lastTrans == vec3f_zero ) return false;
   m->setLocalPos( m->getLocalPos() + _lastTrans * _speed );
   return true;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/// TransZManipEvent
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

bool TransZManipEvent::handleEvent(Manip *m, const MouseMoveEvent &mme)
{
   _lastTrans = getDelta(m, mme);
   float del = _lastTrans.norm();
   if( (-mme.dx() + mme.dy()) > 0.0f ) del = -del;
   vec3f vdir = vec3f_z;
   if(m->getParent()) vdir = m->getParent()->getLocalViewDir();
   _lastTrans = vdir * del;
   m->setLocalPos( m->getLocalPos() + _lastTrans );
   return true;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/// TransZManipEvent
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

bool TransPlaneManipEvent::startEvent(Manip *m, const MouseEvent &me)
{
   if( _pnorm == vec3f_zero ) return false;

   if(me.isButtonDown())
   {
      if(me.z())
      {
         _start = me.getWorldPos();
      }
      else
      {
         _start = _ppos;
      }
      _lastTrans = vec3f_zero;
   }
   return true;
}


bool TransPlaneManipEvent::handleEvent(Manip *m, const MouseMoveEvent &mme)
{
   vec3f lpos = mme.getLast();
   vec3f cpos = mme.getPos();
   vec3f lray = mme.getLast();
   vec3f cray = mme.getPos();
   vec3f ppos =  _start;
   vec3f pnorm = _pnorm;
   if( m->getParent() )
   {
      lpos  = m->getParent()->getLocalPosScreen( lpos );
      cpos  = m->getParent()->getLocalPosScreen( cpos );
      lray  = m->getParent()->getLocalEyeRayScreen( lray );
      cray  = m->getParent()->getLocalEyeRayScreen( cray );
      ppos  = m->getParent()->getLocalPosWorld( ppos );
      pnorm = m->getParent()->getLocalDirWorld( pnorm );
   }

   float tl = gutz::intersectRayPlane(lpos,lray,ppos,pnorm);
   float tc = gutz::intersectRayPlane(cpos,cray,ppos,pnorm);

   _lastTrans = ( cpos + cray * tc ) - ( lpos + lray * tl );

   m->setLocalPos( m->getLocalPos() + _lastTrans );

   return true;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/// TransLineManipEvent
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

bool TransLineManipEvent::startEvent(Manip *m, const MouseEvent &me)
{
  return true;
}

/// should be called when the mouse moves
bool TransLineManipEvent::handleEvent(Manip *m, const MouseMoveEvent &mme)
{
  vec3f dir = m->getWorldEyeRayScreen( mme.getPos() );
  vec3f pos = mme.getWorldPos();
  vec3f lsp = _lstart;
  vec3f lep = _lend;

  vec3f ldir = lep - lsp;

  /// returns the 't' value for first line in [1] element of vector
  vec3f linesol = getMinLineDistance(lsp, ldir, pos, dir);
  float t = linesol[1];
  /// clamp to ends (maybe)
  if( _clamp )
    {
      t = t < 0 ? 0 : t > 1 ? 1 : t;
    }

  /// compute new position
  vec3f npos = lsp + t*ldir;

  /// set it
  if( m->getParent() )
    {
      npos = m->getParent()->getLocalPosWorld( npos );
    }

  m->setLocalPos( npos );
  return true;
}

/// event is over
void TransLineManipEvent::endEvent(Manip *m, const MouseEvent &me)
{

}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/// ScaleXManipEvent
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool ScaleXManipEvent::handleEvent(Manip *m, const MouseMoveEvent &mme)
{
  vec2f pt1 = mme.getLast();
  vec2f pt2 = mme.getPos();

  // Compute the difference between the last and current mouse positions (use the greatest difference in the two dimensions)
  float diff1 = pt2[0] - pt1[0];
  float diff2 = pt1[1] - pt2[1];
  float diff = fabs(diff1) > fabs(diff2) ? diff1 : diff2; 
  vec3f scale = m->getLocalScale();

  // Multiply the scale by the difference and the speed
  scale[0] += (diff*_speed); 

  // Clamp the scale
  if(scale[0] < _clamp)
     scale[0] = _clamp;

  // Set the scale
  m->setLocalScale(scale);

  return true;
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/// UniformScaleXYManipEvent
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool UniformScaleXYManipEvent::handleEvent(Manip *m, const MouseMoveEvent &mme)
{
  vec2f pt1 = mme.getLast();
  vec2f pt2 = mme.getPos();

  // Compute the difference between the last and current mouse positions (use the greatest difference in the two dimensions)
  float diff1 = pt2[0] - pt1[0];
  float diff2 = pt1[1] - pt2[1];
  float diff = fabs(diff1) > fabs(diff2) ? diff1 : diff2; 
  vec3f scale = m->getLocalScale();

  // Multiply the scale by the difference and the speed
  scale[0] += (diff*_speed); 
  scale[1] += (diff*_speed); 

  // Clamp the scale
  if(scale[0] < _clamp)
     scale[0] = _clamp;
  if(scale[1] < _clamp)
     scale[1] = _clamp;

  // Set the scale
  m->setLocalScale(scale);

  return true;
}