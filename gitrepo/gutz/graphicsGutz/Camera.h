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
//                           2002ƒ
///////////////////////////////////////////////////////////////////////////

//Camera.h
// Joe Kniss

#ifndef __GUTZ_CAMERA_DOT_H
#define __GUTZ_CAMERA_DOT_H

#include <mathGutz.h>
#include <list>
#include <vector>
#include "baseManip.h"
#include <iostream>
#include "vectorGraphics.h"

namespace gutz {

////////////////////////////////////////////////////////////
/// forward decls...

////////////////////////////////////////////////////////////
/// Interaction Modes supported by the gutz::Camera manipulator

enum{ //Supported modes, camera Actions
   CAM_MODE_NONE,          ///< Mode not mapped
   CAM_ROT,                ///< Camera rotates about lookat point
   CAM_ZOOM,               ///< Changes frustum, no movement
   CAM_DOLLY,              ///< Camera forward & backward
   CAM_TRANS_XY,           ///< Camera space up/down left/right 
   CAM_TRANS_XZ,           ///< Camera space left/right forward/back
   CAM_TRANS_ZY,           ///< Camera space up/down forward/back
   CAM_TRANS_Z,            ///< Move camera forward and back
   CAM_TRANS_Y,            ///< Move camera up and down
   CAM_TRANS_X,            ///< Move camera side to side
   CAM_ROLL,               ///< Roll, rotate around Z
   CAM_PITCH,              ///< Pitch, rotate around X
   CAM_YAW,                ///< Yaw, rotate around Y
   CAM_SWIVEL,             ///< Pitch & Yaw, rotate about eye point
   CAM_LAST_MODE
};

////////////////////////////////////////////////////////////
/// The "LOCAL" space for this manipulator is WORLD space. 
/// This object maintains the Projection and View matricies.
/// It also handles VIEWPORT transforms implicitly, you 
/// can take SCREEN space points (0-SX)(0-SY)(0-1) and get
/// them either in EYE space or WORLD space. Where:
/// - SX = screen size x
/// - SY = screen size y
/// - (0-1) : 0 = nearClip, 1 = farClip
////////////////////////////////////////////////////////////
class Camera : public BaseManip {
public:
   
   typedef SmartPtr<Camera>           ptr;
   
   typedef std::list<ptr>             list;
   typedef std::list<ptr>::iterator   list_itr;
   
   typedef std::vector<ptr>           vec;
   typedef std::vector<ptr>::iterator vec_itr;
   
   Camera();
   Camera(const Camera &cam);

   virtual ~Camera();

   bool    isCamera() const  { return true;  }
   bool    isManip()  const  { return false; }
   /// cameras don't have cameras!!!
   Camera* getCamera()const  { return 0; }

  /// scales the frustum by "scale" (TODO: should be 1/scale?)
  void  zoom( float scale );
  /// sets the width of the frustum (TODO: should be 1/width?)
  void  setZoom( float z );
  /// gets the width fo the frustum (TODO: should be 1/width?)
  float getZoom() const;

   ////////////////////////////////////////////////////////////
   ///@name Transformations
   ///@{
   /// get the projection transform, PROJECTION<-LOCAL
   gutz::mat4f getProjectTransform() const  {  return _pm * _cm; }
   /// get the inverse projection transform, PROJECTION->LOCAL
   gutz::mat4f getInvProjectTransform() const { return _cinv * _pinv; }
   /// get the screen transform, SCREEN<-PROJECT
   ///  this is wierd because you must apply the Project transform,
   ///  then divide by w, then apply the screen transform.  
   gutz::mat4f getScreenTransform() const { return getViewportMatrix(); }
   /// get the inverse screen transform, SCREEN->LOCAL
   gutz::mat4f getInvScreenTransform() const { return getInvViewportMatrix(); }
   /// get the eye transform: EYE<-LOCAL,
   gutz::mat4f getEyeTransform() const { return _cm; }
   /// get the world transform: WORLD<-LOCAL (Identity) Joe sez wtf?
   gutz::mat4f getWorldTransform() const { return mat4f(); }
   /// get the transform that this manipulator defines
   gutz::mat4f getLocalTransform() const { return _cm; }
   /// get the inverse eye transform: EYE->LOCAL
   gutz::mat4f getInvEyeTransform() const { return _cinv; }
   /// get the inverse world transform: WORLD->LOCAL (Identity)
   gutz::mat4f getInvWorldTransform() const { return mat4f(); }
   /// get the inverse transform that this manipulator defines
   gutz::mat4f getInvLocalTransform() const { return _cinv; }
   /// get the projection matrix currently associated with this transform chain
   gutz::mat4f getProjection() const { return _pm; }
   /// get the inverse projection matrix (inverse of getProjection())
   gutz::mat4f getInvProjection() const { return _pinv; }
   /// get the quaternion that expresses the total rotation in this chain
   ///gutz::quatf getEyeQuat() const { return _camquat; }
   gutz::quatf getEyeQuat() const { return quatf( _cm.rot() ); }
   /// get the inverse quaternion for rotation from EYE to this space
   gutz::quatf getInvEyeQuat() const { return getEyeQuat().conj(); }
   /// get the quaternion for rotation: WORLD<-LOCAL (Identity)
   gutz::quatf getWorldQuat() const { return quatf_id; }
   /// get the inverse quaternion for rotation: WORLD->LOCAL (Identity)
   gutz::quatf getInvWorldQuat() const { return quatf_id; }
   /// get the local quaternion for rotation
   gutz::quatf getLocalQuat() const { return _camquat; }
   /// get the inverse local quaterinion for rotation
   gutz::quatf getInvLocalQuat() const { return _camquat.inv(); }
   ///@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   /// @name Get/Set Camera specific Matricies
   //@{	

