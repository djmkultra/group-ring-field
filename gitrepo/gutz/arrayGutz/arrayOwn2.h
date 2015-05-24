/*
 * $Id: arrayOwn2.h,v 1.3 2006/04/27 19:58:10 jmk Exp $
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
// arrayOwn2 - A lightweight 2D array class that OWNS its data.
//		  - Very little (nearly none) error checking
//		  - Designed to act just like a built-in 2D array, but handle memory allocation
//		  - No dynamic resizing of memory. 
//		  - "reshape(dim0,dim1)" only sets dimensions. It does NOT affect memory allocation.

#ifndef GLIFT_ARRAY_2_OWN_H_
#define GLIFT_ARRAY_2_OWN_H_

#include <arrayWrap2.h>
#include <assert.h>

namespace gutz {

////////////////////////////////////
// ArrayOwn2
////////////////////////////////////

template <class T>
class arrayOwn2 : public arrayWrap2<T>
{
public:
	// Constructors - Deep copies of input data
  arrayOwn2 ();
  arrayOwn2 (int d0, int d1, T v ); 
  arrayOwn2 (int d0, int d1, const T* v);
  arrayOwn2 ( const arrayWrap2<T>& a );
  
  // Copy Constructor, Assignment Operator, and Destructor
  arrayOwn2 (const arrayOwn2<T>& a);
  arrayOwn2<T>& operator= (const arrayOwn2<T>& a);
  ~arrayOwn2() {arrayBase<T>::killData();}
  
  // Transfer ownership of data ('v') to this object.
  // - Destroys previous contents of 'this' before doing shallow copy of new data
  // - 'killWithDelete' is true if 'v' is allocated with 'new', otherwise false ('malloc')
  void transfer( int d0, int d1, T* v, bool killWithDelete );

  void resize( int d0, int d1 )
  {
    if( (this->dim(0) != d0) || (this->dim(1) != d1) )
      {
	arrayBase<T>::killData();
	arrayBase<T>::reshape(d0,d1);
	arrayBase<T>::allocDataOwn();
      }
  }

private:
  void initVal (T v);
  void initData (const T* d);
  void copy (const arrayOwn2<T>& a);
};

////////////////////////////////////
// Implementation
////////////////////////////////////

// Constructors
template <class T>
arrayOwn2<T>::arrayOwn2()
  : arrayWrap2<T>()
{}

template <class T>
arrayOwn2<T>::arrayOwn2 (int d0, int d1, T val)
  : arrayWrap2<T>(d0,d1,NULL)
{
    arrayBase<T>::initValOwn( val );
}

template <class T>
arrayOwn2<T>::arrayOwn2 (int d0, int d1, const T* data)
  : arrayWrap2<T>(d0,d1,NULL)
{
  arrayBase<T>::initDataOwn( data );
}

template <class T>
arrayOwn2<T>::arrayOwn2 ( const arrayWrap2<T>& a ) : arrayWrap2<T>(a)
{
  arrayBase<T>::initDataOwn( a.data() );
}

// Copy Constructor: "this" does NOT exist yet
template <class T>
arrayOwn2<T>::arrayOwn2( const arrayOwn2<T>& a ) : arrayWrap2<T>(a)
												 // SHALLOW copy 'a'
{
  arrayBase<T>::initDataOwn(a.mData);			 // DEEP copy of a
}

// Assignment operator: "this" DOES exist
template <class T>
arrayOwn2<T>& arrayOwn2<T>::operator=( const arrayOwn2<T>& a )
{
  if( &a != this ) {
    // Kill data and allocate new memory only if sizes don't match.
    if( (this->mSize != a.size() ) || 
	(arrayBase<T>::dim(0) != a.dim(0)) ||
	(arrayBase<T>::dim(1) != a.dim(1)) ) {
      arrayBase<T>::killData();
      arrayWrap2<T>::operator=(a);	// SHALLOW copy of a
      arrayBase<T>::allocDataOwn();
    }
    arrayBase<T>::copyDataOwn(a.mData);	// Deep copy of a.mData
  }
  return *this; 
}

// Transfer ownership of data ('v') to this object.
// - Destroys previous contents of 'this' before doing shallow copy of new data
template <class T>
void arrayOwn2<T>::transfer( int d0, int d1, T* v, bool killWithDelete )
{
  arrayBase<T>::killData();
	
  int sizes[2];
  sizes[0] = d0;
  sizes[1] = d1;
  
  set(2, sizes, v);
  this->mSlice = arrayWrap1<T>(d1,v);
  this->mKillWithDelete = killWithDelete;
}

////////////////////////////////////
// Typedefs
////////////////////////////////////

typedef arrayOwn2<char> 	arrayo2c;
typedef arrayOwn2<uchar> 	arrayo2uc;
typedef arrayOwn2<char>	 	arrayo2b;
typedef arrayOwn2<uchar> 	arrayo2ub;
typedef arrayOwn2<short> 	arrayo2s;
typedef arrayOwn2<ushort> 	arrayo2us;
typedef arrayOwn2<int> 		arrayo2i;
typedef arrayOwn2<uint> 	arrayo2ui;
typedef arrayOwn2<long> 	arrayo2l;
typedef arrayOwn2<ulong> 	arrayo2ul;
typedef arrayOwn2<llong> 	arrayo2ll;
typedef arrayOwn2<ullong> 	arrayo2ull;
typedef arrayOwn2<float> 	arrayo2f;
typedef arrayOwn2<double> 	arrayo2d;

#ifdef USING_MATHGUTZ
	typedef arrayOwn2<vec2ub> arrayo2v2ub;
	typedef arrayOwn2<vec2i>  arrayo2v2i;
	typedef arrayOwn2<vec2ui> arrayo2v2ui;
	typedef arrayOwn2<vec2f>  arrayo2v2f;

	typedef arrayOwn2<vec3ub> arrayo2v3ub;
	typedef arrayOwn2<vec3i>  arrayo2v3i;
	typedef arrayOwn2<vec3ui> arrayo2v3ui;
	typedef arrayOwn2<vec3f>  arrayo2v3f;

	typedef arrayOwn2<vec4ub> arrayo2v4ub;
	typedef arrayOwn2<vec4i>  arrayo2v4i;
	typedef arrayOwn2<vec4ui> arrayo2v4ui;
	typedef arrayOwn2<vec4f>  arrayo2v4f;

	typedef arrayOwn2<mat2ub> arrayo2m2ub;
	typedef arrayOwn2<mat2i>  arrayo2m2i;
	typedef arrayOwn2<mat2ui> arrayo2m2ui;
	typedef arrayOwn2<mat2f>  arrayo2m2f;

	typedef arrayOwn2<mat3ub> arrayo2m3ub;
	typedef arrayOwn2<mat3i>  arrayo2m3i;
	typedef arrayOwn2<mat3ui> arrayo2m3ui;
	typedef arrayOwn2<mat3f>  arrayo2m3f;

	typedef arrayOwn2<mat4ub> arrayo2m4ub;
	typedef arrayOwn2<mat4i>  arrayo2m4i;
	typedef arrayOwn2<mat4ui> arrayo2m4ui;
	typedef arrayOwn2<mat4f>  arrayo2m4f;
#endif

} // End of namespace gutz

#endif // arrayOwn2_h
