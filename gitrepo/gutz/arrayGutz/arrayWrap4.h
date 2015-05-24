/*
 * $Id: arrayWrap4.h,v 1.3 2006/04/27 19:58:10 jmk Exp $
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
//			Joe M. Kniss    School of Computing
//							University of Utah
//
//  arrayWrap4 	- An Array WRAPPER class.
//  	   		- This is a lightweight class that does NOT own its data.
//  	   		- Copies of "arrayWrap4" objects are shallow.
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

#ifndef ARRAY_WRAP_4_h
#define ARRAY_WRAP_4_h

#include <arrayBase.h>
#include <assert.h>
#include <arrayWrap2.h>

namespace gutz {

// Forward Decl
template <class T> class arrayWrap5;

/////////////////////////
// arrayWrap4
/////////////////////////
template <class T>
class arrayWrap4 : public arrayBase<T>
{
public:
  friend class arrayWrap5<T>;
  
  // Constructors - Shallow wrap around data. No size checking!
 arrayWrap4() : arrayBase<T>() {}
 arrayWrap4( int d0, int d1, int d2, int d3, T* v ): arrayBase<T>(d0,d1,d2,d3,v) {}
 arrayWrap4( const arrayWrap4<T>& a ) : arrayBase<T>(a) { mSlice = a.mSlice; }
  arrayWrap4<T>& operator=( const arrayWrap4<T>& a );

  virtual ~arrayWrap4();
  
  // Return a slice of data - Optimized by returning reference to arrayWrap3<T> member.
  inline arrayWrap3<T>&		operator[] (int i);
  inline const arrayWrap3<T>& operator[] (int i) const;
  
  // Return the element at data(i,j,k,m) - Faster than arr[i][j][k][m]
  inline T&					operator() (int i, int j, int k, int m);
  inline const T&				operator() (int i, int j, int k, int m) const;

protected:

  // The slice wrapper used in the [] operator. 
  // Mutable so that it can be changed by a const operator
  mutable arrayWrap3<T>  mSlice;	
	
private:
  inline void setSlice( int i ) const;		   			 // Set 'mSlice' to the ith slice of 'mData'
  inline int  address(  int i, int j, int k, int m ) const;// Get the address of the data(i,j)'th element
  inline void setData( T* d ) { this->mData = d; }					 // Reset just data pointer
};

////////////////////////////////////
// Implementation
////////////////////////////////////

// Constructors - All are shallow!!
//template <class T>
//arrayWrap4<T>::arrayWrap4 ()
//:arrayBase<T>(), mSlice(arrayWrap3<T>())
//{}

//template <class T>
//arrayWrap4<T>::arrayWrap4 (int d0, int d1, int d2, int d3, T* v)
//:arrayBase<T>(d0,d1,d2,d3,v), mSlice(arrayWrap3<T>(d1,d2,d3,v))
//{}

// Copy Ctor
//template <class T>
//arrayWrap4<T>::arrayWrap4<T>( const arrayWrap4<T>& a ) : arrayBase<T>(a)
//{
//	mSlice = a.mSlice;
//}

// Assignment Op
template <class T>
arrayWrap4<T>& arrayWrap4<T>::operator=(const arrayWrap4<T>& a)
{
  if( this != &a ) {
    arrayBase<T>::operator=(a);// Call base class assign. op
    mSlice = a.mSlice;
  }
  return *this;
}

// Destructor
template <class T>
arrayWrap4<T>::~arrayWrap4()
{}

// Accessing Operators
template <class T>
inline arrayWrap3<T>& arrayWrap4<T>::operator[] (int i) 				
{ 
  setSlice(i);
  return mSlice; 
}

template <class T>
inline const arrayWrap3<T>& arrayWrap4<T>::operator[] (int i) const 	
{ 
  setSlice(i);
  return mSlice; 
}

template <class T>
inline T& arrayWrap4<T>::operator() (int i, int j, int k, int m)
{
  return this->mData[ address(i,j,k,m) ];
}

template <class T>
inline const T& arrayWrap4<T>::operator() (int i, int j, int k, int m) const
{
  return this->mData[ address(i,j,k,m) ];
}

template <class T>
inline int arrayWrap4<T>::address( int i, int j, int k, int m) const
{
  //return i*mAxisDim[3]*mAxisDim[2]*mAxisDim[1] + j*mAxisDim[3]*mAxisDim[2] + k*mAxisDim[3] + m;
  return i*this->mAxisStride[0] + j*this->mAxisStride[1] + k*this->mAxisStride[2] + m;
}

// Set 'mSlice' to be the ith slice of 'mData'
template <class T>
inline void arrayWrap4<T>::setSlice( int i ) const
{
  //int a = i * mAxisDim[1] * mAxisDim[2] * mAxisDim[3];
  int a = i * this->mAxisStride[0];
  assert( a < this->mSize );
  mSlice.setData( this->mData + a );
}

////////////////////////////////////
// Typedefs
////////////////////////////////////

typedef arrayWrap4<char> 	arrayw4c;
typedef arrayWrap4<uchar> 	arrayw4uc;
typedef arrayWrap4<char> 	arrayw4b;
typedef arrayWrap4<uchar> 	arrayw4ub;
typedef arrayWrap4<short> 	arrayw4s;
typedef arrayWrap4<ushort> 	arrayw4us;
typedef arrayWrap4<int>	 	arrayw4i;
typedef arrayWrap4<uint> 	arrayw4ui;
typedef arrayWrap4<long> 	arrayw4l;
typedef arrayWrap4<ulong> 	arrayw4ul;
typedef arrayWrap4<llong> 	arrayw4ll;
typedef arrayWrap4<ullong> 	arrayw4ull;
typedef arrayWrap4<float> 	arrayw4f;
typedef arrayWrap4<double> 	arrayw4d;

#ifdef USING_MATHGUTZ
	typedef arrayWrap4<vec2ub> arrayw4v2ub;
	typedef arrayWrap4<vec2i>  arrayw4v2i;
	typedef arrayWrap4<vec2ui> arrayw4v2ui;
	typedef arrayWrap4<vec2f>  arrayw4v2f;

	typedef arrayWrap4<vec3ub> arrayw4v3ub;
	typedef arrayWrap4<vec3i>  arrayw4v3i;
	typedef arrayWrap4<vec3ui> arrayw4v3ui;
	typedef arrayWrap4<vec3f>  arrayw4v3f;

	typedef arrayWrap4<vec4ub> arrayw4v4ub;
	typedef arrayWrap4<vec4i>  arrayw4v4i;
	typedef arrayWrap4<vec4ui> arrayw4v4ui;
	typedef arrayWrap4<vec4f>  arrayw4v4f;

	typedef arrayWrap4<mat2ub> arrayw4m2ub;
	typedef arrayWrap4<mat2i>  arrayw4m2i;
	typedef arrayWrap4<mat2ui> arrayw4m2ui;
	typedef arrayWrap4<mat2f>  arrayw4m2f;

	typedef arrayWrap4<mat3ub> arrayw4m3ub;
	typedef arrayWrap4<mat3i>  arrayw4m3i;
	typedef arrayWrap4<mat3ui> arrayw4m3ui;
	typedef arrayWrap4<mat3f>  arrayw4m3f;

	typedef arrayWrap4<mat4ub> arrayw4m4ub;
	typedef arrayWrap4<mat4i>  arrayw4m4i;
	typedef arrayWrap4<mat4ui> arrayw4m4ui;
	typedef arrayWrap4<mat4f>  arrayw4m4f;
#endif

} // End of namespace gutz

#endif // arrayWrap4_h
