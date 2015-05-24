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

//baseManip.h


#ifndef __BASE_MANIP_DOT_H
#define __BASE_MANIP_DOT_H


#include <map>
#include <mathGutz.h>
#include <gutzKeyMouse.h>
#include <iostream>
#include <smartptr.h>

//#pragma warning(disable:4786) //dissable stl identifier too long warnings

namespace gutz {

/// forward decl
class MouseEvent;
class MouseMoveEvent;
class Camera;

////////////////////////////////////////////////////////////////////////////
/// Common functionality between manipulators, including cameras and 
/// lights.
///
/// Manipulators and Cameras handle transformations and interactions with
/// objects/camera.  We define several important spaces:
/// - SCREEN SPACE:  xdim = (0-SX) ydim = (0-SY) zdim=(0-1)
/// - PROJECTION SPACE: the cannonical view volume (-1,1)(-1,1)(-1,1)
/// - EYE SPACE: Pre-projection, center at (0,0,0) view direction = Z AXIS, up = Y AXIS
/// - WORLD SPACE: Pre-view matrix, the "reference space" independent of view
/// - MODEL/LOCAL SPACE: Space objects are defined in, defined by a Transform Chain
///
/// Here is how a MODEL/LOCAL space point is projected to the screen: reads 
/// from right to left\n
/// SCREEN<-(divide w)PROJECTION(camera)<-EYE(camera/view)<-WORLD<-MANIP(s...)<-point \n
/// That is, the point is multiplied by all Manip objects in chain to get
/// it into WORLD space, then it is multiplied by the camera's view matrix to
/// get it into EYE space, then it is multiplied by the camera's projection
/// matrix to get it into PROJECTION space, finally a Viewport transform is 
/// applied to get it into SCREEN space.  The inverse of this transform takes
/// a screen space point into world space.  Notice that after we apply the 
/// projection matrix we must divide all components by the "w" component of the
/// resulting position, only then can we apply the screen transform.  You should 
/// notice that in the Transform functions you can get the total transform from
/// "LOCAL" space to "PROJECTION" space but not to "SCREEN" space, this is because
/// you need to divide by "w" to get into projection space.  This is why you 
/// can only get the matrix from SCREEN<-PROJECTION and visa/versa; you have two 
/// steps for a transformation from SCREEN<-LOCAL:
/// \code
///  vec3f myPt = ....; /// transform a local point to the screen
///  myPt = getProjectionTransform() * myPt; /// apply PROJECTION<-LOCAL
///  myPt /= myPt.w                          /// divide by w
///  myPt = getScreenTransform() * myPt;     /// apply SCREEN<-PROJECTION
///  /// now myPt is in screen space
/// \endcode
/// The same process works for going from screen space to local.
///
/// Transformation chains are constructed by setting a manipulators parent to
/// be another BaseManip, that means that the manipulator "lives" in it's 
/// parents space.  Its parent could either be another manipulator or a Camera.
/// Cameras can only be attached at the top of a transformation chain. See 
/// gutz::Manip for the interface for managing chains. 
////////////////////////////////////////////////////////////////////////////

class BaseManip : public Counted {
public:
   
   typedef gutz::SmartPtr<BaseManip> BaseManipSP;

   virtual ~BaseManip(){}

   ////////////////////////////////////////////////////////////
   ///@name RTTI (Run-Time Type Information)
   ///@{
   virtual bool isCamera() const = 0;
   virtual bool isManip()  const = 0;
   ///@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///Tranformation chains (for Manip) interface
   ///   Manip basically implements a linked list of manipulators that
   ///   can be strung together.  If you set your parent to be 0,
   ///   then you effectively nuke the chain above this manipulator.
   ///   Cameras are special, there can only be one, and it is always
   ///   at the very top of the chain (ie has no parent).
   ///   See gutz::Manip for the rest of this interface
   virtual gutz::SmartPtr<BaseManip> getParent() const 
   {    return gutz::SmartPtr<BaseManip>(0);   }
   virtual void                      setParent(const BaseManipSP parent) {}
   virtual void                      insertParent(const BaseManipSP parent){}
   virtual Camera*                   getCamera() const = 0;
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Screen size functions
   ///   Valid (non zero) only if this object is a Camera, or
   ///   if this is a Manip, the Camera has either been set
   ///   during a mouse event or by the user.
   //@{
   virtual gutz::vec2ui getScreen()  const = 0;
   virtual unsigned int getScreenX() const = 0;
   virtual unsigned int getScreenY() const = 0;
   //@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Transformations
   ///@{

