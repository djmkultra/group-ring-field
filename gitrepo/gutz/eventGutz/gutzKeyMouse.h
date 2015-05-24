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

//gutzKeyMouse.h

// Universal names for common keyboard and mouse keys

#ifndef __GUTZ_KEY_MOUSE_DOT_H
#define __GUTZ_KEY_MOUSE_DOT_H

#include <map>
#include "../include/smartptr.h"

namespace gutz {


////////////////////////////////////////////////////////////////////////////
/// Key Map Compare Functor, for 
///    Mapping keys to events using bit vectors
////////////////////////////////////////////////////////////////////////////
   
struct eventKeyMapCmp
{
  bool operator()(unsigned int keyBV1, unsigned int keyBV2) const
  {
	  return (keyBV1 < keyBV2);
  }
};

/// Event map, takes button ids (unsigned ints) and returns event behaviors
///  (also unsigned ints).  I overloaded this guy so that the const [] op
///   will behave correctly.
class EventKeyMap 
   : public std::map<unsigned int, unsigned int,   eventKeyMapCmp>,
     public gutz::Counted
{
public:
   typedef std::map<unsigned int, unsigned int, eventKeyMapCmp> Base;
   typedef Base::iterator Iter;
   typedef Base::const_iterator CIter;

   EventKeyMap() {}
   ~EventKeyMap() {}

   unsigned int operator[](unsigned int key) const 
   {
      CIter ei = find(key);
      if( ei == end() ) return 0;
      return (*ei).second;
   }

   unsigned int &operator[](unsigned int key)
   { 
      return Base::operator[](key);
   }


};
typedef gutz::SmartPtr<EventKeyMap> EventKeyMapSP;

/// Event Param map, takes button ids (unsigned ints) and returns event paramters
///  (floats).  I overloaded this guy so that the const [] op
///   will behave correctly.
class EventParamMap
   : public std::map<unsigned int, float, eventKeyMapCmp>,
     public gutz::Counted
{
public:
   typedef std::map<unsigned int, float, eventKeyMapCmp> Base;
   typedef Base::iterator Iter;
   typedef Base::const_iterator CIter;

   EventParamMap() {}
   ~EventParamMap() {}

   float operator[](unsigned int key) const 
   {
      CIter ei = find(key);
      if( ei == end() ) return 0;
      return (*ei).second;
   }

   float &operator[](unsigned int key)
   { 
      return Base::operator[](key);
   }
};
typedef gutz::SmartPtr<EventParamMap> EventParamMapSP;

//class EventMap
//   : public std::map<unsigned int, 

//typedef std::map<int, int,   eventKeyMapCmp> eventKeyMap;
//typedef std::map<int, float, eventKeyMapCmp> eventParamMap;

////////////////////////////////////////////////////////////
/// GutzButtonState,
///  Mouse and Arrow Key State identifiers
////////////////////////////////////////////////////////////
typedef enum GUTZ_BUTTON_STATES {
   GUTZ_KEY_DOWN,
   GUTZ_KEY_UP   
} GutzButtonState;


////////////////////////////////////////////////////////////
/// GutzButton,
///  Mouse and Arrow Key Identifiers and Modifiers
////////////////////////////////////////////////////////////
typedef enum GUTZ_MOUSE_BUTTONS { //Buttons
   GUTZ_BUTTON_NONE      = 0,
   GUTZ_MOUSE_UP         = 1 << 0,
   GUTZ_LEFT_MOUSE       = 1 << 1,      ///<Left Mouse      button
   GUTZ_MIDDLE_MOUSE     = 1 << 2,      ///<Middle Mouse    button
   GUTZ_RIGHT_MOUSE      = 1 << 3,      ///<Right Mouse     button
   GUTZ_UP_ARROW         = 1 << 4,      ///<Up arrow        key 
   GUTZ_DOWN_ARROW       = 1 << 5,      ///<Down arrow      key 
   GUTZ_RIGHT_ARROW      = 1 << 6,      ///<Right arrow     key 
   GUTZ_LEFT_ARROW       = 1 << 7,      ///<Left arrow      key
   GUTZ_SHIFT            = 1 << 8,      ///<Shift key       modifier
   GUTZ_CTRL             = 1 << 9,      ///<Control key     modifier
   GUTZ_ALT              = 1 << 10,      ///<ALT key         modifier
   GUTZ_DBL_LEFT_MOUSE   = 1 << 11,     ///< double click
   GUTZ_DBL_RIGHT_MOUSE  = 1 << 12,     ///< double click
   GUTZ_DBL_MIDDLE_MOUSE = 1 << 13,     ///< double click
   GUTZ_LAST_BUTTON      = 1 << 14,     ///<LAST button & Total number of unique combinations
   GUTZ_UNUSED_01        = 1 << 15,     ///<Use for custom events
   GUTZ_UNUSED_02        = 1 << 16,     ///<Use for custom events
   GUTZ_UNUSED_03        = 1 << 17,     ///<Use for custom events
   GUTZ_UNUSED_04        = 1 << 18,     ///<Use for custom events
   GUTZ_UNUSED_05        = 1 << 19,     ///<Use for custom events
   GUTZ_UNUSED_06        = 1 << 20,     ///<Use for custom events
   GUTZ_UNUSED_07        = 1 << 21      ///<Use for custom events
} GutzButton;

////////////////////////////////////////////////////////////
/// Keyboard keys, use a GUTZ_MOUSE_BUTTONS for modifiers
////////////////////////////////////////////////////////////
typedef enum GUTZ_KEYS {
   GUTZ_ASCI   = 255, ///< unsigned char keys (ASCI keys)
   GUTZ_F1,  ///< Special (F) keys
   GUTZ_F2,
   GUTZ_F3,
   GUTZ_F4,
   GUTZ_F5,
   GUTZ_F6,
   GUTZ_F7,
   GUTZ_F8,
   GUTZ_F9,
   GUTZ_F10,
   GUTZ_F11,
   GUTZ_F12,
   GUTZ_TAB,  ///< Tab
   GUTZ_NLK,  ///< Num Lock
   GUTZ_CLK,  ///< Caps Lock
   GUTZ_ESC,  ///< Escape
   GUTZ_INS,  ///< Insert
   GUTZ_DEL,  ///< Delete
   GUTZ_HOM,  ///< Home
   GUTZ_END,  ///< End
   GUTZ_PUP,  ///< Page up
   GUTZ_PDN,  ///< Page down
   GUTZ_AUP,  ///< Arrow up
   GUTZ_ADN,  ///< Arrow down
   GUTZ_ALF,  ///< Arrow left
   GUTZ_ART,  ///< Arrow right
   GUTZ_PSCR, ///< Print screen
   GUTZ_SCRL, ///< Scroll lock
   GUTZ_PSEB, ///< Pause/break
   GUTZ_LAST_KEY
} GutzKeys;

//////////////////////////////////////////////////////////////
/// add a key to a bit vector
//////////////////////////////////////////////////////////////
inline
unsigned int gutzAddKey(unsigned int &keyBitVec, unsigned int keyID)
{
   keyBitVec = ((unsigned int)keyBitVec | (unsigned int)keyID);
   return keyBitVec;
}

//////////////////////////////////////////////////////////////
/// remove a key from a bit vector
//////////////////////////////////////////////////////////////
inline
unsigned int gutzDelKey(unsigned int &keyBitVec, unsigned int keyID)
{
   keyBitVec = ((unsigned int)keyBitVec - (unsigned int)keyID);
   return keyBitVec;
}

//////////////////////////////////////////////////////////////
/// Test for specific keys
//////////////////////////////////////////////////////////////

inline
bool isMouse(unsigned int keyBitVec)
{
   if(keyBitVec & GUTZ_LEFT_MOUSE) return true;
   if(keyBitVec & GUTZ_MIDDLE_MOUSE) return true;
   if(keyBitVec & GUTZ_RIGHT_MOUSE) return true;
   return false;
}

inline
bool isArrow(unsigned int keyBitVec)
{
   if(keyBitVec & GUTZ_UP_ARROW) return true;
   if(keyBitVec & GUTZ_DOWN_ARROW) return true;
   if(keyBitVec & GUTZ_LEFT_ARROW) return true;
   if(keyBitVec & GUTZ_RIGHT_ARROW) return true;
   return false;
}

inline
bool isModified(unsigned int keyBitVec)
{
   if(keyBitVec & GUTZ_SHIFT) return true;
   if(keyBitVec & GUTZ_ALT) return true;
   if(keyBitVec & GUTZ_CTRL) return true;
   return false;
}

inline
bool isCustom(unsigned int keyBitVec)
{
   if(keyBitVec > GUTZ_LAST_KEY) return true;
   return false;
}

} //<end namespace gutz

#endif

