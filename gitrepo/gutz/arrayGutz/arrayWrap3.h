/*
 * $Id: arrayWrap3.h,v 1.3 2006/04/27 19:58:10 jmk Exp $
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
//  arrayWrap3 	- An Array WRAPPER class.
//  	   		- This is a lightweight class that does NOT own its data.
//  	   		- Copies of "arrayWrap3" objects are shallow.
//  	   		- The constructor does NOT allocate any memory.
//		   		- The destructor does NOT free the memory.
//		   		- Accessors (In order of increasing overhead)
//		   			1) The (i,j,k) operator is just as fast a built-in C/C++ array.
//		   			2) The [] (slice) operator is next fastest 
//		   			   - 10% overhead for 1 level of slice
//		   			   - 15% overhead for 3D access via slices.
//		   			   - Only use slicing when you need it, otherwise use (i,j,k)!
//
////////////////////////////////////////////////////////////////////////
#ifndef ARRAY_WRAP_3_h
#define ARRAY_WRAP_3_h

#include <arrayBase.h>
#include <assert.h>
#include <arrayWrap2.h>

namespace gutz {

// Forward Decl
template <class T> class arrayWrap4;

/////////////////////////
// arrayWrap3
/////////////////////////
template <class T>
class arrayWrap3 : public arrayBase<T>
{
public:
  friend class arrayWrap4<T>;
  
  // Constructors - Shallow wrap around data. No size checking!
 arrayWrap3() : arrayBase<T>() {}
 arrayWrap3( int d0, int d1, int d2, T* v ) : arrayBase<T>(d0,d1,d2,v) {}
 arrayWrap3( const arrayWrap3<T>& a ) : arrayBase<T>(a) { mSlice = a.mSlice; }
  arrayWrap3<T>& operator=( const arrayWrap3<T>& a );

  virtual ~arrayWrap3();
  
  // Return a slice of data - Optimized by returning reference to arrayWrap2<T> member.
  inline arrayWrap2<T>&		operator[] (int i);
  inline const arrayWrap2<T>& operator[] (int i) const;

  // Return the element at data(i,j,k) - Faster than arr[i][j][k]
  inline T&					operator() (int i, int j, int k);
  inline const T&				operator() (int i, int j, int k) const;

protected:

  // The slice wrapper used in the [] operator. 
  // Mutable so that it can be changed by a const operator
  mutable arrayWrap2<T>  mSlice;
		
private:
  inline void setSlice( int i ) const;					// Set 'mSlice' to the ith slice of 'mData'
  inline int  address(  int i, int j, int k ) const;		// Get the address of the data(i,j)'th element
  inline void setData( T* d ) { this->mData = d; }					// Reset just data pointer
};

////////////////////////////////////
// Implementation
////////////////////////////////////

// Constructors - All are shallow!!
//template <class T>
//arrayWrap3<T>::arrayWrap3 ()
//:arrayBase<T>(), mSlice(arrayWrap2<T>())
//{}


//template <class T>
//arrayWrap3<T>::arrayWrap3 (int d0, int d1, int d2, T* v)
//:arrayBase<T>(d0, d1, d2, v), mSlice(arrayWrap2<T>(d1,d2,v))
//{}

// Copy Ctor
//template <class T>
//arrayWrap3<T>::arrayWrap3<T>( const arrayWrap3<T>& a ) : arrayBase<T>(a)
//{
//	mSlice = a.mSlice;
//}

// Assignment Op
template <class T>
arrayWrap3<T>& arrayWrap3<T>::operator=(const arrayWrap3<T>& a)
{
	if( this != &a ) {
		arrayBase<T>::operator=(a);// Call base class assign. op
		mSlice = a.mSlice;
	}
	return *this;
}

// Destructor
template <class T>
arrayWrap3<T>::~arrayWrap3()
{}

// Accessing Operators
template <class T>
inline arrayWrap2<T>& arrayWrap3<T>::operator[] (int i) 				
{ 
	setSlice(i);
	return mSlice; 
}

template <class T>
inline const arrayWrap2<T>& arrayWrap3<T>::operator[] (int i) const 	
{ 
	setSlice(i);
	return mSlice; 
}

// Set 'mSlice' to be the ith slice of 'mData'
template <class T>
inline void arrayWrap3<T>::setSlice( int i ) const
{
	//int a = i * mAxisDim[1] * mAxisDim[2];
	int a = i * this->mAxisStride[0];
	assert( a < this->mSize );
	mSlice.setData( this->mData + a );
}

// (i,j,k)'th element Accessors
template <class T>
inline T& arrayWrap3<T>::operator() (int i, int j, int k)
{
	return this->mData[ address(i,j,k) ];
}

template <class T>
inline const T& arrayWrap3<T>::operator() (int i, int j, int k) const
{
	return this->mData[ address(i,j,k) ];
}

template <class T>
inline int arrayWrap3<T>::address( int i, int j, int k) const
{
	//return i*mAxisDim[2]*mAxisDim[1]+ j*mAxisDim[2] + k;
	return i*this->mAxisStride[0]+ j*this->mAxisStride[1] + k;
}

////////////////////////////////////
// Typedefs
////////////////////////////////////

typedef arrayWrap3<char> 	arrayw3c;
typedef arrayWrap3<uchar> 	arrayw3uc;
typedef arrayWrap3<char> 	arrayw3b;
typedef arrayWrap3<uchar> 	arrayw3ub;
typedef arrayWrap3<short> 	arrayw3s;
typedef arrayWrap3<ushort> 	arrayw3us;
typedef arrayWrap3<int>	 	arrayw3i;
typedef arrayWrap3<uint> 	arrayw3ui;
typedef arrayWrap3<long> 	arrayw3l;
typedef arrayWrap3<ulong> 	arrayw3ul;
typedef arrayWrap3<llong> 	arrayw3ll;
typedef arrayWrap3<ullong> 	arrayw3ull;
typedef arrayWrap3<float> 	arrayw3f;
typedef arrayWrap3<double> 	arrayw3d;

#ifdef USING_MATHGUTZ
	typedef arrayWrap3<vec2ub> arrayw3v2ub;
	typedef arrayWrap3<vec2i>  arrayw3v2i;
	typedef arrayWrap3<vec2ui> arrayw3v2ui;
	typedef arrayWrap3<vec2f>  arrayw3v2f;

	typedef arrayWrap3<vec3ub> arrayw3v3ub;
	typedef arrayWrap3<vec3i>  arrayw3v3i;
	typedef arrayWrap3<vec3ui> arrayw3v3ui;
	typedef arrayWrap3<vec3f>  arrayw3v3f;

	typedef arrayWrap3<vec4ub> arrayw3v4ub;
	typedef arrayWrap3<vec4i>  arrayw3v4i;
	typedef arrayWrap3<vec4ui> arrayw3v4ui;
	typedef arrayWrap3<vec4f>  arrayw3v4f;

	typedef arrayWrap3<mat2ub> arrayw3m2ub;
	typedef arrayWrap3<mat2i>  arrayw3m2i;
	typedef arrayWrap3<mat2ui> arrayw3m2ui;
	typedef arrayWrap3<mat2f>  arrayw3m2f;

	typedef arrayWrap3<mat3ub> arrayw3m3ub;
	typedef arrayWrap3<mat3i>  arrayw3m3i;
	typedef arrayWrap3<mat3ui> arrayw3m3ui;
	typedef arrayWrap3<mat3f>  arrayw3m3f;

	typedef arrayWrap3<mat4ub> arrayw3m4ub;
	typedef arrayWrap3<mat4i>  arrayw3m4i;
	typedef arrayWrap3<mat4ui> arrayw3m4ui;
	typedef arrayWrap3<mat4f>  arrayw3m4f;
#endif

} // End of namespace gutz

#endif // arrayWrap3_h
