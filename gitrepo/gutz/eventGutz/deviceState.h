///////////////////////////////////////////////////////////////////////////
//              _____________  ______________________    ^    ----  _
//             /  ________  |  |   ___   ________   /   / \  /    \ |
//            |  |       |  |_ |  |_ |  |       /  /   /   \|       |
//            |  |  ___  |  || |  || |  |      /  /   / --- \   --- |
//            |  | |   \ |  || |  || |  |     /  /   /       \____/ |_____|
//            |  | |_@  ||  || |  || |  |    /  /          
//            |  |___/  ||  ||_|  || |  |   /  /_____________________
//             \_______/  \______/ | |__|  /___________________________
//                        |  |__|  |
//                         \______/
//                 University of New Mexico       
//                           2010
///////////////////////////////////////////////////////////////////////////

//mouseEvent.h

#ifndef __GUTZ_DEVICE_STATE_DOT_H
#define __GUTZ_DEVICE_STATE_DOT_H

#include <mathGutz.h>
#include <smartptr.h>
#include <vector>
#if defined(WIN32) || defined(__linux__)
    #include <algorithm>
#endif

namespace gutz {

/// A base class for a whole host of devices...  
///    For example see WiiMoteState below for a complete device classs
///    This class is only an abstract base class, assumes devices have at least 
///    a button to play with...
	
class DeviceState : public gutz::Counted 
{
public:
	/// this is how you duplicate this guy
	virtual DeviceState *clone() = 0;
	
	enum ButtonState {
		STATE_UP      = 0,
		STATE_DOWN    = 1,
		STATE_UNKNOWN = 2,
		STATE_LAST
	};

	/// generic button, really just a fancy struct
	class Button {
	public:  
		Button( const char* bname, 
			   ButtonState bs = STATE_UP, 
			   const char* description = " " )
		: name(bname), state(bs), descriptor(description) 
		{}
		
		///@{
		/// public data members, use freely
		std::string  name;
		unsigned int state;
		std::string  descriptor; 
		long         timeStamp;   ///< Time stamp should be in microseconds. 1/1,000,000 of a second (one millionth)
		///@}
		
		///@{
		/// comparison operators so you can find buttons by name
		bool operator==(const Button &b) const
		{   return name == b.name; }
		bool operator!=(const Button &b) const
		{   return name != b.name; }
		bool operator==(const std::string &s) const
		{   return name == s; }
		bool operator!=(const std::string &s) const
		{   return name != s; }
		///@}
	};
	typedef std::vector<Button> ButtonVec;
	typedef ButtonVec::iterator ButtonVecIter;
	typedef ButtonVec::const_iterator ButtonVecCIter;
	
	virtual unsigned int getNumButtons() const
	{ return _buttonVec.size(); }
	
	virtual unsigned int getButtonState( unsigned int buttonNumber ) const
	{ 
		if( buttonNumber >= _buttonVec.size() ) return STATE_UNKNOWN;
		return _buttonVec[buttonNumber].state;
	}
	virtual Button getButton( unsigned int buttonNumber ) const
	{
		assert( buttonNumber < _buttonVec.size() );
		return _buttonVec[ buttonNumber ]; 
	}
	
	///@{
	/// find button by name, returns a ButtonNotDefined if not found 
	///   see definition of ButtonNotDefined below, also you can see
	///   that the name of the button is "not defined" two ways to tell
	virtual Button getButton( const std::string &name ) const
	{
		ButtonVecCIter i;
		// find() provided by #include <algorithm> on windows
		// and linux. OS X doesn't need the include, which
		// seems strange. -jbowles
		i = find( _buttonVec.begin(), _buttonVec.end(), name );
		if( i != _buttonVec.end() ) return *i;
		else return Button("not defined", STATE_UNKNOWN, "not defined");
	}
	virtual Button getButton( const char *name ) const
	{ return getButton( std::string( name ) ); }
	///@}
	/// add a button, helper function
	virtual void addButton(const char* name, ButtonState bs=STATE_UP, const char* desc= " ")
	{
		_buttonVec.push_back( Button(name,bs,desc) );
	}
	
