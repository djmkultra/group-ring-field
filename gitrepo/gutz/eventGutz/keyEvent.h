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

//keyEvent.h

#ifndef __GUTZ_KEY_EVENT_DOT_H
#define __GUTZ_KEY_EVENT_DOT_H

#include "renderEvent.h"
#include "mouseEvent.h"

namespace gutz {

/// KeyEvent, a key was pressed.
///  This object is a MouseEvent since the mouse position at the time of the
///  click might be of importantce.  A MouseEvent is a CameraEvent since we 
///  need the camera for the the mouse position to make sence.
class KeyEvent :  public MouseEvent {
public:
   KeyEvent(unsigned int key, const gutz::vec3f &pos, bool down, unsigned int button,
            const CameraSP &cam, const ManipSP &manip)
            : MouseEvent(pos,down,button,cam,manip), _key(key) 
   {}
   KeyEvent(unsigned int key, const MouseEvent &me)
      : MouseEvent(me), _key(key)
   {}
   KeyEvent(const KeyEvent &ke)
      : MouseEvent(ke), _key(ke._key)
   {}
   virtual ~KeyEvent() {}

   ///@name get/set Key 
   ///@{
   
   char getAsci() const { return char( _key & (unsigned int)255 ); }
   void setAsci(char k) { _key = ( _key & (~(unsigned int)255) ) | k; }

   unsigned int getKey() const         { return _key; }
   void         setKey(unsigned int k) {_key = k;}

   ///@}


protected:
   unsigned int _key;

};

} //< end namespace gutz

#endif

