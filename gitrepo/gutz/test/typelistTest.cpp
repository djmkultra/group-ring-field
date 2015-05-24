#if 0
// If-zeroed-out due to development conflicts for the Physics Automation Toolkit
#include <typelistGutz.h>
#include <staticCheckGutz.h>
#include <loki/HierarchyGenerators.h>
#include <stdlib.h>
#include <iostream>
#include <vec.h>
#include <vector>

using gutz::Typelist;
using gutz::Length;
using gutz::NullType;
using gutz::TypeAt;
using gutz::IndexOf;
using gutz::IsSameType;
using gutz::GenScatterHierarchy;
using std::vector;

#include <loki/Typelist.h>
#include <loki/TypeTraits.h>
#include <loki/EmptyType.h>

// Test lengths of typelists
void testTypelist(void)
{
   typedef NullType ZeroList;
   typedef Typelist<NullType, NullType> OneList0;
   typedef TYPELIST_1(int) OneList1;
   typedef TYPELIST_2(int, unsigned int) TwoList;
   typedef TYPELIST_10(int, unsigned int, short, char, 
                       char*, int*, float, float*, double, double*) TenList;
   
   const int ZeroListLen = Length<ZeroList>::value;
   const int OneList0Len = Length<OneList0>::value;
   const int OneList1Len = Length<OneList1>::value;
   const int TwoListLen  = Length<TwoList>::value;
   const int TenListLen  = Length<TenList>::value;
   
   GUTZ_STATIC_CHECK(ZeroListLen ==  0, ZeroList_should_have_length_of_0);
   GUTZ_STATIC_CHECK(OneList0Len ==  1, OneList0_should_have_length_of_1);
   GUTZ_STATIC_CHECK(OneList1Len ==  1, OneList1_should_have_length_of_1);
   GUTZ_STATIC_CHECK(TwoListLen  ==  2, TwoList_should_have_length_of_2);
   GUTZ_STATIC_CHECK(TwoListLen  !=  3, TwoList_should_not_have_length_of_3);
   GUTZ_STATIC_CHECK(TenListLen  == 10, TenList_should_have_length_of_10);
   GUTZ_STATIC_CHECK(TenListLen  != 12, TenList_should_not_have_length_of_12);
   
   typedef TypeAt<TenList, 0>::result TenList_idx0_type;
   typedef TypeAt<TenList, 4>::result TenList_idx4_type;
   typedef TypeAt<TenList, 9>::result TenList_idx9_type;
   
   GUTZ_STATIC_CHECK((IsSameType<TenList_idx0_type, int>::value     == true),  TenList_at_idx0_should_be_int);
   GUTZ_STATIC_CHECK((IsSameType<TenList_idx9_type, double*>::value == true),  TenList_at_idx9_should_be_ptr_to_double);
   GUTZ_STATIC_CHECK((IsSameType<TenList_idx4_type, float>::value   != true),  TenList_at_idx4_should_not_be_float);
   GUTZ_STATIC_CHECK((IsSameType<TenList_idx4_type, float>::value   == false), TenList_at_idx4_should_not_be_float);
   
   typedef IndexOf<TenList, int> IndexOfInt;
   typedef IndexOf<TenList, double*> IndexOfDoubleptr;
   
   GUTZ_STATIC_CHECK(IndexOfInt::result == 0, TenList_should_have_int_at_index_0);
   GUTZ_STATIC_CHECK(IndexOfDoubleptr::result == 9, TenList_should_have_ptr_to_double_at_index_9);
   GUTZ_STATIC_CHECK(IndexOfDoubleptr::result != 6, TenList_should_not_have_ptr_to_double_at_index_6);
}

enum MeasurementType
{
   MASS,
   POSITION,
   VELOCITY,
   ACCELERATION,
};

enum Unit
{
   KG,   // Kilograms
   G,    // Grams
   M,    // Meters
   M_S,  // Meters per second
   M_S2, // Meters per second squared
};

