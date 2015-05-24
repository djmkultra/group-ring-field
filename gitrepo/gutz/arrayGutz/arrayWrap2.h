/*
 * $Id: arrayWrap2.h,v 1.3 2006/04/27 19:58:10 jmk Exp $
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
//  arrayWrap2 	- An Array WRAPPER class.
//  	   		- This is a lightweight class that does NOT own its data.
//  	   		- Copies of "arrayWrap2" objects are shallow.
//  	   		- The constructor does NOT allocate any memory.
//		   		- The destructor does NOT free the memory.
//		   		- Accessors (In order of increasing overhead)
//		   			1) The (i,j) operator is just as fast a built-in C/C++ array.
//		   			2) The [] (slice) operator is next fastest 
//		   			   - 8% overhead for 2D access via slices.
//		   			   - Only use slicing when you need it, otherwise use (i,j)!
//		   			
////////////////////////////////////////////////////////////////////////

#ifndef ARRAY_WRAP_2_h
#define ARRAY_WRAP_2_h

#include <arrayBase.h>
#include <assert.h>
#include <arrayWrap1.h>

#ifdef USING_MATHGUTZ
	#include <mathGutz.h>
#endif

namespace gutz {

// Forward Decl
template <class T> class arrayWrap3;

/////////////////////////
// arrayWrap2
/////////////////////////
template <class T>
class arrayWrap2 : public arrayBase<T>
{
public:
  friend class arrayWrap3<T>;
  
  // Constructors - Shallow wrap around data. No size checking!
 arrayWrap2() : arrayBase<T>() {}
 arrayWrap2( int d0, int d1, T* v=0 ) 
   :arrayBase<T>(d0,d1,v),
    mSlice(arrayWrap1<T>(d1,v)) 
    {}

 arrayWrap2( const arrayWrap2<T>& a ) : arrayBase<T>(a) {mSlice = a.mSlice;}
  arrayWrap2<T>& operator=( const arrayWrap2<T>& a );

  virtual ~arrayWrap2();
  
  // Return a slice of array
  inline arrayWrap1<T>&		operator[] (int i);
  inline const arrayWrap1<T>& operator[] (int i) const;
  
  // Return the element at data(i,j) - Faster than arr[i][j]
  inline T&					operator() (int i, int j);
  inline const T&				operator() (int i, int j) const;

protected:
		
  // The slice wrapper used in the [] operator. 
  // Mutable so that it can be changed by a const operator
  mutable arrayWrap1<T>  mSlice;	
	
private:
  inline void setSlice( int i ) const;				// Set 'mSlice' to be the ith slice of 'mData'
  inline int  address(  int i, int j ) const;			// Get the address of the data(i,j)'th element
  inline void setData( T* d ) { this->mData = d; }				// Reset just data pointer
};

////////////////////////////////////
// Implementation
////////////////////////////////////

// Constructors - All are shallow!!
//template <class T>
//arrayWrap2<T>::arrayWrap2 ()
//:arrayBase<T>(), mSlice(arrayWrap1<T>())
//{}


//template <class T>
//arrayWrap2<T>::arrayWrap2 (int d0, int d1, T* v)
//:arrayBase<T>(d0,d1,v), mSlice(arrayWrap1<T>(d1,v))
//{}

// Copy Ctor
//template <class T>
//arrayWrap2<T>::arrayWrap2<T>( const arrayWrap2<T>& a ) : arrayBase<T>(a)
//{
//  mSlice = a.mSlice;
//}

// Assignment Op
template <class T>
  arrayWrap2<T>& arrayWrap2<T>::operator=(const arrayWrap2<T>& a)
  {
    if( this != &a ) {
      arrayBase<T>::operator=(a);// Call base class assign. op
      mSlice = a.mSlice;
    }
    return *this;
}

// Destructor
template <class T>
arrayWrap2<T>::~arrayWrap2()
{}

// Accessing Operators
template <class T>
inline arrayWrap1<T>& arrayWrap2<T>::operator[] (int i) 				
{ 
  setSlice(i);
  return mSlice;
}

template <class T>
inline const arrayWrap1<T>& arrayWrap2<T>::operator[] (int i) const 	
{ 
  setSlice(i);
  return mSlice;
}

// Set 'mSlice' to be the ith slice of 'mData'
template <class T>
inline void arrayWrap2<T>::setSlice( int i ) const
{
  int a = i * this->mAxisStride[0]; 
  assert( a < this->mSize ); 
  mSlice.setData( this->mData + a );
}

template <class T>
inline T& arrayWrap2<T>::operator() (int i, int j)
{
  return this->mData[ address(i,j) ];
}

template <class T>
inline const T& arrayWrap2<T>::operator() (int i, int j) const
{
  return this->mData[ address(i,j) ];
}

template <class T>
inline int arrayWrap2<T>::address( int i, int j) const 
{
  return i*this->mAxisStride[0] + j;
}


////////////////////////////////////
// Typedefs
////////////////////////////////////

typedef arrayWrap2<char> 	arrayw2c;
typedef arrayWrap2<uchar> 	arrayw2uc;
typedef arrayWrap2<char> 	arrayw2b;
typedef arrayWrap2<uchar> 	arrayw2ub;
typedef arrayWrap2<short> 	arrayw2s;
typedef arrayWrap2<ushort> 	arrayw2us;
typedef arrayWrap2<int>	 	arrayw2i;
typedef arrayWrap2<uint> 	arrayw2ui;
typedef arrayWrap2<long> 	arrayw2l;
typedef arrayWrap2<ulong> 	arrayw2ul;
typedef arrayWrap2<llong> 	arrayw2ll;
typedef arrayWrap2<ullong> 	arrayw2ull;
typedef arrayWrap2<float> 	arrayw2f;
typedef arrayWrap2<double> 	arrayw2d;

#ifdef USING_MATHGUTZ
	typedef arrayWrap2<vec2ub> arrayw2v2ub;
	typedef arrayWrap2<vec2i>  arrayw2v2i;
	typedef arrayWrap2<vec2ui> arrayw2v2ui;
	typedef arrayWrap2<vec2f>  arrayw2v2f;

	typedef arrayWrap2<vec3ub> arrayw2v3ub;
	typedef arrayWrap2<vec3i>  arrayw2v3i;
	typedef arrayWrap2<vec3ui> arrayw2v3ui;
	typedef arrayWrap2<vec3f>  arrayw2v3f;

	typedef arrayWrap2<vec4ub> arrayw2v4ub;
	typedef arrayWrap2<vec4i>  arrayw2v4i;
	typedef arrayWrap2<vec4ui> arrayw2v4ui;
	typedef arrayWrap2<vec4f>  arrayw2v4f;

	typedef arrayWrap2<mat2ub> arrayw2m2ub;
	typedef arrayWrap2<mat2i>  arrayw2m2i;
	typedef arrayWrap2<mat2ui> arrayw2m2ui;
	typedef arrayWrap2<mat2f>  arrayw2m2f;

	typedef arrayWrap2<mat3ub> arrayw2m3ub;
	typedef arrayWrap2<mat3i>  arrayw2m3i;
	typedef arrayWrap2<mat3ui> arrayw2m3ui;
	typedef arrayWrap2<mat3f>  arrayw2m3f;

	typedef arrayWrap2<mat4ub> arrayw2m4ub;
	typedef arrayWrap2<mat4i>  arrayw2m4i;
	typedef arrayWrap2<mat4ui> arrayw2m4ui;
	typedef arrayWrap2<mat4f>  arrayw2m4f;
#endif

} // End of namespace gutz

#endif // arrayWrap2_h
