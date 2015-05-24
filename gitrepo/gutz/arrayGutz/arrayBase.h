/*
 * $Id: arrayBase.h,v 1.2 2006/03/07 06:13:26 kpotter Exp $
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
// 9/8/02	Joe M. Knis	    Scientific Computing and Imaging Institute
//							School of Computing
//							University of Utah
//
//         arrayBase.h This is a base class for all arrays in gutz
//         - it knows its 
//                * dimension
//                * size of each axis
//                * the data pointer
//                * total size of the array
////////////////////////////////////////////////////////////////////////


#ifndef ARRAY_BASE_h
#define ARRAY_BASE_h

#include <assert.h>
#include <mm.h>

#ifdef USING_MATHGUTZ
	#include <mathGutz.h>
#endif

namespace gutz
{

#define MAX_ARRAY_DIM_GUTZ 5

//////////////////////////////
// arrayBase
//////////////////////////////
template <class T>
class arrayBase
{
public:
	arrayBase();
	//assignment constructors implicitly set the dimension
	arrayBase(int d0, T* a);
	arrayBase(int d0, int d1, T* a);
	arrayBase(int d0, int d1, int d2, T* a);
	arrayBase(int d0, int d1, int d2, int d3, T* a);
	arrayBase(int d0, int d1, int d2, int d3, int d4, T* a);
	//This assignment constructor explicitly sets the dimension
	arrayBase(int dim, int* aSize, T* a);

	//copy constructor, creates an identical array
	arrayBase( const arrayBase<T> &a);

	// 10/10/02 Added by Aaron Lefohn
	// - I think that operator= should have the same 
	//   functionality as the copy ctor. If other, non-trad
	//   assignments are required, they should exist via
	//   a member function such as "transfer" or "wrap"
	// (OLD comment: "assignment operator, assumes data is WRAPPED by class,
	//				  only sets the data pointer and total size, YOU specify the 
	//				  dimension and axis sizes when you construct the class")
	arrayBase<T>& operator=( const arrayBase<T>& a );

    // Return the element at data(i,j,k,m,n)
	inline T&					operator() (int i, int j=0, int k=0, int m=0, int n=0);
	inline const T&				operator() (int i, int j=0, int k=0, int m=0, int n=0) const;

	// Return the dimension of the array
	inline int    dim() const {return mDim;}
	// Return the size of an axis
	inline int    dim(const int axis) const {return mAxisDim[axis];}

	// Total elements in array
	inline int    size() const {return mSize;}

	// Is the array empty
	inline int    empty() const {return mData==NULL || mSize==0;}

	// Get the data
	inline T*		data()		 { return mData; }
	inline const T*	data() const { return mData; }

	// Should the data be deleted with 'delete' or 'malloc'?
	// - NOTE: Data only deleted by 'arrayOwnN<T>' classes, not by arrayWrapN<T>
	inline bool		killWithDelete() const {return mKillWithDelete;}

	// Reshape the axes sizes, does not change dimension of array, does not reallocate data
	inline void	reshape( int d0, int d1 = 0, int d2 = 0, int d3 = 0, int d4 = 0 );


protected:
	T*   mData;                              ///< data pointer

	int  mAxisDim[MAX_ARRAY_DIM_GUTZ];       ///< # of elements allong each axis
	int  mAxisStride[MAX_ARRAY_DIM_GUTZ];    ///< # of elements to jump for the next entry
	int  mSize;                              ///< total # of elements in array
	int  mDim;                               ///< # of axes in array
	bool mKillWithDelete;					 ///< Kill data with delete or free?

	inline void set( int dim, int* aSize, T* a);     ///< # of axies, # of elements per axis, data pointer
	//inline int  address(int i, int j, int k, int m, int n); ///< address of array element	
	inline const int  address(int i, int j, int k, int m, int n) const; ///< address of array element
	
	// ***************************************
	// ****   arrayOwn routines
	// ***************************************
	// Init memory from a single value
	// Preconditions: 1) mData MUST already be [] deleted if necessary
	//				  2) mSize MUST be set
	inline void initValOwn(T val);
	inline void initDataOwn(const T* data); // Calls allocCopyOwn and copyDataOwn
	inline void copyDataOwn(const T* data); // Copies data: does NOT allocate memory
	inline void allocDataOwn();				// Allocates data, does NOT copy memory
	inline void killData(); // Called by arrayOwnN<T> destructors
};

//////////////////////////////
// Implementation
//////////////////////////////

/// Default constructor
template<class T>
arrayBase<T>::arrayBase()
:mData(0),mSize(0),mDim(0),mKillWithDelete(true)
{
}

template<class T>
arrayBase<T>::arrayBase(int d0, T* a)
{
	int sizes[1];
	sizes[0] = d0;
	set(1, sizes, a);
}

template<class T>
arrayBase<T>::arrayBase(int d0, int d1, T* a)
{
	int sizes[2];
	sizes[0] = d0;
	sizes[1] = d1;
	set(2, sizes, a);
}

template<class T>
arrayBase<T>::arrayBase(int d0, int d1, int d2, T* a)
{
	int sizes[3];
	sizes[0] = d0;
	sizes[1] = d1;
	sizes[2] = d2;
	set(3, sizes, a);
}

template<class T>
arrayBase<T>::arrayBase(int d0, int d1, int d2, int d3, T* a)
{
	int sizes[4];
	sizes[0] = d0;
	sizes[1] = d1;
	sizes[2] = d2;
	sizes[3] = d3;
	set(4, sizes, a);
}

template<class T>
arrayBase<T>::arrayBase(int d0, int d1, int d2, int d3, int d4, T* a)
{
	int sizes[5];
	sizes[0] = d0;
	sizes[1] = d1;
	sizes[2] = d2;
	sizes[3] = d3;
	sizes[4] = d3;
	set(5, sizes, a);
}

template<class T>
arrayBase<T>::arrayBase(int dim, int *aSize, T* a)
{
	set(dim, aSize, a);
}

//copy constructor, creates an identical array
// SHALLOW COPY!
template<class T>
arrayBase<T>::arrayBase( const arrayBase<T> &a)
{
	assert( MAX_ARRAY_DIM_GUTZ == 5 );
	int sizes[MAX_ARRAY_DIM_GUTZ];
	sizes[0] = a.mAxisDim[0];
	sizes[1] = a.mAxisDim[1];
	sizes[2] = a.mAxisDim[2];
	sizes[3] = a.mAxisDim[3];
	sizes[4] = a.mAxisDim[4];

	set(a.mDim, sizes, a.mData);

/*	mDim  = a.mDim;
	mSize = a.mSize;
	mData = a.mData;

	mAxisDim[0] = a.mAxisDim[0];
	mAxisDim[1] = a.mAxisDim[1];
	mAxisDim[2] = a.mAxisDim[2];
	mAxisDim[3] = a.mAxisDim[3];
	mAxisDim[4] = a.mAxisDim[4];

	mAxisStride[0] = a.mAxisStride[0];
	mAxisStride[1] = a.mAxisStride[1];
	mAxisStride[2] = a.mAxisStride[2];
	mAxisStride[3] = a.mAxisStride[3];
	mAxisStride[4] = a.mAxisStride[4];
*/
}

