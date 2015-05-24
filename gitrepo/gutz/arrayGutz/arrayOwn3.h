/*
 * $Id: arrayOwn3.h,v 1.3 2006/04/27 19:58:10 jmk Exp $
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
// 8/26/02	Aaron Lefohn	Scientific Computing and Imaging Institute
//							School of Computing
//							University of Utah
// 
// arrayOwn3 - A lightweight 3D array class that OWNS its data.
//		  	 - Very little (nearly none) error checking
//		  	 - Designed to act just like a built-in 2D array, but handle memory allocation
//		  	 - No dynamic resizing of memory. 
//		  	 - "reshape(d0, d1, d2)" only sets dimensions. It does NOT affect memory allocation.

#ifndef GLIFT_ARRAY_3_OWN_H_
#define GLIFT_ARRAY_3_OWN_H_

#include <arrayWrap3.h>
#include <assert.h>

namespace gutz {

////////////////////////////////////
// ArrayOwn3
////////////////////////////////////

template <class T>
class arrayOwn3 : public arrayWrap3<T>
{
public:
	// Constructors - Deep copies of input data
  arrayOwn3 ();
  arrayOwn3 (int d0, int d1, int d2, T v ); 
  arrayOwn3 (int d0, int d1, int d3, const T* v);
  arrayOwn3 ( const arrayWrap3<T>& a );
  
  // Copy Constructor, Assignment Operator, and Destructor
  arrayOwn3 (const arrayOwn3<T>& a);
  arrayOwn3<T>& operator= (const arrayOwn3<T>& a);
  ~arrayOwn3() {arrayBase<T>::killData();}

  // Transfer ownership of data ('v') to this object.
  // - Destroys previous contents of 'this' before doing shallow copy of new data
  // - 'killWithDelete' is true if 'v' is allocated with 'new', otherwise false ('malloc')
  void transfer( int d0, int d1, int d3, T* v, bool killWithDelete );

private:
  void initVal (T v);
  void initData (const T* d);
  void copy (const arrayOwn3<T>& a);
};

////////////////////////////////////
// Implementation
////////////////////////////////////

// Constructors
template <class T>
arrayOwn3<T>::arrayOwn3()
  : arrayWrap3<T>()
{}

template <class T>
arrayOwn3<T>::arrayOwn3 (int d0, int d1, int d2, T val)
  : arrayWrap3<T>(d0,d1,d2,NULL)
{
  arrayBase<T>::initValOwn( val );
}

template <class T>
arrayOwn3<T>::arrayOwn3 (int d0, int d1, int d2, const T* data)
  : arrayWrap3<T>(d0,d1,d2,NULL)
{
  arrayBase<T>::initDataOwn( data );
}

template <class T>
arrayOwn3<T>::arrayOwn3 ( const arrayWrap3<T>& a ) : arrayWrap3<T>(a)
{ 
  arrayBase<T>::initDataOwn( a.data() );
}

// Copy Constructor
template <class T>
arrayOwn3<T>::arrayOwn3 (const arrayOwn3<T>& a) : arrayWrap3<T>(a)		 // Shallow copy of a
{
  arrayBase<T>::initDataOwn(a.mData);			 // DEEP copy of a
}
    
// Assignment Operator
template <class T>
arrayOwn3<T>& arrayOwn3<T>::operator= (const arrayOwn3<T>& a) 
{ 
  if( &a != this ) {
    // Kill data and allocate new memory only if sizes don't match.
    if( (this->mSize != a.size() ) || 
	(arrayBase<T>::dim(0) != a.dim(0)) ||
	(arrayBase<T>::dim(1) != a.dim(1)) ||
	(arrayBase<T>::dim(2) != a.dim(2)) ) {
      arrayBase<T>::killData();
      arrayWrap3<T>::operator=(a);	// SHALLOW copy of a
      arrayBase<T>::allocDataOwn();
    }
    arrayBase<T>::copyDataOwn(a.mData);	// Deep copy of a.mData
  }
  return *this; 
}

// Transfer ownership of data ('v') to this object.
// - Destroys previous contents of 'this' before doing shallow copy of new data
template <class T>
void arrayOwn3<T>::transfer( int d0, int d1, int d2, T* v, bool killWithDelete )
{
  arrayBase<T>::killData();
  
  int sizes[3];
  sizes[0] = d0;
  sizes[1] = d1;
  sizes[2] = d2;
  
  set(3, sizes, v);
  this->mSlice = arrayWrap2<T>(d1,d2,v);
  this->mKillWithDelete = killWithDelete;
}

////////////////////////////////////
// Typedefs
////////////////////////////////////

typedef arrayOwn3<char> 	arrayo3c;
typedef arrayOwn3<uchar> 	arrayo3uc;
typedef arrayOwn3<char>	 	arrayo3b;
typedef arrayOwn3<uchar> 	arrayo3ub;
typedef arrayOwn3<short> 	arrayo3s;
typedef arrayOwn3<ushort> 	arrayo3us;
typedef arrayOwn3<int> 		arrayo3i;
typedef arrayOwn3<uint> 	arrayo3ui;
typedef arrayOwn3<long> 	arrayo3l;
typedef arrayOwn3<ulong> 	arrayo3ul;
typedef arrayOwn3<llong> 	arrayo3ll;
typedef arrayOwn3<ullong> 	arrayo3ull;
typedef arrayOwn3<float> 	arrayo3f;
typedef arrayOwn3<double> 	arrayo3d;

#ifdef USING_MATHGUTZ
	typedef arrayOwn3<vec2ub> 	arrayo3v2ub;
	typedef arrayOwn3<vec2i> 	arrayo3v2i;
	typedef arrayOwn3<vec2ui> 	arrayo3v2ui;
	typedef arrayOwn3<vec2f> 	arrayo3v2f;

	typedef arrayOwn3<vec3ub> 	arrayo3v3ub;
	typedef arrayOwn3<vec3i> 	arrayo3v3i;
	typedef arrayOwn3<vec3ui>	arrayo3v3ui;
	typedef arrayOwn3<vec3f> 	arrayo3v3f;

	typedef arrayOwn3<vec4ub> 	arrayo3v4ub;
	typedef arrayOwn3<vec4i> 	arrayo3v4i;
	typedef arrayOwn3<vec4ui>	arrayo3v4ui;
	typedef arrayOwn3<vec4f> 	arrayo3v4f;

	typedef arrayOwn3<mat2ub> 	arrayo3m2ub;
	typedef arrayOwn3<mat2i> 	arrayo3m2i;
	typedef arrayOwn3<mat2ui> 	arrayo3m2ui;
	typedef arrayOwn3<mat2f> 	arrayo3m2f;

	typedef arrayOwn3<mat3ub> 	arrayo3m3ub;
	typedef arrayOwn3<mat3i> 	arrayo3m3i;
	typedef arrayOwn3<mat3ui> 	arrayo3m3ui;
	typedef arrayOwn3<mat3f> 	arrayo3m3f;

	typedef arrayOwn3<mat4ub> 	arrayo3m4ub;
	typedef arrayOwn3<mat4i> 	arrayo3m4i;
	typedef arrayOwn3<mat4ui> 	arrayo3m4ui;
	typedef arrayOwn3<mat4f> 	arrayo3m4f;
#endif

} // End of namespace gutz

#endif // arrayOwn3_h
