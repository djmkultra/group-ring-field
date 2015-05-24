/*
 * $Id: arrayOwn1.h,v 1.3 2006/04/27 19:58:10 jmk Exp $
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
// 9/7/02	Aaron Lefohn	Scientific Computing and Imaging Institute
//							School of Computing
//							University of Utah
////////////////////////////////////////////////////////////////////////
//
// arrayOwn1 - An array that OWNS its data
// 			 - The constructors copy the data in
// 			 - The destructor deletes the data
// 			 
////////////////////////////////////////////////////////////////////////

#ifndef __ARRAY_OWN_1_H__
#define __ARRAY_OWN_1_H__

#include <arrayWrap1.h>
#include <assert.h>

namespace gutz {

////////////////////////////////////
// ArrayOwn1
////////////////////////////////////

template <class T>
class arrayOwn1 : public arrayWrap1<T>
{
public:
  // Constructors - Deep Copies of data
  arrayOwn1();
  arrayOwn1( int s, T v ); 
  arrayOwn1( int s, const T* v);
  arrayOwn1( const arrayWrap1<T>& a );
  
  arrayOwn1( const arrayOwn1<T>& a );
  arrayOwn1<T>& operator=( const arrayOwn1<T>& a );
  
  ~arrayOwn1() {arrayBase<T>::killData();}
  
  // Transfer ownership of data ('v') to this object.
  // - Destroys previous contents of 'this' before doing shallow copy of new data
  // - 'killWithDelete' is true if 'v' is allocated with 'new', otherwise false ('malloc')
  void transfer( int s, T* v, bool killWithDelete );
     	
private:
  void initVal (T v);
  void initData (const T* v);
};


////////////////////////////////////
// Implementation
////////////////////////////////////

// Constructors
template <class T>
  arrayOwn1<T>::arrayOwn1 ()
  : arrayWrap1<T>()
    {}

template <class T>
arrayOwn1<T>::arrayOwn1 (int s, T v)
  : arrayWrap1<T>(s,NULL)
{
    arrayBase<T>::initValOwn( v );
}

template <class T>
arrayOwn1<T>::arrayOwn1 (int s, const T* v)
  : arrayWrap1<T>(s,NULL)
{
  arrayBase<T>::initDataOwn( v );
}

template <class T>
arrayOwn1<T>::arrayOwn1 ( const arrayWrap1<T>& a)
  : arrayWrap1<T>(a.size(),NULL)
{
  arrayBase<T>::initDataOwn(a.data());
}

// Copy Constructor: "this" does NOT exist yet
template <class T>
arrayOwn1<T>::arrayOwn1( const arrayOwn1<T>& a ) : arrayWrap1<T>(a) // SHALLOW copy 'a' 
{
  arrayBase<T>::initDataOwn(a.mData);								// DEEP copy of a
}

// Assignment operator: "this" DOES exist
template <class T>
arrayOwn1<T>& arrayOwn1<T>::operator=( const arrayOwn1<T>& a )
{
  if( &a != this ) {
    // Kill data and allocate new memory only if sizes don't match.
    if( (this->mSize != a.size() ) || 
	(arrayBase<T>::dim(0) != a.dim(0)) ) {
      arrayBase<T>::killData();
      arrayWrap1<T>::operator=(a);	// SHALLOW copy of a
      arrayBase<T>::allocDataOwn();
    }
    arrayBase<T>::copyDataOwn(a.mData);	// Deep copy of a.mData
  }
  return *this; 
}

// Transfer ownership of data ('v') to this object.
// - Destroys previous contents of 'this' before doing shallow copy of new data
template <class T>
void arrayOwn1<T>::transfer( int s, T* v, bool killWithDelete )
{
  arrayBase<T>::killData();
  
  int sizes[1];
  sizes[0] = s;
  set(1, sizes, v);
  
  this->mKillWithDelete = killWithDelete;
}


////////////////////////////////////
// Typedefs
////////////////////////////////////

typedef arrayOwn1<char> 		arrayo1c;
typedef arrayOwn1<uchar> 		arrayo1uc;
typedef arrayOwn1<char> 		arrayo1b;
typedef arrayOwn1<uchar> 		arrayo1ub;
typedef arrayOwn1<short> 		arrayo1s;
typedef arrayOwn1<ushort> 		arrayo1us;
typedef arrayOwn1<int> 			arrayo1i;
typedef arrayOwn1<uint>	 		arrayo1ui;
typedef arrayOwn1<long> 		arrayo1l;
typedef arrayOwn1<ulong> 		arrayo1ul;
typedef arrayOwn1<llong> 		arrayo1ll;
typedef arrayOwn1<ullong> 		arrayo1ull;
typedef arrayOwn1<float> 		arrayo1f;
typedef arrayOwn1<double> 		arrayo1d;

#ifdef USING_MATHGUTZ
	typedef arrayOwn1<vec2ub> 	arrayo1v2ub;
	typedef arrayOwn1<vec2i> 	arrayo1v2i;
	typedef arrayOwn1<vec2ui> 	arrayo1v2ui;
	typedef arrayOwn1<vec2f> 	arrayo1v2f;

	typedef arrayOwn1<vec3ub> 	arrayo1v3ub;
	typedef arrayOwn1<vec3i> 	arrayo1v3i;
	typedef arrayOwn1<vec3ui> 	arrayo1v3ui;
	typedef arrayOwn1<vec3f> 	arrayo1v3f;

	typedef arrayOwn1<vec4ub> 	arrayo1v4ub;
	typedef arrayOwn1<vec4i> 	arrayo1v4i;
	typedef arrayOwn1<vec4ui> 	arrayo1v4ui;
	typedef arrayOwn1<vec4f> 	arrayo1v4f;

	typedef arrayOwn1<mat2ub> 	arrayo1m2ub;
	typedef arrayOwn1<mat2i> 	arrayo1m2i;
	typedef arrayOwn1<mat2ui> 	arrayo1m2ui;
	typedef arrayOwn1<mat2f> 	arrayo1m2f;

	typedef arrayOwn1<mat3ub> 	arrayo1m3ub;
	typedef arrayOwn1<mat3i> 	arrayo1m3i;
	typedef arrayOwn1<mat3ui> 	arrayo1m3ui;
	typedef arrayOwn1<mat3f> 	arrayo1m3f;

	typedef arrayOwn1<mat4ub> 	arrayo1m4ub;
	typedef arrayOwn1<mat4i> 	arrayo1m4i;
	typedef arrayOwn1<mat4ui> 	arrayo1m4ui;
	typedef arrayOwn1<mat4f> 	arrayo1m4f;
#endif

} // End of namespace gutz

#endif // arrayOwn1