   /// transform from PROJECTION space to screen space (from device normalized coords)
   ///   right now this object does not own the viewport, so.. we are just using
   ///   the viewport (0,0,w,h), need to fix that!!!
   mat4f        getViewportMatrix() const;
   mat4f        getInvViewportMatrix() const;
   //TODO: need to add true viewport functionality to this!!
   // get the viewport (x,y,w,h)<->(start,size)
   //vec4ui       getViewport() const               { return _viewport; }
   // set the viewport
   //void         setViewport(const vec4ui vp)      { _viewport = vp; }
   const mat4f &getProjectMatrix() const          { return _pm;   }
   const mat4f &getInvProjectMatrix() const       { return _pinv; }
   void         setProjectMatrix(const mat4f &m);
   const mat4f &getViewMatrix() const             { return _cm;   }
   const mat4f &getInvViewMatrix() const          { return _cinv; }
   void         setViewMatrix(const mat4f &m);
   mat4f        getInvMatrix() const              { return _pinv * _cinv; }
   quatf        getCamQuat() const                { return _camquat; }
   //@}


   ////////////////////////////////////////////////////////////
   ///@name Position & orientation
   ///@{
   gutz::vec3f getLocalPos() const    { return _eyePos; }
   void        setLocalPos(const vec3f &pos)
   {  _cm[12] = pos.x(); _cm[13] = pos.y(); _cm[14] = pos.z(); }
   gutz::mat3f getLocalOrient() const { return _cm.rot(); }
   /// TODO: implement this!!!
   void        setLocalQuat(const quatf &lq) {}
   ///@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Serialization.
   //@{

   /// write.
   virtual bool serialize(std::ostream &os, const std::string indent = std::string(""));
   /// read.
   virtual bool unserialize(std::istream &is);
   /// serialize specific parts
   /// state, first all of it, then the specific ones.
   /// serialState calls SerialEventMap & SerialParamMap & sets up tags
   virtual bool serialState(std::ostream &os, const std::string indent = std::string(""));
   virtual bool unserialState(std::istream &is);
   virtual bool serialEventMap(std::ostream &os, const std::string indent = std::string(""));
   virtual bool serialParamMap(std::ostream &os, const std::string indent = std::string(""));
   virtual bool unserialEventMap(std::istream &is);
   virtual bool unserialParamMap(std::istream &is);
   virtual bool serialData(std::ostream &os, const std::string indent = std::string(""));
   virtual bool unserialData(std::istream &is);
   ///@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Clipping planes,
   //@{
   /// Keep this current
   virtual void  setClips(float nearClip, float farClip);
   virtual gutz::vec2f getClips() const {return _clips;}
   virtual float getNearClip() const ;
   virtual float getFarClip() const ;
   //@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name LookAt functions
   //@{
   virtual void          setEyePos(vec3f eyePos);
   virtual void          setLookAt(vec3f lookat);
   virtual void          setUpVec(vec3f upvec);

   virtual const vec3f & getEyePos() const          {return _eyePos;};
   virtual const vec3f & getLookAt() const          {return _lookat;};

