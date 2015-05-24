/*
 * $Id: arrayWrap1.h,v 1.3 2006/04/27 19:58:10 jmk Exp $
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
///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// 9/8/02	Aaron Lefohn	Scientific Computing and Imaging Institute
//			Joe M. Kniss	School of Computing
//							University of Utah
//
//  Array1 - An Array WRAPPER class.
//  	   - This is a lightweight class that does NOT own its data.
//  	   - Copies of "arrayWrap1" objects are shallow.
//  	   - The constructor does NOT allocate any memory.
//		   - The destructor does NOT free the memory.
//		   - The (i) and [i] operators are identical for the 1D case.
//		     Both exist here for compatibility with higher-dimensional arrays.
////////////////////////////////////////////////////////////////////////

#ifndef ARRAY_WRAP_1_h
#define ARRAY_WRAP_1_h

#include <arrayBase.h>
#include <assert.h>
#include <mm.h>

#ifdef USING_MATHGUTZ
	#include <mathGutz.h>
#endif

namespace gutz
{

// Foward Decl
template <class T> class arrayWrap2;

//////////////////////////////
// arrayWrap1
//////////////////////////////
template <class T>
class arrayWrap1 : public arrayBase<T>
{
public:

  friend class arrayWrap2<T>; // This is a friend so that it can access the 'setData' function.
  
  // Constructors - Shallow wrap around data
  // WARNING: No size checking is done here so be sure to get allocation correct!!
 arrayWrap1() : arrayBase<T>() {};
 arrayWrap1( int s, T* v) : arrayBase<T>(s,v) {}
 arrayWrap1( const arrayWrap1<T>& a ) : arrayBase<T>(a) {}
  arrayWrap1<T>& operator=( const arrayWrap1<T>& a );
  
  ~arrayWrap1();
  
  // Accessors - The [] and () operators are identical here, but here for consistency with
  // 			   the higher-dimensional arrays.
  inline T&       operator[] (int i)       { assert(i < this->mSize); return this->mData[i]; }
  inline const T& operator[] (int i) const { assert(i < this->mSize); return this->mData[i]; }
  inline T&       operator() (int i)	      { assert(i < this->mSize); return this->mData[i]; }
  inline const T& operator() (int i) const { assert(i < this->mSize); return this->mData[i]; }
    
 protected:
  
 private:
    inline void     setData(T* d) {this->mData = d;}
};

////////////////////////////////////
// Implementation
////////////////////////////////////

// Constructors - All are shallow!!
//template <class T>
//arrayWrap1<T>::arrayWrap1()
//:arrayBase<T>()
//{
//}

//template <class T>
//arrayWrap1<T>::arrayWrap1( int size, T* data )
//:arrayBase<T>(size,data)
//{
//}

// Copy Ctor
//template <class T>
//arrayWrap1<T>::arrayWrap1<T>( const arrayWrap1<T>& a ) : arrayBase<T>(a)
//{}

// Assignment Op
template <class T>
arrayWrap1<T>& arrayWrap1<T>::operator=(const arrayWrap1<T>& a)
{
  if( this != &a ) {
    arrayBase<T>::operator=(a);// Call base class assign. op
  }
  return *this;
}

// Destructor
template <class T>
arrayWrap1<T>::~arrayWrap1()
{}

////////////////////////////////////
// Typedefs
////////////////////////////////////

typedef arrayWrap1<char> 	arrayw1c;
typedef arrayWrap1<uchar> 	arrayw1uc;
typedef arrayWrap1<char> 	arrayw1b;
typedef arrayWrap1<uchar> 	arrayw1ub;
typedef arrayWrap1<short> 	arrayw1s;
typedef arrayWrap1<ushort> 	arrayw1us;
typedef arrayWrap1<int>	 	arrayw1i;
typedef arrayWrap1<uint> 	arrayw1ui;
typedef arrayWrap1<long> 	arrayw1l;
typedef arrayWrap1<ulong> 	arrayw1ul;
typedef arrayWrap1<llong> 	arrayw1ll;
typedef arrayWrap1<ullong> 	arrayw1ull;
typedef arrayWrap1<float> 	arrayw1f;
typedef arrayWrap1<double> 	arrayw1d;

#ifdef USING_MATHGUTZ
	typedef arrayWrap1<vec2ub> arrayw1v2ub;
	typedef arrayWrap1<vec2i>  arrayw1v2i;
	typedef arrayWrap1<vec2ui> arrayw1v2ui;
	typedef arrayWrap1<vec2f>  arrayw1v2f;

	typedef arrayWrap1<vec3ub> arrayw1v3ub;
	typedef arrayWrap1<vec3i>  arrayw1v3i;
	typedef arrayWrap1<vec3ui> arrayw1v3ui;
	typedef arrayWrap1<vec3f>  arrayw1v3f;

	typedef arrayWrap1<vec4ub> arrayw1v4ub;
	typedef arrayWrap1<vec4i>  arrayw1v4i;
	typedef arrayWrap1<vec4ui> arrayw1v4ui;
	typedef arrayWrap1<vec4f>  arrayw1v4f;

	typedef arrayWrap1<mat2ub> arrayw1m2ub;
	typedef arrayWrap1<mat2i>  arrayw1m2i;
	typedef arrayWrap1<mat2ui> arrayw1m2ui;
	typedef arrayWrap1<mat2f>  arrayw1m2f;

	typedef arrayWrap1<mat3ub> arrayw1m3ub;
	typedef arrayWrap1<mat3i>  arrayw1m3i;
	typedef arrayWrap1<mat3ui> arrayw1m3ui;
	typedef arrayWrap1<mat3f>  arrayw1m3f;

	typedef arrayWrap1<mat4ub> arrayw1m4ub;
	typedef arrayWrap1<mat4i>  arrayw1m4i;
	typedef arrayWrap1<mat4ui> arrayw1m4ui;
	typedef arrayWrap1<mat4f>  arrayw1m4f;
#endif

} // End of namespace gutz

#endif // arrayWrap1_h