	/// set button state, duhhh
	void setButton( unsigned int buttonID, ButtonState bs )
	{
		assert( buttonID < _buttonVec.size() );
		_buttonVec[buttonID].state = bs;
	}	

	void setButton( unsigned int buttonID, int bs )
	{
		assert( buttonID < _buttonVec.size() );
		_buttonVec[buttonID].state = static_cast<ButtonState>(bs);
	}	
	
   unsigned int getButtonID(const std::string& name)
   {
      for(unsigned int i = 0; i < _buttonVec.size(); ++i)
		{
			if(_buttonVec[i].name.compare(name) == 0)
			{
				return i;
			}
			
		}
		return -1;
   }
   
   void incrementButton( unsigned int buttonID, int bs )
   {
		assert( buttonID < _buttonVec.size() );
		++_buttonVec[buttonID].state;
   }
	
	/// helper will print out buttons name = state  desc
	std::ostream &printButtons(std::ostream &os) const
	{
		for(unsigned int i=0; i<_buttonVec.size(); ++i)
		{
			os << _buttonVec[i].name << " = " << _buttonVec[i].state << "   "
			   << _buttonVec[i].descriptor << std::endl;
		}
		return os;
	}
	
	/// Name of this Device, should be unique to the device, 
	///   handy for finding stuff by name.
	const std::string &getName() const {return _name;}
	
   virtual void reset(void)
   {
      ButtonVec::iterator button;
      for(button = _buttonVec.begin(); button != _buttonVec.end(); ++button)
      {
         (*button).state = STATE_UP;
      }
   }
   
   virtual bool isNotZero(void) const
   {
      ButtonVec::const_iterator button;
      for(button = _buttonVec.begin(); button != _buttonVec.end(); ++button)
      {
         if((*button).state)
         {
//            std::cout << (*button).state << " " ;
            return true;
         }
         else {
//            std::cout << (*button).state << " ";
         }

      }
      return false;
   }
   
protected:
	///@{
	///  Abstract base class, cannot create these, use derived!
	DeviceState() {}
	DeviceState(const char* name) : _name(name) {}
	DeviceState(const DeviceState &ds) : _name(ds._name), _buttonVec(ds._buttonVec) {}
	DeviceState &operator=(const DeviceState& ds) 
   {
      _name = ds._name;
      _buttonVec = ds._buttonVec;
      return *this;
   }
	///@}
	
	std::string  _name;
	
	/// _buttonVec, a vector of buttons, put your buttons here!
	ButtonVec           _buttonVec;
};
typedef gutz::SmartPtr< DeviceState > DeviceStateSP;
const  DeviceState::Button ButtonNotDefined = DeviceState::Button("not defined", DeviceState::STATE_UNKNOWN, "not defined");
	
	
	
  
   
	
	
	
	
///////////////////////////////////////////////////////////////////////////////
/// NO_MEASURE_VALUE is the "unknown" value for any physics vec4 
const vec4f NO_MEASURE_VALUE = vec4f(-88888888.8888f, -8888.8888f,-8888.8888f,-8888.8888f);

///////////////////////////////////////////////////////////////////////////////
/// A device with physics sensors, like accelerometers, gyros, cameras etc....
///////////////////////////////////////////////////////////////////////////////
	
class PhysicsDeviceState : public DeviceState
{
public:
   typedef DeviceState BaseType;
   
	virtual PhysicsDeviceState *clone() { return new PhysicsDeviceState(*this); }
		
	enum PhysicsDeviceKinds 
   {
	   ACCELEROMETERS,
		GYROSCOPES,
		IR_POINTS,
		DEVICE_KINDS_LAST
	};
	