   virtual const vec3f  getUpVec() const           
   {
      gutz::vec4f up4 = getInvViewMatrix() * gutz::vec4f(0.0, 1.0, 0.0, 1.0) - gutz::vec4f(getEyePos(), 1.0f);
      gutz::vec3f up  = gutz::vec3f(up4.x(), up4.y(), up4.z());
      up.normalize();
      return up;
   }
   
   virtual       vec3f   getXVec()  const           
   {
      vec3f z = getViewDir();
      vec3f c = cross(z, getUpVec());
      c.normalize();
      return c;
   }
   
   virtual       vec3f   getViewDir() const        
   {
      vec3f vd = getLookAt() - getEyePos();
      vd.normalize();
      return vd;
   }
   
   virtual       float   getFocalLength() const
   {  return (getEyePos() - getLookAt()).norm(); }
   //@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Screen size functions
   ///   Keep this current.
   //@{
   void           setScreen(unsigned int x, unsigned int y);
   gutz::vec2ui   getScreen()  const         {return _screen;}
   unsigned int   getScreenX() const         {return _screen[0];};
   unsigned int   getScreenY() const         {return _screen[1];};
   //@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Projection
   /// assumes you have set clips & screen first
   //@{

   /// Sets the projection matrix based on a field of view
   virtual void         setPerspective(float fov);
   /// index names of frustum entries
   enum { FRUST_LEFT=0, FRUST_RIGHT=1, FRUST_BOTTOM=2, FRUST_TOP=3 };
   /// Sets the frustum explicitly, perspective
   virtual void         setFrustum(float left, float right, float bottom, float top);
   /// Gets the current frustum, valid wether set via Perspective, Frustum, or Ortho
   gutz::vec4f          getFrustum() const { return _frustum; }
   /// Sets the projection matrix to Orthographic
   virtual void         setOrtho(float left, float right, float bottom, float top);
   /// check if we are orthographic
   bool                 isOrtho() const { return _ortho; }
   
   //@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Project, Unproject, & space conversions
   ///@{

   /// You have a point in WORLD space and now you want it in "this" (WORLD)
   ///   manipulators space. WORLD->WORLD space, identity transform. 
   gutz::vec3f getLocalPosWorld(const gutz::vec3f &wpos) const 
   {   return wpos;  }
   /// WORLD->LOCAL dir, You have a vector (direction) in WORLD, you want it in LOCAL
   gutz::vec3f getLocalDirWorld(const gutz::vec3f &wdir) const
   {   return wdir;  }
   /// You have a LOCAL point and you want it in WORLD space, identity
   /// WORLD<-WORLD
   gutz::vec3f getWorldPosLocal(const gutz::vec3f &lpos) const
   {   return lpos;  }
   /// WORLD<-LOCAL dir, You have a vector (direction) in WORLD, you want it in LOCAL
   gutz::vec3f getWorldDirLocal(const gutz::vec3f &ldir) const
   {   return ldir;  }
   /// You have an EYE space point and want it in LOCAL space
   /// EYE->LOCAL
   gutz::vec3f getLocalPosEye(const gutz::vec3f &epos) const
   {   return _cinv.tpoint(epos);  }
   /// You have a LOCAL space point and want it in EYE space
   /// EYE<-LOCAL
   gutz::vec3f getEyePosLocal(const gutz::vec3f &lpos) const 
   {   return _cm.tpoint(lpos);   }
   /// You have a point in SCREEN space and now you want it in LOCAL/WORLD space
   /// SCREEN->WORLD
   gutz::vec3f getLocalPosScreen(const gutz::vec3f &spos) const
   {   return _cinv.tpoint(getEyePosScreen(spos));  }
   /// You have a point in LOCAL space and want it in SCREEN space
   /// SCREEN<-LOCAL, you may have to flip the y pos (screenY - pos.y)
   gutz::vec3f getScreenPosLocal(const gutz::vec3f &lpos) const
   {
      gutz::vec4f ppos = _pm * (_cm * vec4f(lpos, 1));
      ppos /= ppos.w();
      ppos.x() = (ppos.x() + 1)/2.0f * _screen[0];
      ppos.y() = (ppos.y() + 1)/2.0f * _screen[1];
      ppos.z() = (ppos.z() + 1)/2.0f; 
      return ppos;
   }
   /// Get pick pos from SCREEN (x,y,z), return in EYE space
   /// z should be in [0,1] range: 0 = nearClip, 1 = farClip \n
   inline 
   gutz::vec3f getEyePosScreen(const gutz::vec3f &scrPos) const
   {
      vec4f tmp(scrPos.x()/float(getScreenX()) *2.0f-1.0f,
                (getScreenY() - scrPos.y() - 1)/float(getScreenY()) *2.0f-1.0f,
                scrPos.z() *2.0f-1.0f,
                1.0f);
      tmp = _pinv * tmp;
      return tmp/tmp.w();
   }
   /// You have a point in SCREEN space and want it in WORLD space
   /// WORLD<-SCREEN
   gutz::vec3f getWorldPosScreen(const gutz::vec3f &spos) const
     {
       return getLocalPosScreen(spos);
     }

