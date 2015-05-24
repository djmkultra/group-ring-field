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

//manip.h
//Joe Kniss

#ifndef __MANIP_DOT_H
#define __MANIP_DOT_H

#include <mathGutz.h>
#include <signalGutz.h>
#include <map>
#include <list>
#include "baseManip.h"
#include "Camera.h"
#include "ManipEventBase.h"

#ifdef KONG
class KokoBuffer;
#endif

namespace gutz {


//////////////////////////////////////////////////////////////
/// Interaction Modes supported by this class, object actions
//////////////////////////////////////////////////////////////
enum{ //Supported modes, camera Actions
   OBJ_MODE_NONE = CAM_LAST_MODE + 1,       ///< Mode not mapped
   OBJ_CENT_ROT,        ///< Rotates object around its center
   OBJ_ABS_ROT,         ///< Rotates object around the origin of its space
   OBJ_STRANS_XY,       ///< Screen space object translate X,Y plane
   OBJ_STRANS_XZ,       ///< Screen space object translate X,Z plane
   OBJ_STRANS_ZY,       ///< Screen space object translate Y,Z plane
   OBJ_ATRANS_01,       ///< Translate in the plane defined by a custom vector 01
   OBJ_ATRANS_02,       ///< Translate in the plane defined by a custom vector 01
   OBJ_ATRANS_03,       ///< Translate in the plane defined by a custom vector 01
   OBJ_ATRANS_04,       ///< Translate in the plane defined by a custom vector 01
   OBJ_ATRANS_05,       ///< Translate in the plane defined by a custom vector 01
   OBJ_LAST_MODE
};

////////////////////////////////////////////////////////////////////////////
///   A Manipulator interaction class. (include graphicsGutz.h)
/// 
///   A Manipluator interaction class. Handles basic operations relating to
///   object manipulation.  The Camera is automatically set if the 
///   gutz::MouseEvent contains a valid camera, which it should if it was
///   correctly created. 
///   
///  Implementation notez (users of this class can ignore this stuff): \n
///    Manipulators are composed of 3 basic transforms, a "baked" transform (B)
///    representing all translation and rotation up to the current rotation (R)
///    around the "center of rotation" (C).  Here is how we generate the local 
///    transform (LM): \n
///    LM = B * C^{-1} * R * C \n
///    The matrix B is what I call the "baked" matrix, so called because
///    when the center of rotation changes, we "bake" the previous
///    C^{-1}*R*C into B, ie B = B*C^{-1}*R*C.  It is also key when the 
///    user sets the matrix, there may be no way to recover a pure rotation
///    about our current center of rotation. In this case we just set it to B
///    and set R to identity.  C is the transform that takes the local coordinate
///    frame to the center of rotation, ie -(minus)"center of rotation".  R is 
///    the rotation around that center of rotation; it is stored as a quaternion
///    accessed through getCenterQuat() and setCenterQuat().  \n
///
///  Events implementing rotation need to be aware of the relationships described
///   above.  Since B is applied before R, you need to do the appropriate 
///   change of basis to assign/modify R.  Remember that B includes rotation.  Right
///   now, rotations are generated with respect to the current view.  There is a 
///   function calld getCamQuat() that returns a quaternion representing all 
///   rotation from EYE space to this local coordinate frame, including rotations
///   in B, but not R.  This means that you can multiply R by another quaternion
///   Q (defined in EYE space) like this: \n
///   R' = CQ^{-1} * Q * CQ * R \n
///   Where CQ is the camera quaternion.  The part of the equation: \n
///   CQ^{-1} * Q * CQ \n
///   is the rotation Q in the local coordinate frame, it expresses the change of
///   basis to get Q into the local frame.  This quaternion can be stored and 
///   applied during tumbling.  Take a look at gutz::RotateManipEvent to see 
///   a reference implementation of this process.  
///
///  At some point, I want to lighten up this object; right now it is carying
///   a lot of data.  I am caching the matrix and its inverse since I assume
///   that the user should have access to these, and that it be very fast.
////////////////////////////////////////////////////////////////////////////

/// see ManipEventBase.h for the declaration of EventMap
class Manip : public BaseManip, public EventMap<Manip> {
public:
    /// EventMap is declared in ManipEventBase.h
    typedef EventMap<Manip> ManipEventMap;
    typedef ManipEventBase<Manip> ManipEvent;
    typedef gutz::SmartPtr<ManipEvent> ManipEventSP;