//assignment operator
// SHALLOW COPY
// 10/10/02 Added by Aaron Lefohn
// - I think that operator= should have the same 
//   functionality as the copy ctor. If other, non-trad
//   assignments are required, they should exist via
//   a member function such as "transfer" or "wrap"
template<class T>
arrayBase<T>& arrayBase<T>::operator=( const arrayBase<T>& a )
{
	if( this != &a ) {
		assert( MAX_ARRAY_DIM_GUTZ == 5 );
		int sizes[MAX_ARRAY_DIM_GUTZ];
		sizes[0] = a.mAxisDim[0];
		sizes[1] = a.mAxisDim[1];
		sizes[2] = a.mAxisDim[2];
		sizes[3] = a.mAxisDim[3];
		sizes[4] = a.mAxisDim[4];

		set(a.mDim, sizes, a.mData);

	/*	mDim  = a.mDim;
		mSize = a.size();
		mData = (T*)a.data();

		mAxisDim[0] = a.mAxisDim[0];
		mAxisDim[1] = a.mAxisDim[1];
		mAxisDim[2] = a.mAxisDim[2];
		mAxisDim[3] = a.mAxisDim[3];
		mAxisDim[4] = a.mAxisDim[4];

		mAxisStride[0] = a.mAxisStride[0];
		mAxisStride[1] = a.mAxisStride[1];
		mAxisStride[2] = a.mAxisStride[2];
		mAxisStride[3] = a.mAxisStride[3];
		mAxisStride[4] = a.mAxisStride[4];
	*/
	}
	return *this;
}

// Return the element at data(i,j,k,m)
template<class T>
T& arrayBase<T>::operator() (int i, int j, int k, int m, int n)
{
	return mData[address(i,j,k,m,n)];
}

template<class T>
const T& arrayBase<T>::operator() (int i, int j, int k, int m, int n) const
{
	return mData[address(i,j,k,m,n)];
}

template<class T>
const int arrayBase<T>::address(int i, int j, int k, int m, int n) const///< address of array element	
{
	assert(mSize < (i*mAxisStride[0] + j*mAxisStride[1] + k*mAxisStride[2] +
 					m*mAxisStride[3] + n*mAxisStride[4]));
	return i*mAxisStride[0] + j*mAxisStride[1] + k*mAxisStride[2] + m*mAxisStride[3] + n*mAxisStride[4];
}

template<class T>
void arrayBase<T>::set( int dim, int* aSize, T* a)
{
	assert(dim >= 0 );
	assert(dim <= MAX_ARRAY_DIM_GUTZ);
	
	//Zero out everything
	for(int s=0; s<MAX_ARRAY_DIM_GUTZ; ++s)
	{
		mAxisStride[s] = 0;
		mAxisDim[s] = 0;
	}

	//Set the member variables
	mDim = dim;
	int size = dim > 0 ? 1 : 0;  
	for(int i=0; i<dim; ++i)
	{
		//Axis size & total size
		mAxisDim[i] = aSize[i];
		//if(i==0) size = aSize[i];
		//else 
		size *= aSize[i];
		
		//Strides
		mAxisStride[i] = 1;
		for(int j=i+1; j<dim; ++j)
		{
			mAxisStride[i] *= aSize[j];
			assert(aSize[j] > 0);
		}
	}
	//Set the total size and array pointer
	mSize = size;
	mData = a;
	mKillWithDelete = true;
}