   ///////////////////////////////////////////////////////////
   /// Get pick pos, takes SCREEN space (x,y,z) and returns
   ///  the point in WORLD space,
   inline
   gutz::vec3f getPickPos(const gutz::vec3f &scrPos) const
   {
      return getLocalPosScreen(scrPos);
   }

   ////////////////////////////////////////////////////////////
   /// Get pick pos, takes SCREEN space (x,y) and returns point
   ///  on near clip in WORLD space,
   template<class T> inline
      gutz::vec3f  getPickPos(const T x, const T y) const
   {
      return getPickPos(gutz::vec3f(x,y,0.0f));
   }

   ////////////////////////////////////////////////////////////
   /// Get pick pos, takes SCREEN space (x,y) and returns point
   ///  on near clip in WORLD space, same as above
   template<class T> inline
      gutz::vec3f getPickPos(const gutz::vec<T,2> &p) const
   {
      return getPickPos(gutz::vec3f(p.x,p.y,0.0f));
   }

   ////////////////////////////////////////////////////////////
   /// Get the ray from the eye pointing toward a point on the
   /// near clip. x,y given in SCREEN Space, returns ray in 
   /// WORLD space. Needed since there is an algorithmic difference
   ///  between ray computation with perspective and ortho projections,
   ///  and we could like to have algorithms that don't care what 
   ///  "kind" of camera it is.  
   inline
   gutz::ray3f  getRay(float x, float y) const
   {
      if(isOrtho()) return gutz::ray3f(getPickPos<float>(x,y), getViewDir());
      gutz::vec3f srcpt = getPickPos<float>(x,y);
      gutz::vec3f vdir  = srcpt - getEyePos();
      vdir.normalize();
      return gutz::ray3f(srcpt, vdir);
   }
   /// much like getRay, but for a world space point.
   ///  returns the ray from the eye through the given point
   inline
   gutz::ray3f  getRayWorld(gutz::vec3f pos) const
   {
      if(isOrtho()) return gutz::ray3f(pos, getViewDir());
      gutz::vec3f vdir = pos - getEyePos();
      vdir.normalize();
      return gutz::ray3f(pos, vdir);
   }
   ///@}

   /////////////////////////////////////////////////////////////
   ///@name Vectors
   ///  Simmilar to Projections, but rather than projecting a
   ///  point you are projecting/deriving a direction vector.
   ///@{

   /// get a LOCAL direction (ray) through a LOCAL point from the eye 
   gutz::vec3f getLocalEyeRayLocal(const gutz::vec3f &lpos) const
   {
      if(isOrtho()) return getViewDir();
      return lpos - getWorldPos();
   }
   /// get a LOCAL direction through a SCREEN point from the eye
   gutz::vec3f getLocalEyeRayScreen(const gutz::vec3f &spos) const
   {
      if(isOrtho()) return getViewDir();
      return getLocalPosScreen(spos) - getWorldPos();
   }
   /// get a World direction through a Screen point from the eye
   gutz::vec3f getWorldEyeRayScreen(const gutz::vec3f &spos) const
   { return getLocalEyeRayScreen(spos); }
   /// get a World direction through a World point from the eye
   gutz::vec3f getWorldEyeRayWorld(const gutz::vec3f &wpos) const
   { return getLocalEyeRayLocal(wpos); }
   /// get the View direction in LOCAL space
   gutz::vec3f getLocalViewDir() const { return getViewDir(); }

   ///@}

   /// \return left position of frustum
   const float getFrustumLeft(void)   const { return getFrustum()[FRUST_LEFT]; }
   /// \return right position of frustum
   const float getFrustumRight(void)  const { return getFrustum()[FRUST_RIGHT]; }
   /// \return bottom position of frustum
   const float getFrustumBottom(void) const { return getFrustum()[FRUST_BOTTOM]; }
   /// \return top position of frustum
   const float getFrustumTop(void)    const { return getFrustum()[FRUST_TOP]; }
   /// \return height of frustum
   const float getFrustumHeight(void) const { return getFrustumTop() - getFrustumBottom(); }
   /// \return width of frustum
   const float getFrustumWidth(void)  const { return getFrustumRight() - getFrustumLeft(); }