	class PhysicsMeasure
   {
	public:
		PhysicsMeasure(const char* pname, 
					   const vec4f &pmeasure = NO_MEASURE_VALUE, 
					   const char* desc = " ")
		: name(pname), measure(pmeasure), descriptor(desc)
		{}
      
      
		std::string name;
		gutz::vec4f measure;
		std::string descriptor;
		long        timeStamp;  ///< Time stamp should be in microseconds. 1/1,000,000 of a second (one millionth)
		
		
		///@{
		/// comparison operators so you can find buttons by name
		bool operator==(const PhysicsMeasure &m) const 	{   return name == m.name; }
		bool operator!=(const PhysicsMeasure &m) const	{   return name != m.name; }
		bool operator==(const std::string &s) const		{   return name == s; }
		bool operator!=(const std::string &s) const		{   return name != s; }
		///@}
	};
	typedef std::vector< PhysicsMeasure >     PhysicsMeasureVec;
	typedef PhysicsMeasureVec::iterator       PhysicsMeasureVecIter;
	typedef PhysicsMeasureVec::const_iterator PhysicsMeasureVecCIter;
	
	unsigned int getNumMeasures() const { return _measureVec.size(); }
	
	const vec4f& getMeasureValue(unsigned int measureNum) const 
	{  
		if( measureNum >= _measureVec.size() )
      {
         return NO_MEASURE_VALUE;
      }
		else
      {
         return _measureVec[measureNum].measure;
      }
	}
   
   const std::string getMeasureName(unsigned int measureNum) const 
	{  
		if( measureNum >= _measureVec.size() )
      {
         return std::string();
      }
		else
      {
         return _measureVec[measureNum].name;
      }
	}
	
	///@{
	/// find measures by name, returns MeasureNotDefined (see definition below)
	///   also, the name is "not defined" if not found, two ways to tell
	PhysicsMeasure getMeasure( const std::string &name) const
	{
		PhysicsMeasureVecCIter i;
		i = find( _measureVec.begin(), _measureVec.end(), name );
		if( i != _measureVec.end() )
      {
         return *i;
      }
		else
      {
         return PhysicsMeasure("not defined", NO_MEASURE_VALUE, "not defined");
      }
	}
   
	PhysicsMeasure getMeasure( const char *name ) const {  return getMeasure( std::string(name) );    }
	///@}
	
	/// add measure, helper function
	void addMeasure(const char *name, 
					const vec4f &m = NO_MEASURE_VALUE, 
					const char* desc = " ")
	{
		_measureVec.push_back( PhysicsMeasure(name,m,desc) );
	}
	
   // Redundant - getMeasure by name already exists
   
	/// Get measure ID number based on name.
	int getMeasureID(const std::string &name) const
	{
		for(unsigned int i = 0; i < _measureVec.size(); i++)
		{
			if (_measureVec[i].name.compare(name) == 0)
			{
				return i;
			}
			
		}
		return -1;
	}

	int getMeasureID( const char *name ) const
	{   
		return getMeasureID( std::string(name) );    
	}
	
	
	void setMeasure(unsigned int measureID, const vec4f &m)
	{
		assert( measureID < _measureVec.size() );
		_measureVec[measureID].measure = m;
	}

   void incrementMeasure(unsigned int measureID, const vec4f &m)
	{
		assert( measureID < _measureVec.size() );
		_measureVec[measureID].measure += m;
	}
   
   
	/// hopefully, the physics measures will allow us to estimate the
	///  devices orientation... be sure to check against NO_MEASURE_VALUE
	///  might not ever be meaninfull for this device
	const vec4f& getOrientation() const {return _orientation;}
	
	/// helper will print the measures, name = measure   desc
	std::ostream& printMeasures(std::ostream &os) const
	{
		for(unsigned int i=0; i<_measureVec.size(); ++i)
		{
			os << _measureVec[i].name << " = " << _measureVec[i].measure << "   "
			<< _measureVec[i].descriptor << std::endl;
		}
		return os;
	}
	
   /// Reset the physics state
   virtual void reset(void)
   {
      PhysicsMeasureVec::iterator measure;
      for(measure = _measureVec.begin(); measure != _measureVec.end(); ++measure)
      {
         (*measure).measure = vec4f_zero;
      }
      
      BaseType::reset();
   }
   
