/*
* $Id: vec.h,v 1.6 2007/03/05 01:19:02 jmk Exp $
*/
///////////////////////////////////////////////////////////////////////////
//              _____________  ______________________   
//             /  ________  |  |   ___   ________   /   
//            |  |       |  |_ |  |_ |  |       /  /    
//            |  |  ___  |  || |  || |  |      /  /     
//            |  | |   \ |  || |  || |  |     /  /      
//            |  | |_@  ||  || |  || |  |    /  /       
//            |  |___/  ||  ||_|  || |  |   /  /_____________________
//             \_______/  \______/ | |__|  /___________________________
//                        |  |__|  |
//                         \______/
///////////////////////////////////////////////////////////////////////////

// vec.h


#ifndef GUTZ_VEC_BASE_H
#define GUTZ_VEC_BASE_H

#include <assert.h>
#include <iostream>
#include <sstream>
#include <math.h>

namespace gutz {

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/// a vector templated in type and dimension
/// a generic vector, with constructors up to length 10
/// bridges the gap between vectors and arrays, very similar to 
/// an array1 but, supports smaller 1D arrays that are used like
/// N Dimensional vectors.
///     A vector is truely just an array of data.  This means you can 
///     do stuff like this:
/// \code
///   float vdata[3] = {1.f, 0.f, 1.f};
///   vec<float,3> &v = *reinterpret_cast<vec<float,3>*>(vdata);
///   cerr << "the vector is: " << v << endl;
///   // should print -- the vector is: 1.0 0.0 1.0
/// \endcode
///     This bit of code demonstrates that you can "cast" a float array
///     to a vec type.  This is handy if you want to turn some data 
///     into a vec temporarily, i.e take advantage of the operators etc...
///
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
   template<int D> class plusminus {public: enum PM { PLUS = D+1, MINUS = D-1 }; };
   template<> class plusminus<1> {public: enum PM { PLUS = 2, MINUS = 2 }; };

template<class T, int DIM>
class vec {
public:

   typedef T value_type; 
   // Accessible at runtime and compile time. Useful for template metaprogramming.
   static const int size = DIM;

#if 0
   // jbowles, 10/24/10, the following has been superceded by the static
   // size variable.
   //
   // jbowles, 8/8/11, perhaps an enum is better, this is what is done
   // in the Loki library
   enum THE_DIM {
      DIM   = V_DIM
   };
#endif

   ///////////////////////////////////////
   /// Public data Member
   T v[DIM];
   ///////////////////////////////////////

   vec() {}
   ~vec() {}
   vec(const vec &vee);
   vec(const vec<T,plusminus<DIM>::PLUS> &vee)
   {
         for(int i=0; i<DIM; ++i) this->v[i] = vee.v[i];
   }
   vec(const vec<T,plusminus<DIM>::MINUS> &vee, T last = 0)
   {
         for(int i=0; i<DIM-1; ++i) this->v[i] = vee.v[i];
         this->v[DIM-1] = last;
   }
   /// the last value in a constructor gets replicated to the
   /// end of the vec if there are more values in the vector
   /// than specified by the constructor.
   vec(T v1);
   vec(T v1, T v2); 
   vec(T v1, T v2, T v3);
   vec(T v1, T v2, T v3, T v4);
   vec(T v1, T v2, T v3, T v4, T v5);
   vec(T v1, T v2, T v3, T v4, T v5, T v6);
   vec(T v1, T v2, T v3, T v4, T v5, T v6, T v7);
   vec(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8);
   vec(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9);
   vec(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9, T v10);
   /// copy data array
   /// when numVals == -1 you indicate that the "vee" array
   /// is at least as big as the dimension of this vec
   vec(T *vee, int numVals=-1);

   //virtual ~vec() {}

   /// element accessor
   T &operator[](int i)            { return v[i]; }
   const T operator[](int i) const { return v[i]; }

   // Commented out by jbowles. When this information is presented
   // in function format, it is only available at runtime. It is useful
   // to have this information available both at runtime and compile time
   // for template metaprogramming.
   //
   // These functions have been replaced with "static const int size" which
   // is available both at runtime and compile time.
   //
   // Access it with gutz::vec<T,DIM>::size or gutz::vec3f::size.
   // The same semantics apply for both runtime and compile time
#if 0
   //// these are all the same, preference/symantics really :)
   int len()  { return DIM; }
   int dim()  { return DIM; }
   int size() { return DIM; }
#endif