template<class T>
void arrayBase<T>::reshape( int d0, int d1, int d2, int d3, int d4)
{
	//Set the new dimensions
	mAxisDim[0] = d0;
	mAxisDim[1] = d1;
	mAxisDim[2] = d2;
	mAxisDim[3] = d3;
	mAxisDim[4] = d4;

	//Set the strides
	for(int i=0; i<mDim; ++i)
	{		
		mAxisStride[i] = 1;
		for(int j=i+1; j<mDim; ++j)
		{
			mAxisStride[i] *= mAxisDim[j];
		}
	}
}

// ***************************************
// ****   arrayOwn routines
// ***************************************
// Init memory from a single value
// Preconditions: 1) mData MUST already be [] deleted if necessary
//				  2) mSize MUST be set
template <class T>
void arrayBase<T>::initValOwn(T val)
{
	if( mSize > 0 ) {
		mData = new T[mSize];
		assert(mData);
		for (int i=0; i<mSize; i++) { mData[i] = val; }	
	}
}

// Init memory from an array
// Init memory from a single value
// Preconditions: 1) mData MUST already be [] deleted if necessary
//				  2) mSize MUST be set
template <class T>
void arrayBase<T>::initDataOwn(const T* data)
{	
	// Do DEEP copy of 'data'
	/*if( mSize > 0 ) {
	    mData = new T[mSize];
		assert(mData);
		for (int i=0; i<mSize; i++) { mData[i] = data[i]; }	
	}*/
	allocDataOwn();
	copyDataOwn(data);
}

// ****************************
//TODO: Remove THIS!!!!
//#include <iostream>
//using namespace std;
// ****************************

template <class T>
void arrayBase<T>::allocDataOwn()
{
	//cerr << "arrayBase<T>::allocDataOwn() Called\n";
	if( mSize > 0 ) {
		mData = new T[mSize];
		assert(mData);
	}
}

template <class T>
void arrayBase<T>::copyDataOwn(const T* data)
{
	if( mSize > 0 && mData) {
		for (int i=0; i<mSize; i++) { mData[i] = data[i]; }	
	}
}

// Delete old mem
template <class T>
void arrayBase<T>::killData()
{
	//cerr << "arrayBase<T>::killData() Called\n";
	if( mData ) {						
		if( mKillWithDelete ) {
			delete [] mData;
		}
		else {
			free(mData);
		}
		mData = NULL;
	}
}


////////////////////////////////////
// Typedefs
////////////////////////////////////

typedef arrayBase<char> 	arraybc;
typedef arrayBase<uchar> 	arraybuc;
typedef arrayBase<char> 	arraybb;
typedef arrayBase<uchar> 	arraybub;
typedef arrayBase<short> 	arraybs;
typedef arrayBase<ushort> 	arraybus;
typedef arrayBase<int>	 	arraybi;
typedef arrayBase<uint> 	arraybui;
typedef arrayBase<long> 	arraybl;
typedef arrayBase<ulong> 	arraybul;
typedef arrayBase<llong> 	arraybll;
typedef arrayBase<ullong> 	arraybull;
typedef arrayBase<float> 	arraybf;
typedef arrayBase<double> 	arraybd;

#ifdef USING_MATHGUTZ
	typedef arrayBase<vec2ub> arraybv2ub;
	typedef arrayBase<vec2i>  arraybv2i;
	typedef arrayBase<vec2ui> arraybv2ui;
	typedef arrayBase<vec2f>  arraybv2f;

	typedef arrayBase<vec3ub> arraybv3ub;
	typedef arrayBase<vec3i>  arraybv3i;
	typedef arrayBase<vec3ui> arraybv3ui;
	typedef arrayBase<vec3f>  arraybv3f;

	typedef arrayBase<vec4ub> arraybv4ub;
	typedef arrayBase<vec4i>  arraybv4i;
	typedef arrayBase<vec4ui> arraybv4ui;
	typedef arrayBase<vec4f>  arraybv4f;

	typedef arrayBase<mat2ub> arraybm2ub;
	typedef arrayBase<mat2i>  arraybm2i;
	typedef arrayBase<mat2ui> arraybm2ui;
	typedef arrayBase<mat2f>  arraybm2f;

	typedef arrayBase<mat3ub> arraybm3ub;
	typedef arrayBase<mat3i>  arraybm3i;
	typedef arrayBase<mat3ui> arraybm3ui;
	typedef arrayBase<mat3f>  arraybm3f;

	typedef arrayBase<mat4ub> arraybm4ub;
	typedef arrayBase<mat4i>  arraybm4i;
	typedef arrayBase<mat4ui> arraybm4ui;
	typedef arrayBase<mat4f>  arraybm4f;
#endif

}//end namespace gutz
#endif

