/*
* $Id: signalIF.h,v 1.2 2006/11/08 21:34:09 jmk Exp $
*/
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
//
// By Joe Kniss, with help from Yarden Livnat
///////////////////////////////////////////////////////////////////////////

//signalIF.h

#ifndef __GUTZ_SIGNAL_INTERFACE_DOT_H
#define __GUTZ_SIGNAL_INTERFACE_DOT_H

namespace gutz {

///////////////////////////////////////////////////////////////////////////
/// Signal interface, see gutz::Signal, NOT FOR GENERAL USE. (below). 
///   Users can ignore this class
///////////////////////////////////////////////////////////////////////////
class SignalIF {
public:
   SignalIF() {}
   virtual ~SignalIF() {} 

   /// detatch a slot, all references to it!!!
   /// The Signal<> class implements this
   /// and deletes all calls to pointers that match. 
   /// Called by the "SignalTracker" object when
   /// someone declaring "HAS_SLOTS" destructs. 
   virtual void detatchSlotIF(void const *callee) = 0;
protected:
};

} // end namespace gutz

#endif