   virtual bool isNotZero(void) const
   {
      PhysicsMeasureVec::const_iterator measure;
      for(measure = _measureVec.begin(); measure != _measureVec.end(); ++measure)
      {
		  
         if((*measure).measure != gutz::vec4f_zero)
         {
            // std::cout << "hi! " << (*measure).measure << std::endl; 
            return true;
         }
      }
      return BaseType::isNotZero();
   }

protected:
	/// still abstract base classs, need further derivation to create...
	PhysicsDeviceState()
	: DeviceState(), _orientation(NO_MEASURE_VALUE)
	{}
	PhysicsDeviceState(const char *name)
	: DeviceState(name), _orientation(NO_MEASURE_VALUE)
	{}
	/// using default copy constructor and assignment operator	
	PhysicsDeviceState(const PhysicsDeviceState& ps )
	: DeviceState(ps), _measureVec(ps._measureVec), _orientation(ps._orientation)
	{}
   
   
	/// a vector of physics measures
	PhysicsMeasureVec  _measureVec;
	
	vec4f _orientation;
};
typedef gutz::SmartPtr<PhysicsDeviceState> PhysicsDeviceStateSP;
const PhysicsDeviceState::PhysicsMeasure MeasureNotDefined 
	= PhysicsDeviceState::PhysicsMeasure("not defined", NO_MEASURE_VALUE, "not defined");	
	
class GenericMrmrState : public PhysicsDeviceState
{
public:
	GenericMrmrState(const char *name) : PhysicsDeviceState(name) {}
};

typedef gutz::SmartPtr< GenericMrmrState > GenericMrmrStateSP;	
	
	
	
	
	
	
	
	
	
///////////////////////////////////////////////////////////////////////////////
/// A WiiMote, has accelerometers, gyros, cameras etc....
///////////////////////////////////////////////////////////////////////////////	
class WiiMoteState : public PhysicsDeviceState 
{
public:
   typedef PhysicsDeviceState BaseType;
   
	virtual WiiMoteState *clone() { return new WiiMoteState(*this); }
	
	/// be sure to name your devices... good idea when using osc
	WiiMoteState(const char* name=0) 
	: PhysicsDeviceState(name)
	{
		/// Note, these buttons must correspond to 
		/// enum entries exaclty, or we will have problems...
		///   Names should currently correspond to Osculator address names
		
		/// the order (number/index) of these names MUST EXACTLY match the enums below
		addButton("1");
		addButton("2");
		addButton("Minus");
		addButton("Plus");
		addButton("Home");
		addButton("A");
		addButton("Left");
		addButton("Right");
		addButton("Up");
		addButton("Down");
		addButton("B");
		addButton("C");
		addButton("Z");
		
		/// the order (number/index) of these names MUST EXACTLY match the enums below
		addMeasure("accel-xyz");
		addMeasure("accel-pry");
		addMeasure("gyro-velo");
		addMeasure("gyro-angles");
		addMeasure("gyro-quats", gutz::quatf());
		addMeasure("IR1");
		addMeasure("IR2");
		addMeasure("IR3");
		addMeasure("IR4");
		addMeasure("chuck-accel");
		addMeasure("chuck-joystick");
		addMeasure("chuck-accel-xyz");
		addMeasure("balance-indvidual");
		addMeasure("balance-calculated");
	}
   
   WiiMoteState(const WiiMoteState& ws)
   : BaseType(ws)
   {
   }
	
	enum WiiButton {
		ONE,
		TWO,
		MINUS,
		PLUS,
		HOME,
		A,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		B,
		C,
		Z,
		WII_BUTTON_LAST
	};
	