template<typename T, enum MeasurementType, enum Unit>
class Measurement
{
public:
   // Constructors
   Measurement() {}
   Measurement(const Measurement& v) : _value(v._value) {}
   Measurement(const T& v)           : _value(v)        {}
  
   Measurement& operator=(const T& v)
   {
      _value = v;
      return *this;
   }
#if 0
   T& operator=(const T& v)
   {
      _value = v;
      return &value;
   }
#endif
   operator T&()       { return _value; }
   operator const T&() { return _value; }
   
//private:
   T _value;
};

template<int UNIT, int DIM>
class Measure{ 
public:
	enum { 
		Units = UNIT,
		Dim   = DIM
	};
	
}



#if 1
typedef Measurement<float,       MASS,         KG>   Mass;
typedef Measurement<gutz::vec3f, POSITION,     M>    Position;
typedef Measurement<gutz::vec3f, VELOCITY,     M_S>  Velocity;
typedef Measurement<gutz::vec3f, ACCELERATION, M_S2> Acceleration;
#else

typedef float Mass;
typedef gutz::vec3f Position;
typedef gutz::vec3f Velocity;
typedef gutz::vec3f Acceleration;

#endif


void testGenScatterHierarchy(void)
{  
//   typedef Loki::Seq<Mass, Velocity Acceleration>::Type particle_typelist;

#if 1
//   Mass kg;
//   kg = 10;
   
   // Define the particle typelist
   typedef Loki::Seq<Mass*, Position*>::Type particle_typelist;

   // Define the particle type
   typedef Loki::GenScatterHierarchy<particle_typelist, Loki::TupleUnit> particle_type;

   // Instantiate a list of particles

   int numParticles = 4096;
   particle_type particle_list[numParticles];
   vector<Mass>     massList;
   vector<Position> positionList;
   vector<Velocity> velocityList;
   
   massList.resize(numParticles);
   positionList.resize(numParticles);
   velocityList.resize(numParticles);
   
   int i;
   for(i = 0; i < numParticles; ++i)
   {
      Loki::Field<Mass*>(particle_list[i]).value_     = &(massList[i]);
      Loki::Field<Position*>(particle_list[i]).value_ = &(positionList[i]);
//      (*Loki::Field<0>(particle_list[i])).value_         = &(massList[i]);
      //   Loki::Field<Velocity*>(particle_list).value_ = &(velocityList[i]);
   }
   
   int idx = 20;


   massList[idx]     = 10;
   positionList[idx] = gutz::vec3f(1,2,3);
   velocityList[idx] = gutz::vec3f(4,5,6);

   Mass m1 = *Loki::Field<Mass*>(particle_list[idx]).value_;
   Mass m2 = 20;

   Mass m3 = m1._value + m2._value;
   std::cout << "mass: " << m1._value << std::endl;
   // Get the size of a Tuple if it isn't part of a tuple
   const size_t sizeofInt      = sizeof(int) * 4096;
   const size_t sizeofFloat    = sizeof(float) * 4096;
   const size_t sizeofDouble   = sizeof(double) * 4096;
   const size_t sizeofAll      = sizeofInt + sizeofFloat + sizeofDouble;

   // Get the size of the Tuple
   const size_t sizeofParticleType = sizeof(particle_type);
   
   // Compare the size of the three values with and without the tuple wrapper
   std::cout << "sizeof separate: "      << sizeofAll          << std::endl;
   std::cout << "sizeof particle_type: " << sizeofParticleType << std::endl;
   
#if 0
//   int val;
//   val = Loki::Field<int>(someInfo)();
   std::cout << "int field: " << (Loki::Field<Mass*>(particle_list[i]).value_)() << std::endl;
//   std::cout << "int field: " << (*Loki::Field<Velocity*>(particle_list[idx]).value_) << std::endl;
#endif
#endif
}

//template <class T, int NAME>

