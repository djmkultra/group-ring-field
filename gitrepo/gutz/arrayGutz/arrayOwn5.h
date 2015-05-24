/*
 * $Id: arrayOwn5.h,v 1.3 2006/04/27 19:58:10 jmk Exp $
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
// 1/13/03	Aaron Lefohn	Scientific Computing and Imaging Institute
//							School of Computing
//							University of Utah
// 
// arrayOwn5 - A lightweight 5D array class that OWNS its data.
//		  	 - Very little (nearly none) error checking
//		  	 - Designed to act just like a built-in 2D array, but handle memory allocation
//		  	 - No dynamic resizing of memory. 
//		  	 - "reshape(d0, d1, d2, d2, d3)" only sets dimensions. It does NOT affect memory allocation.

#ifndef GLIFT_ARRAY_5_OWN_H_
#define GLIFT_ARRAY_5_OWN_H_

#include <arrayWrap5.h>
#include <assert.h>

namespace gutz {

////////////////////////////////////
// arrayOwn5  
////////////////////////////////////

template <class T>
class arrayOwn5 : public arrayWrap5<T>
{
public:
	// Constructors - Deep copies of input data
  arrayOwn5 ();
  arrayOwn5 (int d0, int d1, int d2, int d3, int d4, T v ); 
  arrayOwn5 (int d0, int d1, int d2, int d3, int d4, const T* v);
  arrayOwn5 ( const arrayWrap5<T>& a );
  
  // Copy Constructor, Assignment Operator, and Destructor
  arrayOwn5 (const arrayOwn5<T>& a);
  arrayOwn5<T>& operator= (const arrayOwn5<T>& a);
  ~arrayOwn5() {arrayBase<T>::killData();}
  
  // Transfer ownership of data ('v') to this object.
  // - Destroys previous contents of 'this' before doing shallow copy of new data
  // - 'killWithDelete' is true if 'v' is allocated with 'new', otherwise false ('malloc')
  void transfer( int d0, int d1, int d3, int d4, int d5, T* v, bool killWithDelete );

private:
  void initVal (T v);
  void initData (const T* d);
  void copy (const arrayOwn5<T>& a);
};

////////////////////////////////////
// Implementation
////////////////////////////////////

// Constructors
template <class T>
arrayOwn5<T>::arrayOwn5()
  : arrayWrap5<T>()
{}

template <class T>
arrayOwn5<T>::arrayOwn5 (int d0, int d1, int d2, int d3, int d4, T val)
  : arrayWrap5<T>(d0, d1, d2, d3, d4, NULL)
{
  arrayBase<T>::initValOwn( val );
}

template <class T>
arrayOwn5<T>::arrayOwn5 (int d0, int d1, int d2, int d3, int d4, const T* data)
  : arrayWrap5<T>(d0, d1, d2, d3, d4, NULL)
{
  arrayBase<T>::initDataOwn( data );
}

template <class T>
arrayOwn5<T>::arrayOwn5 ( const arrayWrap5<T>& a ) : arrayWrap5<T>(a)
{
  arrayBase<T>::initDataOwn( a.data() );
}

// Copy Constructor
template <class T>
arrayOwn5<T>::arrayOwn5 (const arrayOwn5<T>& a) : arrayWrap5<T>(a) // Shallow copy of a
{
	arrayBase<T>::initDataOwn(a.mData);			 // DEEP copy of a
}
    
// Assignment Operator
template <class T>
arrayOwn5<T>& arrayOwn5<T>::operator= (const arrayOwn5<T>& a) 
{ 
  if( &a != this ) {
    if( (this->mSize != a.size() ) || 
	(arrayBase<T>::dim(0) != a.dim(0)) ||
	(arrayBase<T>::dim(1) != a.dim(1)) ||
	(arrayBase<T>::dim(2) != a.dim(2)) ||
	(arrayBase<T>::dim(3) != a.dim(3)) ||
	(arrayBase<T>::dim(4) != a.dim(4)) ) {
      arrayBase<T>::killData();	
      arrayWrap5<T>::operator=(a);	// SHALLOW copy of a
      arrayBase<T>::allocDataOwn();
    }
    arrayBase<T>::copyDataOwn(a.mData);	// Deep copy of a.mData
  }
  return *this; 
}

// Transfer ownership of data ('v') to this object.
// - Destroys previous contents of 'this' before doing shallow copy of new data
template <class T>
void arrayOwn5<T>::transfer( int d0, int d1, int d2, int d3, int d4, T* v, bool killWithDelete )
{
  arrayBase<T>::killData();
  
  int sizes[5];
  sizes[0] = d0;
  sizes[1] = d1;
  sizes[2] = d2;
  sizes[3] = d3;
  sizes[4] = d4;
  
  set(5, sizes, v);
  this->mSlice = arrayWrap4<T>(d1,d2,d3,d4,v);
  this->mKillWithDelete = killWithDelete;
}

////////////////////////////////////
// Typedefs
////////////////////////////////////

typedef arrayOwn5<char> 	arrayo5c;
typedef arrayOwn5<uchar> 	arrayo5uc;
typedef arrayOwn5<char>	 	arrayo5b;
typedef arrayOwn5<uchar> 	arrayo5ub;
typedef arrayOwn5<short> 	arrayo5s;
typedef arrayOwn5<ushort> 	arrayo5us;
typedef arrayOwn5<int> 		arrayo5i;
typedef arrayOwn5<uint> 	arrayo5ui;
typedef arrayOwn5<long> 	arrayo5l;
typedef arrayOwn5<ulong> 	arrayo5ul;
typedef arrayOwn5<llong> 	arrayo5ll;
typedef arrayOwn5<ullong> 	arrayo5ull;
typedef arrayOwn5<float> 	arrayo5f;
typedef arrayOwn5<double> 	arrayo5d;

#ifdef USING_MATHGUTZ
	typedef arrayOwn5<vec2ub> 	arrayo5v2ub;
	typedef arrayOwn5<vec2i> 	arrayo5v2i;
	typedef arrayOwn5<vec2ui> 	arrayo5v2ui;
	typedef arrayOwn5<vec2f> 	arrayo5v2f;

	typedef arrayOwn5<vec3ub> 	arrayo5v3ub;
	typedef arrayOwn5<vec3i> 	arrayo5v3i;
	typedef arrayOwn5<vec3ui>	arrayo5v3ui;
	typedef arrayOwn5<vec3f> 	arrayo5v3f;

	typedef arrayOwn5<vec4ub> 	arrayo5v4ub;
	typedef arrayOwn5<vec4i> 	arrayo5v4i;
	typedef arrayOwn5<vec4ui>	arrayo5v4ui;
	typedef arrayOwn5<vec4f> 	arrayo5v4f;

	typedef arrayOwn5<mat2ub> 	arrayo5m2ub;
	typedef arrayOwn5<mat2i> 	arrayo5m2i;
	typedef arrayOwn5<mat2ui> 	arrayo5m2ui;
	typedef arrayOwn5<mat2f> 	arrayo5m2f;

	typedef arrayOwn5<mat3ub> 	arrayo5m3ub;
	typedef arrayOwn5<mat3i> 	arrayo5m3i;
	typedef arrayOwn5<mat3ui> 	arrayo5m3ui;
	typedef arrayOwn5<mat3f> 	arrayo5m3f;

	typedef arrayOwn5<mat4ub> 	arrayo5m4ub;
	typedef arrayOwn5<mat4i> 	arrayo5m4i;
	typedef arrayOwn5<mat4ui> 	arrayo5m4ui;
	typedef arrayOwn5<mat4f> 	arrayo5m4f;
#endif

} // End of namespace gutz

#endif // arrayOwn5_h