   /// assignment =
   vec &operator=(const vec &v)
   { for(int i=0; i<DIM; ++i) this->v[i] = v.v[i]; return *this;}
   vec &operator=(const vec<T,DIM+1> &v)
   { for(int i=0; i<DIM; ++i) this->v[i] = v.v[i]; return *this;}
   //vec &operator=(const T& val)
   //{ for(int i=0; i<DIM; ++i) this->v[i] = val; return *this;}
   /// +=
   vec &operator+=(const T& v)
   { for(int i=0; i<DIM; ++i) this->v[i] += v; return *this;}
   vec &operator+=(const vec &v)
   { for(int i=0; i<DIM; ++i) this->v[i] += v.v[i]; return *this;}
   /// -=
   vec &operator-=(const T& v)
   { for(int i=0; i<DIM; ++i) this->v[i] -= v; return *this;}
   vec &operator-=(const vec &v)
   { for(int i=0; i<DIM; ++i) this->v[i] -= v.v[i]; return *this;}
   /// *=
   vec &operator*=(const T& v)
   { for(int i=0; i<DIM; ++i) this->v[i] *= v; return *this;}
   vec &operator*=(const vec &v)
   { for(int i=0; i<DIM; ++i) this->v[i] *= v.v[i]; return *this;}
   /// /=
   vec &operator/=(const T& v)
   { for(int i=0; i<DIM; ++i) this->v[i] /= v; return *this;}
   vec &operator/=(const vec &v)
   { for(int i=0; i<DIM; ++i) this->v[i] /= v.v[i]; return *this;}

   /// negate -
   vec operator-() const
   {
      vec vee;
      for(int i=0; i<DIM; ++i) vee[i] = - (this->v[i]);
      return vee;
   }
   /// subtract
   vec operator-(const T& v) const
   {
      vec vee;
      for(int i=0; i<DIM; ++i) vee[i] = this->v[i] - v;
      return vee;
   }
   vec operator-(const vec &v) const
   {
      vec vee;
      for(int i=0; i<DIM; ++i) vee[i] = this->v[i] - v.v[i];
      return vee;
   }
   /// add
   vec operator+(const T& v) const
   {
      vec vee;
      for(int i=0; i<DIM; ++i) vee[i] = this->v[i] + v;
      return vee;
   }
   vec operator+(const vec &v) const
   {
      vec vee;
      for(int i=0; i<DIM; ++i) vee[i] = this->v[i] + v.v[i];
      return vee;
   }
   /// multiply
   vec operator*(const T& v) const
   {
      vec vee;
      for(int i=0; i<DIM; ++i) vee[i] = this->v[i] * v;
      return vee;
   }
   
   vec operator*(const vec &v) const
   {
      vec vee;
      for(int i=0; i<DIM; ++i) vee[i] = this->v[i] * v.v[i];
      return vee;
   }

   /// divide
   vec operator/(const T& v) const
   {
      vec vee;
      for(int i=0; i<DIM; ++i) vee[i] = this->v[i] / v;
      return vee;
   }
   vec operator/(const vec &v) const
   {
      vec vee;
      for(int i=0; i<DIM; ++i) vee[i] = this->v[i] / v.v[i];
      return vee;
   }

   /// comparison ops ===================
   /// equal
   bool operator==(const vec &v) const
   {
      for(int i=0; i<DIM; ++i)
         if(this->v[i] != v[i]) return false;
      return true;
   }
   bool operator==(T val) const
   {
      for(int i=0; i<DIM; ++i)
         if(this->v[i] != val) return false;
      return true;
   }
   /// not equal
   bool operator!=(const vec &v) const
   { return !(operator==(v)); }
   /// less
   bool operator< (const vec &v) const
   {	
      for(int i=0; i<DIM; ++i)
         if(this->v[i] >= v.v[i]) return false;
      return true;
   }
   /// less equal
   bool operator<=(const vec &v) const
   {
      for(int i=0; i<DIM; ++i)
         if(this->v[i] > v.v[i]) return false;
      return true;
   }
   /// greater
   bool operator> (const vec &v) const
   { 
      for(int i=0; i<DIM; ++i)
          if(this->v[i] <= v.v[i]) return false;
      return true;
   }
   /// greater equal
   bool operator>=(const vec &v) const
   {
       for(int i=0; i<DIM; ++i)
           if(this->v[i] < v.v[i]) return false;
       return true;
   }