   //////////////////////////////////////////////////////////////////////////////////////////
   /// Gets the (x,y,z) position for a position (x,y) on the "screen"
   /// The screen is the plane / polygon that is perpendicular to the camera
   /// along the view path and is nearClip away from the the eye
   //////////////////////////////////////////////////////////////////////////////////////////
   gutz::vec4f screenToWorld(float x, float y)
	{
		gutz::vec4f pos = vec4f(getEyePos(), 1.0f);
      gutz::vec4f eye = pos;
      
		pos += getNearClip() * getViewDir();
		pos += (getFrustumBottom() + getFrustumHeight() * y) * getUpVec(); 
		pos += (getFrustumLeft()   + getFrustumWidth() * x)  * getXVec();
		
		pos = eye + 2.0f * (pos - eye);
		return pos;
	}
	
   gutz::vec4f worldToScreen( gutz::vec4f pos )
	{
		//gutz::vec4f sp = worldToEyeClip( pos );
		//gutz::vec4f off(frustLeft(), frustBottom(), 0, 0);
		//gutz::vec4f scale(1/(frustRight()-frustLeft()), 1/(frustTop()-frustBottom()), 1, 1);
		
		gutz::vec4f sp = gutz::frustum(getFrustumLeft(),   getFrustumRight(),
                                     getFrustumBottom(), getFrustumTop(),
                                     getNearClip(),      getFarClip()) * getViewMatrix() * pos;
		sp /= sp.w();
		sp = sp + gutz::vec4f(1,1,0,0);
		sp = sp*gutz::vec4f(.5,.5,0,0);
		return sp;
	}
	   
#if 0   
   /// I'm pretty sure that the dome helper functions can be deleted.
   /// jbowles, 9/27/10
   ///
   /////////////////////////////////////////////////////////////
   ///@ name Dome helper functions


   /// Returns a normalized vector pointing from the camera position to the (x,y)
   /// coordinate on the screen. This is calculated using the frustum for this camera.
   gutz::vec3f screenVec(float x, float y) const;
   
	gutz::vec4f worldToEyeClip(gutz::vec4f point)
	{
		gutz::vec4f semiscreen = _cm * point;
		
		
		float intersect = gutz::intersectRayPlane(gutz::vec3f(0,0,0), gutz::vec3f(semiscreen.v), 
                                                gutz::vec3f(0,0,-getNearClip()), gutz::vec3f(0,0,-1));
		
		semiscreen *= intersect;
		semiscreen.w() = 1.0f;
		return semiscreen;
	}
	
	gutz::vec4f screenToEyeClip(float x, float y)
	{
		float newx = gutz::g_affine(0.0f, x, 1.0f, getFrustumLeft(), getFrustumRight());
		float newy = gutz::g_affine(0.0f, 1.0f - y, 1.0f, getFrustumBottom(), getFrustumTop());
		
		return gutz::vec4f(newx,newy,getNearClip(),1);
	}
	///@}
#endif
   
   ////////////////////////////////////////////////////////////
   ///@name Handle mouse events
   ///   returns false if there are no valid events.
   ///@{
   virtual void  mapEvent(unsigned int event, unsigned int button, float speed = 1.0f)
   {_keymap[button] = event; _speedmap[button] = speed;}
   unsigned int  getEvent(const MouseEvent &me) const;
   float         getSpeed(const MouseEvent &me) const;

   /// mouse up/down
   virtual bool mouse(const MouseEvent &me);
   /// move mouse
   virtual bool move(const MouseMoveEvent &mme);
   ///@}


   ////////////////////////////////////////////////////////////
   ///@name Tumble
   ///@{

   /// Update tumble "speed" steps
   virtual void tumble(float speed = 1); 
   ///@}

#ifdef KONG
   ////////////////////////////////////////////////////////////
   /// Pack/unpack details for sending the important data values 
   /// in a camera over the wire.
   unsigned int packSize(void);   // How big of a buffer do we need?
   void pack(KokoBuffer &kbuf, bool reset=true);  // Pack the camera up.
   void unpack(KokoBuffer &kbuf, bool reset=true); // Unpack into Camera.
#endif

   ////////////////////////////////////////////////////////////
   /// Create a new camera that intersects this camera's viewing frustum using
   /// a new position.
   ///
   /// @param pos   the new position of the camera in the same basis of this camera.
   /// @param radius the radius of the dome
   ////////////////////////////////////////////////////////////
   gutz::Camera::ptr domeIntersection(const gutz::vec3f& pos, const float& radius, bool fcam = false) const;

