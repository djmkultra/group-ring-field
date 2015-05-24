/*
 * $Id: arrayOwn4.h,v 1.3 2006/04/27 19:58:10 jmk Exp $
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
// arrayOwn4 - A lightweight 4D array class that OWNS its data.
//		  	 - Very little (nearly none) error checking
//		  	 - Designed to act just like a built-in 2D array, but handle memory allocation
//		  	 - No dynamic resizing of memory. 
//		  	 - "reshape(d0, d1, d2, d3)" only sets dimensions. It does NOT affect memory allocation.

#ifndef GLIFT_ARRAY_4_OWN_H_
#define GLIFT_ARRAY_4_OWN_H_

#include <arrayWrap4.h>
#include <assert.h>

namespace gutz {

////////////////////////////////////
// ArrayOwn4  
////////////////////////////////////

template <class T>
class arrayOwn4 : public arrayWrap4<T>
{
public:
	// Constructors - Deep copies of input data
  arrayOwn4 ();
  arrayOwn4 (int d0, int d1, int d2, int d3, T v ); 
  arrayOwn4 (int d0, int d1, int d2, int d3, const T* v);
  arrayOwn4 ( const arrayWrap4<T>& a );

  // Copy Constructor, Assignment Operator, and Destructor
  arrayOwn4 (const arrayOwn4<T>& a);
  arrayOwn4<T>& operator= (const arrayOwn4<T>& a);
  ~arrayOwn4() {arrayBase<T>::killData();}

  // Transfer ownership of data ('v') to this object.
  // - Destroys previous contents of 'this' before doing shallow copy of new data
  // - 'killWithDelete' is true if 'v' is allocated with 'new', otherwise false ('malloc')
  void transfer( int d0, int d1, int d3, int d4, T* v, bool killWithDelete );

private:
  void initVal (T v);
  void initData (const T* d);
  void copy (const arrayOwn4<T>& a);
};

////////////////////////////////////
// Implementation
////////////////////////////////////

// Constructors
template <class T>
arrayOwn4<T>::arrayOwn4()
  : arrayWrap4<T>()
{}

template <class T>
arrayOwn4<T>::arrayOwn4 (int d0, int d1, int d2, int d3, T val)
  : arrayWrap4<T>(d0, d1, d2, d3, NULL)
{
  arrayBase<T>::initValOwn( val );
}

template <class T>
arrayOwn4<T>::arrayOwn4 (int d0, int d1, int d2, int d3, const T* data)
  : arrayWrap4<T>(d0, d1, d2, d3, NULL)
{
  arrayBase<T>::initDataOwn( data );
}

template <class T>
arrayOwn4<T>::arrayOwn4 ( const arrayWrap4<T>& a ) : arrayWrap4<T>(a)
{
  arrayBase<T>::initDataOwn( a.data() );
}

// Copy Constructor
template <class T>
arrayOwn4<T>::arrayOwn4 (const arrayOwn4<T>& a) : arrayWrap4<T>(a)
												 // Shallow copy of a
{
  arrayBase<T>::initDataOwn(a.mData);			 // DEEP copy of a
}
    
// Assignment Operator
template <class T>
arrayOwn4<T>& arrayOwn4<T>::operator= (const arrayOwn4<T>& a) 
{ 
  if( &a != this ) {
    if( (this->mSize != a.size() ) || 
	(arrayBase<T>::dim(0) != a.dim(0)) ||
	(arrayBase<T>::dim(1) != a.dim(1)) ||
	(arrayBase<T>::dim(2) != a.dim(2)) ||
	(arrayBase<T>::dim(3) != a.dim(3)) ) {
      arrayBase<T>::killData();	
      arrayWrap4<T>::operator=(a);		// SHALLOW copy of a
      arrayBase<T>::allocDataOwn();
    }
    arrayBase<T>::copyDataOwn(a.mData);	// Deep copy of a.mData
  }
  return *this; 
}

// Transfer ownership of data ('v') to this object.
// - Destroys previous contents of 'this' before doing shallow copy of new data
template <class T>
void arrayOwn4<T>::transfer( int d0, int d1, int d2, int d3, T* v, bool killDataWithDelete )
{
  arrayBase<T>::killData();
  
  int sizes[4];
  sizes[0] = d0;
  sizes[1] = d1;
  sizes[2] = d2;
  sizes[3] = d3;
  
  set(4, sizes, v);
  this->mSlice = arrayWrap3<T>(d1,d2,d3,v);
  this->mKillWithDelete = killDataWithDelete;
}



////////////////////////////////////
// Typedefs
////////////////////////////////////

typedef arrayOwn4<char> 	arrayo4c;
typedef arrayOwn4<uchar> 	arrayo4uc;
typedef arrayOwn4<char>	 	arrayo4b;
typedef arrayOwn4<uchar> 	arrayo4ub;
typedef arrayOwn4<short> 	arrayo4s;
typedef arrayOwn4<ushort> 	arrayo4us;
typedef arrayOwn4<int> 		arrayo4i;
typedef arrayOwn4<uint> 	arrayo4ui;
typedef arrayOwn4<long> 	arrayo4l;
typedef arrayOwn4<ulong> 	arrayo4ul;
typedef arrayOwn4<llong> 	arrayo4ll;
typedef arrayOwn4<ullong> 	arrayo4ull;
typedef arrayOwn4<float> 	arrayo4f;
typedef arrayOwn4<double> 	arrayo4d;

#ifdef USING_MATHGUTZ
	typedef arrayOwn4<vec2ub> 	arrayo4v2ub;
	typedef arrayOwn4<vec2i> 	arrayo4v2i;
	typedef arrayOwn4<vec2ui> 	arrayo4v2ui;
	typedef arrayOwn4<vec2f> 	arrayo4v2f;

	typedef arrayOwn4<vec3ub> 	arrayo4v3ub;
	typedef arrayOwn4<vec3i> 	arrayo4v3i;
	typedef arrayOwn4<vec3ui>	arrayo4v3ui;
	typedef arrayOwn4<vec3f> 	arrayo4v3f;

	typedef arrayOwn4<vec4ub> 	arrayo4v4ub;
	typedef arrayOwn4<vec4i> 	arrayo4v4i;
	typedef arrayOwn4<vec4ui>	arrayo4v4ui;
	typedef arrayOwn4<vec4f> 	arrayo4v4f;

	typedef arrayOwn4<mat2ub> 	arrayo4m2ub;
	typedef arrayOwn4<mat2i> 	arrayo4m2i;
	typedef arrayOwn4<mat2ui> 	arrayo4m2ui;
	typedef arrayOwn4<mat2f> 	arrayo4m2f;

	typedef arrayOwn4<mat3ub> 	arrayo4m3ub;
	typedef arrayOwn4<mat3i> 	arrayo4m3i;
	typedef arrayOwn4<mat3ui> 	arrayo4m3ui;
	typedef arrayOwn4<mat3f> 	arrayo4m3f;

	typedef arrayOwn4<mat4ub> 	arrayo4m4ub;
	typedef arrayOwn4<mat4i> 	arrayo4m4i;
	typedef arrayOwn4<mat4ui> 	arrayo4m4ui;
	typedef arrayOwn4<mat4f> 	arrayo4m4f;
#endif

} // End of namespace gutz

#endif // arrayOwn4_h