//This is a trick ot tell if something is a pointer or not, 
// this one is for non-pointers...
template <class T>
class IsPointer {
public:
	enum{ Result=0 };
};
//This specializes the above, catching the case where we have a pointer!
template<class T>
class IsPointer<T*> {
public:
	enum{ Result=1 };
};

#ifdef COMPILE_THIS_DOODLE
namespace awesome
{
   
   ////////////////////////////////////////////////////////////////////////////////
   /// Particle attributes
   ////////////////////////////////////////////////////////////////////////////////
   struct Particle
   {
      enum ParamNames
      {
         Velocity,
         Position,
         Mass,
         Charge,
         ParamNameLast
      };
      
      typedef Loki::Seq<gutz::vec3f*, gutz::vec3f*, float*, float*>::Type particle_type;
      
      typedef Loki::TL::TypeAt<particle_type, Velocity> velocity_type;
      typedef Loki::TL::TypeAt<particle_type, Position> position_type;
      typedef Loki::TL::TypeAt<particle_type, Mass>     mass_type;
      typedef Loki::TL::TypeAt<particle_type, Charge>   charge_type;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /// Integrator
   ////////////////////////////////////////////////////////////////////////////////
   template<class SIMULATION_ATTRIBUTES>
   class Integrator
   {
      // Depends on field, doesn't care about particle_type
   };
   
   ////////////////////////////////////////////////////////////////////////////////
   /// Field
   ////////////////////////////////////////////////////////////////////////////////
   template<class SIMULATION_ATTRIBUTES>
   class Field
   {
      // Depends on particle_type
      typedef SIMULATION_ATTRIBUTES Attributes;
      typedef typename Attributes::ParticleType ParticleType;
   public:
      
      
   };
   
   ////////////////////////////////////////////////////////////////////////////////
   /// Simulation Attributes
   ////////////////////////////////////////////////////////////////////////////////
   template<class ParticleAttributes>
   struct SimulationAttributes
   {
      // Depends on integrator
      
      typedef gutz::field::Gravity<ParticleAttributes>  field_type;
      typedef gutz::RK4<field_type>                     integrator_type;
      
   };
   
   ////////////////////////////////////////////////////////////////////////////////
   /// Simulation 
   ////////////////////////////////////////////////////////////////////////////////
   template<class SIMULATION_ATTRIBUTES>
   class Simulation
   {
      typedef SIMULATION_ATTRIBUTES sim_attribs_type;
   public:
   private:
      
      
      
      
   };
   
   // Contains a list of types that define the simulation
   class SimulationAttributes
   {

   };
      
   typedef Loki::Seq ParticleAttributes;
   class Position {};
   class Velocity {};
   class Acceleration {};
   
   template<class SimulationAttribs>
   class DomeSim : public DomeApp
   {
   public:
      
      void update(void)
      {
         float dt = 0.001;
         _sim.advance(dt);
      }
      
   private:
      Simulation<SimulationAttribs> _sim;
   };

   // Main
   void createSim(void)
   {
      typedef Particle<Mass, Position, Velocity, Acceleration> ParticleType;
      typedef gutz::Field::Add(gutz::field::Currly, gutz::field::NBodyGravity) FieldType;
      
      typedef SimulationAttribs<MyParticleAttribs, gutz::RK4, myField> SimAttribs;
      
      DomeSim<MySimAttribs> app;
      
      app.exec();
   }

   //
   template<class ParticleType>
   class Spring //< N Body Field 
   {
   public:
      vec3f operator()(const ParticleAttributes& p0, const ParticleAttributes& p1)
      {
         vec3f displacement = Loki::Field<ParticleAttributes::Position>(p0) - Loki::Field<ParticleAttributes::Position>(p1);
         float distance = displacement.normalize();
         return gutz::vec3f(1,1,1);
      }

   private:
      float _kappa;
      float _length[3303][3303];
   };
   