   /// other ops ===========================
   /// L1 norm
   T normL1() const
   {
      T tmp = 0;
      for(int i=0; i<DIM; ++i)
         tmp += this->v[i];
      return tmp;
   }
   /// L2 norm squared
   T norm2() const
   {
      T tmp=0;
      for(int i=0; i<DIM; ++i)
         tmp += this->v[i] * this->v[i];
      return tmp;
   }
   /// L2 norm
   T norm() const 
   {
      return T(sqrt(norm2()));
   }
   /// Normalize, side-effect op, normalizes this vector!
   T normalize()
   {
      T tmp = norm();
      (*this) *= T(1.0)/tmp;
      return tmp;
   }

   /// dot product
   T dot(const vec &v) const
   {
     T tmp=0;
     for(int i=0; i<DIM; ++i)
     {
       tmp += (*this)[i]*v[i];
     }
     return tmp;
     //return ((*this) * v).normL1;
   }
   /// abs of all elements
   vec abs() const
   {
      vec tmp;
      for(int i=0; i<DIM; ++i)
         tmp[i] = g_abs(this->v[i]);
      return tmp;
   }

   /// some old-school accessor for convenience
  inline T &x() { return operator[](0); }
  inline T &y() { return operator[](1); }
  inline T &z() { return operator[](2); }
  inline T &w() { return operator[](3); }
  inline const T x() const { return operator[](0); }
  inline const T y() const { return operator[](1); }
  inline const T z() const { return operator[](2); }
  inline const T w() const { return operator[](3); }

