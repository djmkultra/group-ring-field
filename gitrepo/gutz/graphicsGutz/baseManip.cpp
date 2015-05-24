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

//BaseManip.h


#include "baseManip.h"
#include <string>
#include <iostream>
#include <sstream>
//#include <serialize/SerializeUtil.h>
#include "../eventGutz/mouseEvent.h"

using namespace gutz;
using namespace std;



////////////////////////////////////////////////////////////
/// Serialization.
/// write.


const string DATA_STR("Data{");

const string END_SECTION_STR("\n}\n");



//=========================================================================
//  Uber Serialize
//=========================================================================
bool BaseManip::serialize(std::ostream &os, const std::string indent)
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

//=========================================================================
//  Uber UnSerialize
//=========================================================================
/// read.
bool BaseManip::unserialize(std::istream &is)
{
///TODO: Fix this
#if 0
   string tstr;

   bool status = false;

   /// unserial data
   is >> tstr;
   if(serial::unserialKeyCheck(DATA_STR, tstr, "BaseManip, data")) return true;   
   if(unserialData(is)) return true;
   is >> tstr;
   if(serial::unserialKeyCheck(END_SECTION_STR,tstr,"BaseManip, data")) return true;

   status = status || is.eof() || is.fail();
   return status;
#endif
   return true;
}


//=========================================================================
//  Serialize Data
//=========================================================================
const string FRUSTUM_STR("Frustum: ");
const string CLIPS_STR("Clips: ");

bool BaseManip::serialData(std::ostream &os, const std::string indent)
{
   /// frustum 
   //os << indent << FRUSTUM_STR << _frustum << "\n";
   /// clips
   //os << indent << CLIPS_STR << _clips << "\n";
   return false;
}

//=========================================================================
//  Unserialize Data
//=========================================================================
bool BaseManip::unserialData(std::istream &is)
{
   /// frustum
   //string tstr;
   //is >> tstr;
   //if(unserialKeyCheck(FRUSTUM_STR, tstr, "BaseManip, data frustum")) return true;
   //is >> _frustum;
   /// clips
   //is >> tstr;
   //if(unserialKeyCheck(CLIPS_STR, tstr, "BaseManip, data clips")) return true;
   //is >> _clips;

   return is.eof() || is.fail();
}

