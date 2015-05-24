/*
 * $Id: arrayWrap5.h,v 1.3 2006/04/27 19:58:10 jmk Exp $
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
// 1/13/03	Aaron Lefohn	Scientific Computing and Imaging Institute
//			Joe M. Kniss    School of Computing
//							University of Utah
//
//  arrayWrap5 	- An Array WRAPPER class.
//  	   		- This is a lightweight class that does NOT own its data.
//  	   		- Copies of "arrayWrap5" objects are shallow.
//  	   		- The constructor does NOT allocate any memory.
//		   		- The destructor does NOT free the memory.
//		   		- Accessors (In order of increasing overhead)
//		   			1) The (i,j,k,m) operator is just as fast a built-in C/C++ array.
//		   			2) The [] (slice) operator is next fastest 
//		   			   - 8% overhead for 1 level of slice
//		   			   - 20% overhead for 4D access via slices.
//		   			   - Only use slicing when you need it, otherwise use (i,j,k,m)!
//
////////////////////////////////////////////////////////////////////////

#ifndef ARRAY_WRAP_5_h
#define ARRAY_WRAP_5_h

#include <arrayBase.h>
#include <assert.h>
#include <arrayWrap2.h>

namespace gutz {

/////////////////////////
// arrayWrap5
/////////////////////////
template <class T>
class arrayWrap5 : public arrayBase<T>
{
public:
	// Constructors - Shallow wrap around data. No size checking!
 arrayWrap5() : arrayBase<T>() {}
 arrayWrap5( int d0, int d1, int d2, int d3, int d4, T* v ) : arrayBase<T>(d0,d1,d2,d3,d4,v) {}
 arrayWrap5( const arrayWrap5<T>& a ) : arrayBase<T>(a) { mSlice = a.mSlice; }
  arrayWrap5<T>& operator=( const arrayWrap5<T>& a );

  virtual ~arrayWrap5();
  
  // Return a slice of data - Optimized by returning reference to arrayWrap3<T> member.
  inline arrayWrap4<T>&		operator[] (int i);
  inline const arrayWrap4<T>& operator[] (int i) const;
  
  // Return the element at data(i,j,k,m) - Faster than arr[i][j][k][m]
  inline T&					operator() (int i, int j, int k, int m, int n);
  inline const T&				operator() (int i, int j, int k, int m, int n) const;

protected:

  // The slice wrapper used in the [] operator. 
  // Mutable so that it can be changed by a const operator
  mutable arrayWrap4<T>  mSlice;	
	
private:
  inline void setSlice( int i ) const;		   				   // Set 'mSlice' to the ith slice of 'mData'
  inline int  address(  int i, int j, int k, int m, int n) const;// Get the address of the data(i,j)'th element
  inline void setData( T* d ) { this->mData = d; }					   // Reset just data pointer
};

////////////////////////////////////
// Implementation
////////////////////////////////////

// Constructors - All are shallow!!
//template <class T>
//arrayWrap5<T>::arrayWrap5 ()
//:arrayBase<T>(), mSlice(arrayWrap4<T>())
//{}

//template <class T>
//arrayWrap5<T>::arrayWrap5 (int d0, int d1, int d2, int d3, int d4, T* v)
//:arrayBase<T>(d0,d1,d2,d3,d4,v), mSlice(arrayWrap4<T>(d1,d2,d3,d4,v))
//{}

// Copy Ctor
//template <class T>
//arrayWrap5<T>::arrayWrap5<T>( const arrayWrap5<T>& a ) : arrayBase<T>(a)
//{
//	mSlice = a.mSlice;
//}

// Assignment Op
template <class T>
arrayWrap5<T>& arrayWrap5<T>::operator=(const arrayWrap5<T>& a)
{
  if( this != &a ) {
    arrayBase<T>::operator=(a);// Call base class assign. op
    mSlice = a.mSlice;
  }
  return *this;
}

// Destructor
template <class T>
arrayWrap5<T>::~arrayWrap5()
{}

// Accessing Operators
template <class T>
inline arrayWrap4<T>& arrayWrap5<T>::operator[] (int i) 				
{ 
  setSlice(i);
  return mSlice; 
}

template <class T>
inline const arrayWrap4<T>& arrayWrap5<T>::operator[] (int i) const 	
{ 
  setSlice(i);
  return mSlice; 
}

template <class T>
inline T& arrayWrap5<T>::operator() (int i, int j, int k, int m, int n)
{
  return this->mData[ address(i,j,k,m,n) ];
}

template <class T>
inline const T& arrayWrap5<T>::operator() (int i, int j, int k, int m, int n) const
{
  return this->mData[ address(i,j,k,m,n) ];
}

template <class T>
inline int arrayWrap5<T>::address( int i, int j, int k, int m, int n) const
{
  //return i*mAxisDim[3]*mAxisDim[2]*mAxisDim[1] + j*mAxisDim[3]*mAxisDim[2] + k*mAxisDim[3] + m;
  return i*this->mAxisStride[0] + j*this->mAxisStride[1] + k*this->mAxisStride[2] + m*this->mAxisStride[3] + n;
}

// Set 'mSlice' to be the ith slice of 'mData'
template <class T>
inline void arrayWrap5<T>::setSlice( int i ) const
{
  //int a = i * mAxisDim[1] * mAxisDim[2] * mAxisDim[3];
  int a = i * this->mAxisStride[0];
  assert( a < this->mSize );
  mSlice.setData( this->mData + a );
}

////////////////////////////////////
// Typedefs
////////////////////////////////////

typedef arrayWrap5<char> 	arrayw5c;
typedef arrayWrap5<uchar> 	arrayw5uc;
typedef arrayWrap5<char> 	arrayw5b;
typedef arrayWrap5<uchar> 	arrayw5ub;
typedef arrayWrap5<short> 	arrayw5s;
typedef arrayWrap5<ushort> 	arrayw5us;
typedef arrayWrap5<int>	 	arrayw5i;
typedef arrayWrap5<uint> 	arrayw5ui;
typedef arrayWrap5<long> 	arrayw5l;
typedef arrayWrap5<ulong> 	arrayw5ul;
typedef arrayWrap5<llong> 	arrayw5ll;
typedef arrayWrap5<ullong> 	arrayw5ull;
typedef arrayWrap5<float> 	arrayw5f;
typedef arrayWrap5<double> 	arrayw5d;

#ifdef USING_MATHGUTZ
	typedef arrayWrap5<vec2ub> arrayw5v2ub;
	typedef arrayWrap5<vec2i>  arrayw5v2i;
	typedef arrayWrap5<vec2ui> arrayw5v2ui;
	typedef arrayWrap5<vec2f>  arrayw5v2f;

	typedef arrayWrap5<vec3ub> arrayw5v3ub;
	typedef arrayWrap5<vec3i>  arrayw5v3i;
	typedef arrayWrap5<vec3ui> arrayw5v3ui;
	typedef arrayWrap5<vec3f>  arrayw5v3f;

	typedef arrayWrap5<vec4ub> arrayw5v4ub;
	typedef arrayWrap5<vec4i>  arrayw5v4i;
	typedef arrayWrap5<vec4ui> arrayw5v4ui;
	typedef arrayWrap5<vec4f>  arrayw5v4f;

	typedef arrayWrap5<mat2ub> arrayw5m2ub;
	typedef arrayWrap5<mat2i>  arrayw5m2i;
	typedef arrayWrap5<mat2ui> arrayw5m2ui;
	typedef arrayWrap5<mat2f>  arrayw5m2f;

	typedef arrayWrap5<mat3ub> arrayw5m3ub;
	typedef arrayWrap5<mat3i>  arrayw5m3i;
	typedef arrayWrap5<mat3ui> arrayw5m3ui;
	typedef arrayWrap5<mat3f>  arrayw5m3f;

	typedef arrayWrap5<mat4ub> arrayw5m4ub;
	typedef arrayWrap5<mat4i>  arrayw5m4i;
	typedef arrayWrap5<mat4ui> arrayw5m4ui;
	typedef arrayWrap5<mat4f>  arrayw5m4f;
#endif

} // End of namespace gutz

#endif // arrayWrap5_h