   /// do not copy data, just pointer (always shallow copy)
   /// the "vee" array MUST be at least as long as this array
   /// and created using NEW.  The array will be deleted with
   /// this class. Not public cuz it's a little confusing.
   //vec(T *vee, bool noCpy);

protected:
   
};


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/// typedefs
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/// there are specific classes for vectors 2-4

typedef vec<float, 2>  vec2f;
typedef vec<float, 3>  vec3f;
typedef vec<float, 4>  vec4f; 
typedef vec<float, 5>  vec5f;
typedef vec<float, 6>  vec6f;
typedef vec<float, 7>  vec7f;
typedef vec<float, 8>  vec8f;
typedef vec<float, 9>  vec9f;
typedef vec<float, 10> vec10f;

typedef vec<double, 2>  vec2d;
typedef vec<double, 3>  vec3d;
typedef vec<double, 4>  vec4d;
typedef vec<double, 5>  vec5d;
typedef vec<double, 6>  vec6d;
typedef vec<double, 7>  vec7d;
typedef vec<double, 8>  vec8d;
typedef vec<double, 9>  vec9d;
typedef vec<double, 10> vec10d;

typedef vec<int, 2>  vec2i;
typedef vec<int, 3>  vec3i;
typedef vec<int, 4>  vec4i; 
typedef vec<int, 5>  vec5i;
typedef vec<int, 6>  vec6i;
typedef vec<int, 7>  vec7i;
typedef vec<int, 8>  vec8i;
typedef vec<int, 9>  vec9i;
typedef vec<int, 10> vec10i;

typedef vec<unsigned int, 2>  vec2ui;
typedef vec<unsigned int, 3>  vec3ui;
typedef vec<unsigned int, 4>  vec4ui; 
typedef vec<unsigned int, 5>  vec5ui;
typedef vec<unsigned int, 6>  vec6ui;
typedef vec<unsigned int, 7>  vec7ui;
typedef vec<unsigned int, 8>  vec8ui;
typedef vec<unsigned int, 9>  vec9ui;
typedef vec<unsigned int, 10> vec10ui;

typedef vec<short, 2>  vec2s;
typedef vec<short, 3>  vec3s;
typedef vec<short, 4>  vec4s; 
typedef vec<short, 5>  vec5s;
typedef vec<short, 6>  vec6s;
typedef vec<short, 7>  vec7s;
typedef vec<short, 8>  vec8s;
typedef vec<short, 9>  vec9s;
typedef vec<short, 10> vec10s;

typedef vec<unsigned short, 2>  vec2us;
typedef vec<unsigned short, 3>  vec3us;
typedef vec<unsigned short, 4>  vec4us; 
typedef vec<unsigned short, 5>  vec5us;
typedef vec<unsigned short, 6>  vec6us;
typedef vec<unsigned short, 7>  vec7us;
typedef vec<unsigned short, 8>  vec8us;
typedef vec<unsigned short, 9>  vec9us;
typedef vec<unsigned short, 10> vec10us;

typedef vec<char, 2>  vec2b;
typedef vec<char, 3>  vec3b;
typedef vec<char, 4>  vec4b; 

typedef vec<unsigned char, 2>  vec2ub;
typedef vec<unsigned char, 3>  vec3ub;
typedef vec<unsigned char, 4>  vec4ub; 


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/// auxillary ops
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

///////////////////////////////////////
/// String stuff

template<class T, int D>
bool str2vec(const std::string &s, vec<T,D> &v)
{
   std::istringstream z(s);
   vec<T,D> w;
   bool status = z.eof();
   for(int i=0; i<D; ++i)
   {  
      z >> w[i];
      status = status || z.fail() || z.eof();
   }
   if(!status) v = w;
   return status;
}

template<class T>
bool str2vec2(const std::string &s, vec<T,2> v)
{
   return str2vec<T,2>(s,v);
}

template<class T>
bool str2vec3(const std::string &s, vec<T,3> v)
{
   return str2vec<T,3>(s,v);
}

template<class T>
bool str2vec4(const std::string &s, vec<T,4> v)
{
   return str2vec<T,4>(s,v);
}

///////////////////////////////////////
/// side-effect min (no temporary created) alters the first vec
/// in parameter list to be the min (for each element) of the 
/// two vectors
template<class T, int D> inline
vec<T,D> vs_min(vec<T,D> &v1, const vec<T,D> &v2)
{
   for(int i=0; i<D; ++i)
   {
      v1[i] = v1[i] < v2[i] ? v1[i] : v2[i];
   }
   return v1;
}

///////////////////////////////////////
/// side-effect max (no temporary created) alters the first vec
/// in parameter list to be the max (for each element) of the 
/// two vectors
template<class T, int D> inline
vec<T,D> vs_max(vec<T,D> &v1, const vec<T,D> &v2)
{
   for(int i=0; i<D; ++i)
   {
      v1[i] = v1[i] > v2[i] ? v1[i] : v2[i];
   }
   return v1;
}

///////////////////////////////////////
/// <<
template<class T, int D>
std::ostream&
operator<<(std::ostream &os, const vec<T,D> &v)
{
   for(int i=0; i<D; ++i)
      os << v[i] << " ";
   return os;
}
/// >>
template<class T, int D>
std::istream&
operator>>(std::istream &is, vec<T,D> &v)
{
   for(int i=0; i<D; ++i)
      is >> v[i];
   return is;
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/// implementation of vec_base
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
/// more operators
template<class T, int D>
vec<T,D> operator+(const T &val, const vec<T,D> &v)
{
   vec<T,D> vee;
   for(int i=0; i<D; ++i) vee[i] = val + v.v[i];
   return vee;
}

template<class T, int D>
vec<T,D> operator-(const T &val, const vec<T,D> &v)
{
   vec<T,D> vee;
   for(int i=0; i<D; ++i) vee[i] = val - v.v[i];
   return vee;
}

template<class T, int D>
vec<T,D> operator*(const T &val, const vec<T,D> &v)
{
   vec<T,D> vee;
   for(int i=0; i<D; ++i) vee[i] = val * v.v[i];
   return vee;
}

template<class T,  int D>
vec<T,D> operator/(const T &val, const vec<T,D> &v)
{
   vec<T,D> vee;
   for(int i=0; i<D; ++i) vee[i] = val / v.v[i];
   return vee;
}

template<class T>
vec<T,3> cross(const vec<T,3> &a, const vec<T,3> &b)
{
   return vec<T,3>(a[1]*b[2] - a[2]*b[1], 
                   a[2]*b[0] - a[0]*b[2], 
                   a[0]*b[1] - a[1]*b[0]);
}

/////////////////////////////////////////////////////////////////////////
/// constructors 

template<class T, int D>
vec<T,D>::vec(const vec<T,D> &vee)
{
   for(int i=0; i<D; ++i)
   {
      this->v[i] = vee.v[i];
   }
}

template<class T, int D>
vec<T,D>::vec(T *vee, int numVals)
{
   if(numVals == -1)
   {
      for(int i=0; i<D; ++i)
      {
         this->v[i] = vee[i];
      }
   }
   else
   {
      for(int i=0; i< (D<numVals?D:numVals); ++i)
      {
         this->v[i] = vee[i];
      }
   }
}

#if 0
template<class T, int D>
vec<T,D>::vec(T *vee, bool noCpy)
: v(0)
{
   if(noCpy) v = vee;
   else
   {
      v = new T[D];
      for(int i=0; i<D; ++i) v[i] = vee[i];
   }
}
#endif

template<class T, int D>
vec<T,D>::vec(T v1)
{
   assert(D >= 1);
   for(int i=0; i<D; ++i)
      v[i] = v1;
}

template<class T, int D>
vec<T,D>::vec(T v1, T v2)
{
   assert(D >= 2);
   v[0] = v1; v[1] = v2;
   for(int i=2; i<D; ++i)
      v[i] = v2;
}

template<class T, int D>
vec<T,D>::vec(T v1, T v2, T v3)
{
   assert(D >= 3);
   v[0] = v1; v[1] = v2; v[2] = v3;
   for(int i=3; i<D; ++i)
      v[i] = v3;
}

template<class T, int D>
vec<T,D>::vec(T v1, T v2, T v3, T v4)
{
   assert(D >= 4);
   v[0] = v1; v[1] = v2; v[2] = v3; v[3] = v4;
   for(int i=4; i<D; ++i)
      v[i] = v4;
}

template<class T, int D>
vec<T,D>::vec(T v1, T v2, T v3, T v4, T v5)
{
   assert(D >= 5);
   v[0] = v1; v[1] = v2; v[2] = v3; v[3] = v4; v[4] = v5;
   for(int i=5; i<D; ++i)
      v[i] = v5;
}

template<class T, int D>
vec<T,D>::vec(T v1, T v2, T v3, T v4, T v5, T v6)
{
   assert(D >= 6);
   v[0] = v1; v[1] = v2; v[2] = v3; v[3] = v4; v[4] = v5; v[5] = v6;
   for(int i=6; i<D; ++i)
      v[i] = v6;
}

template<class T, int D>
vec<T,D>::vec(T v1, T v2, T v3, T v4, T v5, T v6, T v7)
{
   assert(D >= 7);
   v[0] = v1; v[1] = v2; v[2] = v3; v[3] = v4; v[4] = v5; v[5] = v6;
   v[6] = v7;
   for(int i=7; i<D; ++i)
      v[i] = v7;
}

template<class T, int D>
vec<T,D>::vec(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8)
{
   assert(D >= 8);
   v[0] = v1; v[1] = v2; v[2] = v3; v[3] = v4; v[4] = v5; v[5] = v6;
   v[6] = v7; v[7] = v8;
   for(int i=8; i<D; ++i)
      v[i] = v8;
}

template<class T, int D>
vec<T,D>::vec(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9)
{
   assert(D >= 9);
   v[0] = v1; v[1] = v2; v[2] = v3; v[3] = v4; v[4] = v5; v[5] = v6;
   v[6] = v7; v[7] = v8; v[8] = v9;
   for(int i=9; i<D; ++i)
      v[i] = v9;
}

template<class T, int D>
vec<T,D>::vec(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9, T v10)
{
   assert(D >= 10);
   v[0] = v1; v[1] = v2; v[2] = v3; v[3] = v4; v[4] = v5; v[5] = v6;
   v[6] = v7; v[7] = v8; v[8] = v9; v[9] = v[10];
   for(int i=10; i<D; ++i)
      v[i] = v10;
}

template<class T>
vec<T,3> perspective_divide(const vec<T,4>& v)
{
   vec<T,4> pd = v / v.w();
   return vec<T,3>(pd.x(), pd.y(), pd.z());
}
   
}//end namespace gutz


#endif