   HAS_SLOTS;

   /// if a Camera is passed, the parent is automatically set to the camera
   Manip(Camera *cam = 0);
   /// copy constructor, copies everything, including the parent and events
   Manip(const Manip &m);
   /// assignment, copies everything but events, including the parent
   Manip &operator=(const Manip &m);

   virtual ~Manip(){}

   ///@name RTTI
   ///@{
   bool isCamera() const { return false; }
   bool isManip()  const { return true;  }
   ///@}
    
   ////////////////////////////////////////////////////////////
   ///@name Key Transform Parameters
   ///@{
   
   /// What is the center of rotation, defaults to (0,0,0), in LOCAL space
   vec3f  getCenter() const              { return _center; }
   /// Set the center of rotation & scale, in LOCAL space
   void   setCenter(const vec3f &center);
   /// get the quaternion associated with rotation around the center
   gutz::quatf getCenterQuat() const          { return _currquat; }
   /// set the quaternion associated with rotation around the center
   void        setCenterQuat(const quatf &cq) { _currquat = cq; updateMatrix();}
   /// What is the radius of rotation, returns 0.0 if unset
   float  getRad() const { return _userRad; }
   /// Set the radius of rotation, set to 0.0 if you want default radius behavior
   void   setRad(float rad) { _userRad = rad; }
   /// Get the radius for an event, specifically this returns the 
   ///  default radius if we ONLY have screen position, and no depth,
   ///  if we have depth, and no user defined radius, we guess the radius based
   ///  on the pick point and center of rotation.  
   float  getRad(const gutz::MouseEvent &me);

   ///@}

   ////////////////////////////////////////////////////////////
   ///@name Transformations
   ///@{
   
   /// Get Matrix.
   /// Get the 4x4 homogenious transformation matrix, equiv to getLocalTransform()
   /// for gl apps, you set the manip, by:
   ///  glMultMatrixf( manip.getMatrix() );
   ///   
   mat4f getMatrix() const         {return _om;}
   mat4f getInvMatrix() const      {return _oinv;}
   void  setMatrix(const gutz::mat4f &m); 