   /// get the projection transform, PROJECTION<-LOCAL
   virtual gutz::mat4f getProjectTransform() const = 0;
   /// get the inverse projection transform, PROJECTION->LOCAL
   virtual gutz::mat4f getInvProjectTransform() const = 0;
   /// get the screen transform, SCREEN<-PROJECT
   ///  this is weird because you must apply the Project transform,
   ///  then divide by w, then apply the screen transform.  
   virtual gutz::mat4f getScreenTransform() const = 0;
   /// get the inverse screen transform, SCREEN->PROJECT
   virtual gutz::mat4f getInvScreenTransform() const = 0;
   /// get the eye transform, EYE<-LOCAL,
   virtual gutz::mat4f getEyeTransform() const = 0;
   /// get the inverse eye transform: EYE->LOCAL
   virtual gutz::mat4f getInvEyeTransform() const = 0;
   /// get the world transform, WORLD<-LOCAL
   virtual gutz::mat4f getWorldTransform() const = 0;
   /// get the inverse world transform: WORLD->LOCAL
   virtual gutz::mat4f getInvWorldTransform() const = 0;
   /// get the LOCAL transform that this manipulator defines
   virtual gutz::mat4f getLocalTransform() const = 0;
   /// get the inverse transform that this manipulator defines
   virtual gutz::mat4f getInvLocalTransform() const = 0;
   /// get the projection matrix currently associated with this transform chain
   virtual gutz::mat4f getProjection() const = 0;
   /// get the inverse projection matrix (inverse of getProjection())
   virtual gutz::mat4f getInvProjection() const = 0;
   /// get the quaternion that expresses the total rotation: EYE<-LOCAL
   virtual gutz::quatf getEyeQuat() const = 0;
   /// get the inverse quaternion for rotation: EYE->LOCAL
   virtual gutz::quatf getInvEyeQuat() const = 0;
   /// get the quaternion for rotation: WORLD<-LOCAL
   virtual gutz::quatf getWorldQuat() const = 0;
   /// get the inverse quaternion for rotation: WORLD->LOCAL
   virtual gutz::quatf getInvWorldQuat() const = 0;
   /// get the local quaternion for rotation
   virtual gutz::quatf getLocalQuat() const = 0;
   /// get the inverse local quaterinion for rotation
   virtual gutz::quatf getInvLocalQuat() const = 0;

   ///@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Position & orientation
   ///@{
   virtual gutz::vec3f getLocalPos() const = 0;
   virtual void        setLocalPos(const vec3f &pos) = 0;
   virtual gutz::mat3f getLocalOrient() const = 0;
   virtual void        setLocalQuat(const quatf &lq) = 0;
   
   //  ---------------------------------------------------------
   /// Get WORLD POS
   gutz::vec3f  getWorldPos() const 
   { if( getParent() ) return getParent()->getWorldPosLocal( getLocalPos() );
     return getLocalPos(); 
   }
   
   //  ---------------------------------------------------------
   /// Set WORLD POS (set the local pos using a world-space pos 
   virtual void setWorldPos(const vec3f &wpos) 
   { 
      if( getParent() ) 
      {  setLocalPos( getParent()->getLocalPosWorld(wpos) ); return; }
      setLocalPos( wpos ); 
   }
   
   //  ---------------------------------------------------------
   /// Get WORLD ORIENT
   gutz::mat3f getWorldOrient() const 
   {   return mat3f(getWorldQuat()) * getLocalOrient(); }
   ///@}
   ////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Events 
   /// Maps buttons & modifiers to actions, speed determines
   ///   the rate of the action for that button, state combo.
   ///   Concrete class, camera, object, light etc... will 
   ///   define valid actions.
   ///@{
   virtual bool mouse(const MouseEvent &me) = 0;
   virtual bool move(const MouseMoveEvent &mme) = 0;
   ///@}
   /////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Tumble. 
   ///   enable/disable rotations with inertia
   ///@{
   void      setTumble(bool on){ _tumble = on;}
   bool      getTumble() const { return _tumble; }
   /// Update tumble "speed" amout
   virtual void tumble(float speed = 1){} 
   ///@}
   /////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Projections: Mapping a point from one space to another
   ///   Given either a "WORLD", "EYE", "SCREEN", or "LOCAL" space pos.
   ///   Read it like this: get{A}Pos{B} means "map a 
   ///   point to space {A} from space {B} (ie A<-B)"
   ///@{
   