	enum WiiMeasure {
		ACCEL_XYZ,
		ACCEL_PRY,
		GYRO_VELO,
		GYRO_ANGLES,
		GYRO_QUATS,
		IR1,
		IR2,
		IR3,
		IR4,
		CHUCK_ACCEL,
		CHUCK_JOYSTIC,
		CHUCK_ACCEL_XYZ,
		BALANCE_INDIVIDUAL,
		BALANCE_CALCULATED,
		WII_MEASURE_LAST
	};
	
	void setButtonState(unsigned int buttonID, ButtonState bs)
	{  DeviceState::setButton(buttonID,bs); }
	
	void setMeasureValue(unsigned int measureID, const vec4f &m)
	{ PhysicsDeviceState::setMeasure(measureID,m); }
	
   virtual void reset(void)
   {
      BaseType::reset();
   }
   
   virtual bool isNotZero(void) const
   {
      return PhysicsDeviceState::isNotZero();
   }
   
protected:
	
};
/// WiiMoteState Smart Pointer	
typedef SmartPtr< WiiMoteState > WiiMoteStateSP;


///////////////////////////////////////////////////////////////////////////////
/// A MRMR device, has buttons, heck, anything. Just doing buttons for now
///////////////////////////////////////////////////////////////////////////////	
class MrmrState : public WiiMoteState
{
public:
   typedef WiiMoteState BaseType;
   typedef SmartPtr<MrmrState> ptr;
   typedef std::vector<ptr>       vec;
   typedef std::vector<ptr>::iterator vec_itr;

   //MrmrState::ptr someVarName = new MrmrState(fields);
   // MrMrStateSPVec
   //MrmrState::vec list;
   //list.push_back(new MrMrstatate)
   
   virtual MrmrState *clone() { return new MrmrState(*this); }
     
   /// be sure to name your devices... good idea when using osc
   MrmrState(const char* name=0) 
   : WiiMoteState(name)
   {
      /// Note, these buttons must correspond to 
      /// enum entries exaclty, or we will have problems...
      ///   Names should currently correspond to Osculator address names
         
      /// the order (number/index) of these names MUST EXACTLY match the enums below
      addButton("1");
      addButton("2");
      addButton("3");
      addButton("4");
      addButton("5");
      addButton("6");
      addButton("7");
      addButton("8");
   }
      
   MrmrState(const MrmrState& ws)
   : BaseType(ws)
   {
   }
      
   enum MrmrButton {
      ONE,
      TWO,
      THREE,
      FOUR,
      FIVE,
      SIX,
      SEVEN,
      EIGHT,
   };
      
   void setButtonState(unsigned int buttonID, ButtonState bs)
   {  DeviceState::setButton(buttonID,bs); }
      
   void setMeasureValue(unsigned int measureID, const vec4f &m)
   { PhysicsDeviceState::setMeasure(measureID,m); }
      
   virtual void reset(void)
   {
      BaseType::reset();
   }
      
   virtual bool isNotZero(void) const
   {
      return PhysicsDeviceState::isNotZero();
   }
};

/// MrmrState Smart Pointer	
typedef SmartPtr< MrmrState > MrmrStateSP;
      

// The below operator<< causes these linker errors:

// 4>GLUber.lib(gString.obj) : error LNK2005: "class std::basic_ostream<char,struct std::char_traits<char> > & __cdecl gutz::operator<<(class std::basic_ostream<char,struct std::char_traits<char> > &,class gutz::WiiMoteState const &)" (??6gutz@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AAV12@ABVWiiMoteState@0@@Z) already defined in glwidget.obj
// 4>C:\Users\Andrei\Desktop\DomeBuild\QtFD\Debug\QtFD.exe : fatal error LNK1169: one or more multiply defined symbols found

// (I've checked, and operator<< is not redefined elsewhere with a WiiMoteState argument, so I'm not too sure what the problem is... )

	
//std::ostream&
//operator<<(std::ostream &os, const WiiMoteState &ws)
//{
//	os << "Wii Mote State :" << std::endl;
//	ws.printButtons( os );
//	ws.printMeasures( os );
//	return os;
//		
//}
//	
	
} //< end namespace gutz

#endif