   /// get the projection transform, PROJECTION<-LOCAL
   virtual gutz::mat4f getProjectTransform() const
   { if(_parent) return _parent->getProjectTransform() * _om; return _om; }
   /// get the inverse projection transform, PROJECTION->LOCAL
   virtual gutz::mat4f getInvProjectTransform() const
   { if(_parent) return _oinv * _parent->getInvProjectTransform(); return _oinv; }
   /// get the screen transform, SCREEN<-PROJECT
   ///  this is wierd because you must apply the Project transform,
   ///  then divide by w, then apply the screen transform.  
   virtual gutz::mat4f getScreenTransform() const
   { if(_parent) return _parent->getScreenTransform(); return mat4f_id; }
   /// get the inverse screen transform, SCREEN->PROJECT
   virtual gutz::mat4f getInvScreenTransform() const
   { if(_parent) return _parent->getInvScreenTransform(); return mat4f_id; }
   /// get the eye transform, EYE<-LOCAL
   gutz::mat4f getEyeTransform() const
   { if(_parent) return _parent->getEyeTransform() * _om; return _om; }
   /// get the inverse eye transform: EYE->LOCAL
   gutz::mat4f getInvEyeTransform() const
   { if(_parent) return _oinv * _parent->getInvEyeTransform(); return _oinv; }
   /// get the inverse world transform: WORLD->LOCAL
   virtual gutz::mat4f getInvWorldTransform() const
   { if(_parent) return _oinv * _parent->getInvWorldTransform(); return _oinv; }
   /// get the world transform: WORLD<-LOCAL
   gutz::mat4f getWorldTransform() const
   { if(_parent) return _parent->getWorldTransform() * _om; return _om; }
   /// get the transform that this manipulator defines
   gutz::mat4f getLocalTransform() const { return _om; }
   /// get the inverse transform that this manipulator defines
   gutz::mat4f getInvLocalTransform() const { return _oinv; }
   /// get the projection matrix currently associated with this transform chain
   gutz::mat4f getProjection() const
   { if(_parent) return _parent->getProjection(); return mat4f_id; }
   /// get the inverse projection matrix (inverse of getProjection())
   gutz::mat4f getInvProjection() const
   { if(_parent) return _parent->getInvProjection(); return mat4f_id; }
   /// get the quaternion that expresses rotation: EYE<-LOCAL
   gutz::quatf getEyeQuat() const
   { if(_parent) return _parent->getEyeQuat().mult(getLocalQuat()); return getLocalQuat(); }
   /// get the inverse quaternion for rotation: EYE->LOCAL
   gutz::quatf getInvEyeQuat() const
   { 
      return getEyeQuat().conj();
   }
   /// get the quaternion for rotation: WORLD<-LOCAL
   gutz::quatf getWorldQuat() const
   { if(_parent) _parent->getWorldQuat().mult(getLocalQuat()); return getLocalQuat(); }
   /// get the inverse quaternion for rotation: WORLD->LOCAL
   gutz::quatf getInvWorldQuat() const
   { if(_parent) 
       return getInvLocalQuat().mult(_parent->getInvWorldQuat()); 
     return getInvLocalQuat();
   }
   /// get the local quaternion for rotation
   gutz::quatf getLocalQuat() const { return quatf( _om.rot() ); }
   /// get the inverse local quaterinion for rotation
   gutz::quatf getInvLocalQuat() const { return getLocalQuat().conj(); }

   ///@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Position, orientation, and scale
   ///@{
   gutz::vec3f getLocalPos() const    { return _om.getTrans(); }
   void        setLocalPos(const vec3f &pos);
   gutz::mat3f getLocalOrient() const { return _om.rot(); }
   void        setLocalQuat(const quatf &lq);
   void        setLocalOrient(float angle, vec3f axis);
   gutz::vec3f getLocalScale() const { return _scale; }
   /// set non uniform scale
   void        setLocalScale(const vec3f &scale);
   void        setLocalScale(const vec3f &scale, const vec3f center);
   /// set uniform scale
   void        setLocalScale(float scale); 
   /// DEFINED IN baseManip:
   //
   ///  gutz::vec3f  getWorldPos() const    
   ///  void         setWorldPos(const vec3f &wpos)    
   ///  gutz::mat3f  getWorldOrient() const 
   //
   ///@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Tumble. 
   ///  DEFINED IN baseManip
   ///@{
   ///  void      setTumble(bool on){ _tumble = on;}
   ///  bool      getTumble() const { return _tumble; }
   /// Update tumble "speed" amout
   ///  virtual void tumble(float speed = 1){} 
   ///@}
   /////////////////////////////////////////////////////////////
   
   ////////////////////////////////////////////////////////////
   ///@name Tranformation chains
   ///   Basically implements a linked list of manipulators that
   ///   can be strung together.  If you set your parent to be 0,
   ///   then you effectively nuke the chain above this manipulator.
   ///   Cameras are special, there can only be one, and it is always
   ///   at the very top of the chain.
   ///@{
   /// adds parent above this transformation (head of the transform chain)
   void        setParent(const BaseManipSP parent);
   void        setParent(BaseManip *parent) {setParent(BaseManipSP(parent));}
   void        setParent(const gutz::SmartPtr<Manip> & parent)
   {  setParent( BaseManipSP( parent.getPtr() ) ); }
   BaseManipSP getParent() const { return _parent; }
   /// inserts parent just above this one
   void        insertParent(const BaseManipSP parent);
   /// replaces parent with our parent's parent.
   void        removeParent()
   { if(_parent) _parent = _parent->getParent(); else _parent = 0; }
   ///@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Set/Get Camera,
   /// Sets the camera so interactions are relative to the
   ///   current view.
   ///@{
   virtual void  setCamera(const gutz::CameraSP cam)
   { insertParent(BaseManipSP(cam)); }
   Camera       *getCamera() const;