   /// WORLD->LOCAL, You have a point in WORLD space and now you want it in LOCAL space
   virtual gutz::vec3f getLocalPosWorld(const gutz::vec3f &wpos) const = 0;
   /// WORLD->LOCAL dir, You have a vector (direction) in WORLD, you want it in LOCAL
   virtual gutz::vec3f getLocalDirWorld(const gutz::vec3f &wdir) const = 0;
   /// WORLD<-LOCAL, You have a LOCAL point and you want it in WORLD space
   virtual gutz::vec3f getWorldPosLocal(const gutz::vec3f &lpos) const = 0;
   /// WORLD<-LOCAL dir, You have a vector (direction) in WORLD, you want it in LOCAL
   virtual gutz::vec3f getWorldDirLocal(const gutz::vec3f &ldir) const = 0;
   /// EYE->LOCAL, You have an EYE space point and want it in LOCAL space
   virtual gutz::vec3f getLocalPosEye(const gutz::vec3f &epos) const = 0;
   /// EYE<-LOCAL, You have a LOCAL space point and want it in EYE space
   virtual gutz::vec3f getEyePosLocal(const gutz::vec3f &lpos) const = 0;
   /// SCREEN->LOCAL, You have a point in SCREEN space and now you want it in "this" space
   virtual gutz::vec3f getLocalPosScreen(const gutz::vec3f &spos) const = 0;
   /// SCREEN<-LOCAL, You have a point in LOCAL space and want it in SCREEN space
   virtual gutz::vec3f getScreenPosLocal(const gutz::vec3f &lpos) const = 0;
   /// SCREEN->WORLD
   virtual gutz::vec3f getWorldPosScreen(const gutz::vec3f &spos) const = 0;
   ///@}

   /////////////////////////////////////////////////////////////
   ///@name Vectors
   ///  Simmilar to Projections, but rather than projecting a
   ///  point you are projecting/deriving a direction vector.
   ///  This is needed since our Camera class supports both perspective
   ///  and orthographic projections, this affects how we view rays
   ///  "from the eye" through any other point, in ortho the direction
   ///  is always the same, in perspective, the direction depends on 
   ///  the eye's position.
   ///@{

   /// get a LOCAL direction (ray) through a LOCAL point from the eye 
   virtual gutz::vec3f getLocalEyeRayLocal(const gutz::vec3f &lpos) const = 0;
   /// get a LOCAL direction through a SCREEN point from the eye
   virtual gutz::vec3f getLocalEyeRayScreen(const gutz::vec3f &spos) const = 0;
   /// get a World direction through a Screen point from the eye
   virtual gutz::vec3f getWorldEyeRayScreen(const gutz::vec3f &spos) const = 0;
   /// get a World direction through a World point from the eye
   virtual gutz::vec3f getWorldEyeRayWorld(const gutz::vec3f &wpos) const = 0;
   /// get the View direction in LOCAL space
   virtual gutz::vec3f getLocalViewDir() const = 0;

   ///@}
   /////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////
   ///@name Serialization. returns true if fail, false if success
   ///@{

   ///  write.
   virtual bool serialize(std::ostream &os, const std::string indent = std::string(""));
   /// read.
   virtual bool unserialize(std::istream &is);
   /// specific parts of this object
   /// serialize the data parts of this guy for momentos & transmission
   virtual bool serialData(std::ostream &os, const std::string indent = std::string(""));
   virtual bool unserialData(std::istream &is);
   ///@}
   ////////////////////////////////////////////////////////////

protected:

   BaseManip()
      : _tumble(false)
   {}
   /// copy constructor copies data & behavior
   BaseManip(const BaseManip &bm)
      : _tumble(bm._tumble)
   {}
   /// assignment doesn't copy behavior, just data!
   BaseManip &operator=(const BaseManip &bm)
   { return *this; }

   ////////////////////////////////////////////////////////////
   // protected data
   ////////////////////////////////////////////////////////////

   //bool           _on;                  ///< Drawing on???
   bool           _tumble;              ///< Tumble on???

   //unsigned int   _currentButtons;      ///< Currently active buttons

   unsigned int   _currentMode;         ///< Current mode
   float          _currentSpeed;        ///< Current speed

   //float          _lastX, _lastY;      ///< Last mouse positions
   float          _lastd;               ///< Last distance traveled

};

typedef gutz::SmartPtr<BaseManip> BaseManipSP;

} //end namespace gutz

#endif

