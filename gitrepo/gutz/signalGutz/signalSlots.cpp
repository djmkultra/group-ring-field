/*
* $Id: signalSlots.cpp,v 1.3 2006/11/08 21:34:09 jmk Exp $
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

//signalSlots.cpp


#include "g_signal.h"
#include "signalCall.h"
#include <algorithm>

using namespace std;
using namespace gutz;

SignalTracker::~SignalTracker()
{
   for(int i = 0; i < int(_sigs.size()); ++i)
      if(_sigs[i]) _sigs[i]->detatchSlotIF(callee);
}

void SignalTracker::detatchSignalIF(SignalIF *sig)
{
   SignalPVecIter i = _sigs.begin();
   while( (i = find(_sigs.begin(),_sigs.end(),sig)) != _sigs.end())
   {
     (*i) = 0;
      _sigs.erase(i);
   }
}