   /// get Camera Quaternions, the camera is simply all rotations
   ///   above this object in the chain including any baked rotation: ROT_OF(EYE<-LOCAL).
   /// comes in handy for Events using trackballs, since this is all rotation up
   /// to the "centerQuat()".  
   quatf  getCamQuat()
   {
      if(_parent) return _parent->getEyeQuat().mult( quatf( _baked.rot() ));
      return quatf( _baked.rot() );
   }
   /// get Inverse Camera Quaternion, the camera is simply all 
   ///   rotation above this object in the chain: ROT_OF(EYE->LOCAL)
   quatf getInvCamQuat()
   {  
      return getCamQuat().conj();
   }
   ///@}

   ////////////////////////////////////////////////////////////
   /// @name Screen size functions
   //@{
   gutz::vec2ui getScreen()  const 
   { if(_parent) return _parent->getScreen(); return gutz::vec2ui(0,0);}
   unsigned int getScreenX() const 
   { if(_parent) return _parent->getScreenX(); return 0;}
   unsigned int getScreenY() const 
   { if(_parent) return _parent->getScreenY(); return 0;}
   //@}

   ////////////////////////////////////////////////////////////
   
   ////////////////////////////////////////////////////////////
   ///@name Projections
   ///   Given either a "SCREEN", "EYE", "WORLD", or "LOCAL" space pos
   ///@{

   /// Forward transform
   gutz::vec3f getParentPosLocal(const gutz::vec3f &lpos) const
     {
       return _om.tpoint(lpos);
     }
   /// Inverse transform
   gutz::vec3f getLocalPosParent(const gutz::vec3f &ppos) const
     {
       return _oinv.tpoint(ppos);
     }

   /// You have a point in WORLD space and now you want it in LOCAL space
   /// WORLD->LOCAL
   gutz::vec3f getLocalPosWorld(const gutz::vec3f &wpos) const
   {
      if(_parent) return _oinv.tpoint(_parent->getLocalPosWorld(wpos));
      return _oinv.tpoint(wpos);
   }
   /// WORLD->LOCAL dir, You have a vector (direction) in WORLD, you want it in LOCAL
   gutz::vec3f getLocalDirWorld(const gutz::vec3f &wdir) const
   {
      if(_parent) return _oinv.tdir( _parent->getLocalDirWorld(wdir) );
      return _oinv.tdir( wdir );
   }
   /// You have a LOCAL point and you want it in WORLD space
   /// WORLD<-LOCAL
   gutz::vec3f getWorldPosLocal(const gutz::vec3f &lpos) const
   {
      if(_parent) return _parent->getWorldPosLocal( _om.tpoint(lpos) );
      return _om.tpoint(lpos);
   }
   /// WORLD<-LOCAL dir, You have a vector (direction) in WORLD, you want it in LOCAL
   gutz::vec3f getWorldDirLocal(const gutz::vec3f &ldir) const
   {
      if(_parent) return _parent->getWorldPosLocal( _om.tdir( ldir ) );
      return _om.tdir( ldir );
   }
   /// You have an EYE space point and want it in LOCAL space
   /// EYE->LOCAL
   gutz::vec3f getLocalPosEye(const gutz::vec3f &epos) const
   {
      if(_parent) return _oinv.tpoint(_parent->getLocalPosEye(epos));
      return _oinv.tpoint(epos);
   }
   /// You have a LOCAL space point and want it in EYE space
   /// EYE<-LOCAL
   gutz::vec3f getEyePosLocal(const gutz::vec3f &lpos) const
   {
      if(_parent) return _parent->getEyePosLocal(_om.tpoint(lpos));
      return _om.tpoint(lpos);
   }
   /// transform a SCREEN space pos to LOCAL pos (by the inv model view)
   /// SCREEN->LOCAL
   gutz::vec3f getLocalPosScreen(const gutz::vec3f &spos) const
   {
      if(_parent) return _oinv.tpoint(_parent->getLocalPosScreen(spos));
      return _oinv.tpoint(spos);
   }
   /// You have a point in LOCAL space and want it in SCREEN space
   /// SCREEN<-LOCAL
   gutz::vec3f getScreenPosLocal(const gutz::vec3f &lpos) const
   {
      if(_parent) return _parent->getScreenPosLocal( _om.tpoint(lpos) );
      return _om.tpoint(lpos);
   }
   /// You have a point in SCREEN space and want it in WORLD space
   /// WORLD<-SCREEN
   gutz::vec3f getWorldPosScreen(const gutz::vec3f &spos) const
     {
       if(_parent) return _parent->getScreenPosLocal( spos );
       return spos;
     }
   ///@}
   ////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////
   ///@name Vectors
   ///  Simmilar to Projections, but rather than projecting a
   ///  point you are projecting/deriving a direction vector.
   ///@{