   // Pulled in from DomeLib camera to make the skate interface work
   /**
	 * Rotate camera left and right.
	 */
	
	void rotateLR(float theta)
	{
		mat4f negEye(mat3f(), -getEyePos());
		mat4f roty(mat3f(theta, getUpVec()), vec3f(0,0,0));
		mat4f forEye(mat3f(), getEyePos());
		mat4f rotCam = forEye * roty * negEye;
		
		_lookat  = rotCam * getLookAt();
		
      _upvec = roty * getUpVec();
		
      _setViewMatrix( look_at(_eyePos, _lookat, _upvec) );
	}

   void rotateUD(float theta)
	{
		//float myTheta = theta - prevTheta;
		
		mat4f negEye(mat3f(), -getEyePos());
		mat4f rotx(mat3f(theta, vec3f(getXVec().v)), vec3f(0,0,0));
		mat4f forEye(mat3f(), getEyePos());
		mat4f rotCam = forEye * rotx * negEye;
		
		//std::cout << "Rotation UP DOWN matrix : " << std::endl << rotCam << std::endl;
		
		_lookat  = rotCam * getLookAt();
		
		_upvec = rotx * getUpVec();
		
		//prevTheta = theta;
		
      _setViewMatrix( look_at(_eyePos, _lookat, _upvec) );
	}
   
   void dollyEyeAt(float d)
	{
		_lookat = _lookat + d*getViewDir();
		_eyePos = _eyePos + d * getViewDir();

      _setViewMatrix( look_at(_eyePos, _lookat, _upvec) );
	}
   
   /// Jump the camera along the up vector
   /// @param d
   ///   The distance to jump
   void jump(const float d)
   {
		_lookat = _lookat + d * getUpVec();
		_eyePos = _eyePos + d * getUpVec();
      _setViewMatrix(look_at(_eyePos, _lookat, _upvec));
   }

protected:

   void         _setProjectMatrix(const mat4f &m);
   void         _setViewMatrix(const mat4f &m);


   ///@name Behavior implementation
   ///@{
   virtual bool   handleCamera(const gutz::MouseMoveEvent &mme);

   virtual void   cameraRot(const gutz::MouseMoveEvent &mme);
   virtual void   cameraTrans(const gutz::MouseMoveEvent &mme, 
                              vec3f dxAxis, vec3f dyAxis);
   virtual void   cameraRotAxis(const gutz::MouseMoveEvent &mme,
                                vec3f dxAxis, vec3f dyAxis);
   virtual void   cameraSwivel(const gutz::MouseMoveEvent &mme);
   virtual void   cameraZoom(const gutz::MouseMoveEvent &mme);
   ///@}

   gutz::vec2ui   _screen;   ///<screen size
   ///   right now this object does not own the viewport, so.. we are just using
   ///   the viewport (0,0,w,h), need to fix that!!!, should keep all info 
   ///   relative to the screen in this variable not _screen!!!!!!!!!!!!
   gutz::vec4ui   _viewport; ///<viewport

   gutz::vec4f        _frustum;         ///< view frustum

   gutz::vec2f    _clips;               ///< Clipping planes

   mat4f        _cm, _pm, _cinv, _pinv; ///< Camera & projection matrices

   vec3f        _eyePos,_lookat,_upvec; ///< Current Lookat parameters

   quatf        _camquat;               ///< Quaternions for rotation

   /// "temporary" vectors for events
   vec3f        _lastEyePos,_lastLookat;
   /// "temporary" quaternions for tumbling (track is a bad name joe!!)
   quatf        _camtrackquat, _swivtrackquat;

   bool         _ortho;                 ///< are we orthographic or perspective

   ////////////////////////////////////////////////////////////
   // key 2 action mapers
   EventKeyMap    _keymap;              ///< maps buttons/keys to events
   EventParamMap  _speedmap;            ///< speed for some event

private:

};

////////////////////////////////////////////////////////////////////////////
///  List of Cameras, typedefined for convenience
////////////////////////////////////////////////////////////////////////////

typedef SmartPtr<Camera>              CameraSP;

typedef std::list<Camera *>           CameraList;
typedef std::list<Camera *>::iterator CameraListIter;

typedef std::vector<Camera *>           CameraVec;
typedef std::vector<Camera *>::iterator CameraVecIter;
   
}//end namespace gutz

#endif
