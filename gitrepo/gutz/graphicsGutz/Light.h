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
//------------------------------------------------------------------------
//C++ 
//   
//                   ________    ____   ___ 
//                  |        \  /    | /  /
//                  +---+     \/     |/  /
//                  +--+|  |\    /|     < 
//                  |  ||  | \  / |  |\  \            .
//                  |      |  \/  |  | \  \            .
//                   \_____|      |__|  \__\            .
//                       Copyright  2002 
//                      Joe Michael Kniss
//               "All Your Base are Belong to Us"
//-------------------------------------------------------------------------

//Light.h

#ifndef __LIGHT_DOT_H
#define __LIGHT_DOT_H

#include <Camera.h>

namespace gutz {

////////////////////////////////////////////////////////////////////////////
///   A Light interaction class. 
/// 
///   A Light interaction class. Handles basic operations relating to
///   light placement. Be sure to keep the clipping planes and screen 
///   size current.  This class is a child of Camera, so interaction
///   is identical to the cameras
////////////////////////////////////////////////////////////////////////////

class Light : public Camera {
public:
   Light();

   virtual ~Light();

   ////////////////////////////////////////////////////////////
   /// Serialization.
   /// write.
   virtual bool serialize(std::ostream &os, const std::string indent = std::string("")) { return false;}
   /// read.
   virtual bool unserialize(std::istream &is) { return false;}
};

////////////////////////////////////////////////////////////////////////////
///  List of Lights, typedefined for convenience
////////////////////////////////////////////////////////////////////////////

typedef gutz::SmartPtr<Light>        LightSP;

typedef std::list<LightSP>           LightList;
typedef std::list<LightSP>::iterator LightListIter;

typedef std::vector<LightSP>           LightVec;
typedef std::vector<LightSP>::iterator LightVecIter;

} //end namespace gutz

#endif