   void createDevonSim(void)
   {
      typedef ParticleAttributes<Position, Velocity, Acceleration> MyParticleAttribs;
      typedef gutz::Field::Add(gutz::field::Currly, Spring<MyParticleAttribs>) MyField;
      
      typedef SimulationAttribs<MyParticleAttribs, gutz::RK4, MyField> MySimAttribs;
      
      float* data = readData(filename);

      
      template<class ParticleType>
      struct InitParticle
      {
         operator()(ParticleType& p0, int i)
         {
            Loki::Field<ParticleType::Velocity>(p0) = vec3f(1,1,1);
         }
      };
      
      DomeSim<MySimAttribs> app;

      
      app.foreachParticle(InitParticles());

      app.exec();
   }
   
      
}
#endif

template <class T>
T &getItRaw( T &var ) {return var;} 
// Pointer overload, returns refernce to what is pointed to
template <class T>
T &getItRaw( T *var ) {return *var;}

void testCrazyMulitpleTypeHierarchy()
{
	enum ParamNames {
	    Velocity,
	    Position,
		Mass,
		Charge,
		ParamNameLast
	};
	
	const int sz = 100;
	
	typedef Loki::Seq<gutz::vec3f*, gutz::vec3f*, float*, float*>::Type Partys;
	typedef Loki::GenScatterHierarchy<Partys, Loki::TupleUnit> particle_type;
	
	particle_type particle;
	
	Loki::TL::TypeAt<Partys, 0>::Result hi;
	
	Loki::TL::TypeAt<particle_type::TList, Velocity>::Result VelocityArray[sz];
	
	Loki::Field<Velocity>(particle) = new gutz::vec3f(1,1,1);
	Loki::Field<Position>(particle) = new gutz::vec3f(69,69,69);
	
	getItRaw(Loki::Field<Velocity>(particle)) = gutz::vec3f(1,2,3); // SHIT, doesn't work
	
	//IsPointer<>::getRaw(Loki::Field<Velocity>(particle)) = gutz::vec3f(1,2,3);
	
	
	std::cout << "I is a vec rt? " << (*Loki::Field<Velocity>(particle)) << std::endl;
	std::cout << "I is another vec rt? " << (*Loki::Field<Position>(particle)) << std::endl;
	
	std::cout << typeid( hi ).name() << std::endl;
	std::cout << "Am I a pointer? " << IsPointer<Loki::TL::TypeAt<particle_type::TList, Velocity>::Result>::Result << std::endl;
	
	std::cout << "I is not pointer? " << IsPointer<int>::Result << std::endl;
	
	std::cout << typeid( hi ).name() << std::endl;
	
	//trying jeff's idea:
	
	typedef Measurement<float,       MASS,         KG>   MassType;
	typedef Measurement<gutz::vec3f, POSITION,     M>    PositionType;
	typedef Measurement<gutz::vec3f, VELOCITY,     M_S>  VelocityType;
	typedef Measurement<gutz::vec3f, ACCELERATION, M_S2> AccelerationType;
	
	typedef Loki::Seq<MassType, PositionType, VelocityType, AccelerationType>::Type ParticleTypeSeq;
	typedef Loki::GenScatterHierarchy<ParticleTypeSeq, Loki::TupleUnit> ParticleType;
	
	ParticleType jeffParty;
	
	/// This .value_ sucks, but I think if we dissallow duplicates, it is unncessary... TODO
	Loki::Field<MassType>(jeffParty).value_ = 10;
	Loki::Field<PositionType>(jeffParty).value_ = gutz::vec3f(1,2,3);
	Loki::Field<VelocityType>(jeffParty).value_ = gutz::vec3f(4,5,6);
		
}

// Concept: one physical property


#endif // if 0
int main(void)
{
#if 0
   
   testGenScatterHierarchy();
	testCrazyMulitpleTypeHierarchy();
#endif
	
   return 0;
}