   /// get a LOCAL direction (ray) through a LOCAL point from the eye 
   gutz::vec3f getLocalEyeRayLocal(const gutz::vec3f &lpos) const
   { 
      if(_parent) 
        return _oinv.tdir( _parent->getLocalEyeRayLocal( _om.tpoint(lpos) ) );
      return lpos;
   }
   /// get a LOCAL direction through a SCREEN point from the eye
   gutz::vec3f getLocalEyeRayScreen(const gutz::vec3f &spos) const
   {
      if(_parent)
         return _oinv.tdir( _parent->getLocalEyeRayScreen( spos ) );
      return spos;
   }
   /// get a World direction through a Screen point from the eye
   gutz::vec3f getWorldEyeRayScreen(const gutz::vec3f &spos) const
   {  if(_parent) return _parent->getWorldEyeRayScreen(spos); return spos; }
   /// get a World direction through a World point from the eye
   gutz::vec3f getWorldEyeRayWorld(const gutz::vec3f &wpos) const
   {  if(_parent) return _parent->getWorldEyeRayWorld(wpos); return wpos; }
   /// get the View direction in LOCAL space
   gutz::vec3f getLocalViewDir() const 
   { if(_parent) return getInvEyeTransform().tdir(vec3f_z); 
     return vec3f_z;
   }

   ///@}
   /////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Mouse Events
   ///@{
   virtual bool mouse(const MouseEvent &me);
   virtual bool move(const MouseMoveEvent &mme);
   ///@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Tumble
   ///  Update speed amount
   ///@{
   virtual void tumble(float speed = 1);
   ///@}
   ////////////////////////////////////////////////////////////

#ifdef KONG
   ////////////////////////////////////////////////////////////
   /// Pack/unpack details for sending the important data values 
   /// in a camera over the wire.
   unsigned int packSize(void);   // How big of a buffer do we need?
   void pack(KokoBuffer &kbuf, bool reset=true);   // Pack the manip up.
   void unpack(KokoBuffer &kbuf, bool reset=true); // Unpack into manip.
   void print(void); 
#endif


   ////////////////////////////////////////////////////////////
   /// Serialization.
   /// write.
   virtual bool serialize(std::ostream &os, const std::string indent = std::string(""));
   /// read.
   virtual bool unserialize(std::istream &is);
   /// serialize the data parts of this guy for momentos & transmission
   virtual bool serialData(std::ostream &os, const std::string indent = std::string(""));
   virtual bool unserialData(std::istream &is);



protected:

   void   updateMatrix();
   mat4f  buildMatrix() const;

   ////////////////////////////////////////////////////////////
   // Protected data

   float                  _rad;
   float                  _userRad;
   vec3f                  _center;
   mat4f                  _baked;

   vec3f                  _scale;

   mat4f _om, _oinv; /// eventually we could nuke these, they are just cached

   quatf _currquat;
   quatf _objtrackquat; /// nuke this!!

   BaseManipSP _parent;   ///< who is above us in the transform chain?
};

typedef SmartPtr<Manip>     ManipSP;

/// EventMap is declared in ManipEventBase.h
typedef EventMap<Manip> ManipEventMap;
typedef ManipEventBase<Manip> ManipEvent;
typedef gutz::SmartPtr<ManipEvent> ManipEventSP;


} //end namespace gutz


#endif

