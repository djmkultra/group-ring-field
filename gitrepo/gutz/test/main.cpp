///////////////////////////////////////////////////////////////
/// bary2D
/// main.cpp
///   by Joe Kniss
///////////////////////////////////////////////////////////////

#include <iostream>
#include <mathGutz.h>
#include <arrayGutz.h>
#include <eventGutz.h>
#include <smartptr.h>
#include <signalGutz.h>


using namespace std;
using namespace gutz;

template<class T, int D> bool testVec();
template<class T>        bool testMat();

bool testMath();

int main(int argc, char **argv)
{
   if( testMath() )
   {
      cerr << "Gutz Math Test failed" << endl;
   }
   
}

/////////////////////////////////////////////////////////////////////////////
/// test all math
/////////////////////////////////////////////////////////////////////////////
bool testMath()
{
   cerr << "********** Testing MathGutz ************" << endl;
   bool err = false;
   err |= testVec<float,2>();
   err |= testVec<float,3>();
   err |= testVec<float,4>();
   err |= testVec<float,5>();
   err |= testVec<float,6>();
   err |= testVec<float,7>();

   err |= testMat<float>();
   err |= testMat<double>();


   return err;
}

/////////////////////////////////////////////////////////////////////////////
/// test vector math
/////////////////////////////////////////////////////////////////////////////
template<class T, int D>
bool testVec()
{
   cerr << "  - vec test, dim = " << D << " type = " << typeid(T).name();

   bool err = false;

   vec<T,D> v;
   vec<T,D> v0(T(0));
   vec<T,D> v2(T(1));
   vec<T,D> v3(T(2));
   vec<T,D> v4(v0);

   v4 = v3;
   if( (err |= !( v3 == v4 ) ))
   {
      cerr << " : `==` failed " << endl;
      cerr << v3 << " != " << v4 << endl;
      return err;
   }
   if( (err |= !( v0 < v2 ) ) )
   {
      cerr << " : `<` failed " << endl;
      cerr << v0 << " !< " << v2 << endl;
      return err;
   }
   if( ( err |= (v0 > v2) ) )
   {
      cerr << " : `>` failed " << endl;
      cerr << v0 << " > " << v2 << endl;
      return err;
   }
   
   float d = v0.dot(v2);

   v = v2 + v3;
   v += v4;
   v -= v2;
   v *= d;
   v /= T(.002);
   v = -v + v2;

   d = v.norm();

   if( ( err |= d != v.normalize() ) )
   {
      cerr << " : `norm, normalized` failed " << endl;
      cerr << v.normalize() << " != " << d << endl; 
      return err;
   }

   for(int i=0; i<D; ++i)
      v[i] = v3[i];
   
   if( ( err |= !( v == v3 ) ) )
   {
      cerr << " : `element assign` failed" << endl;
      cerr << v << " != " << v3 << endl;
   }

   if( !err )
   {
      cerr << " : passed " << endl;
   }
   return err;
}

/////////////////////////////////////////////////////////////////////////////
/// test vector math
/////////////////////////////////////////////////////////////////////////////
template< class T >
bool testMat()
{
   bool err = false;
   mat2<T> m;
   mat2<T> m0(T(0));
   mat2<T> m2(T(1));
   mat2<T> m3(T(2));
   mat2<T> m4(m0);

   m = m0;
   err |= !( m == m0 );

   m = m0 + m2;
   m = m * m3;

   vec<T,2> v(1);
   v = m * v;

   mat3<T> mm3;
   mat3<T> m30(T(0));
   mat3<T> m32(T(1));
   mat3<T> m33(T(2));
   mat3<T> m34(m30);

   mm3 = m30;
   err |= !( mm3 == m30 );

   mm3 = m30 + m32;
   mm3 = mm3 * m33;

   vec<T,3> v3(1);
   v3 = mm3 * v3;

   mat4<T> mm4;
   mat4<T> m40(T(0));
   mat4<T> m42(T(1));
   mat4<T> m43(T(2));
   mat4<T> m44(m40);

   mm4 = m40;
   err |= !( mm4 == m40 );

   mm4 = m40 + m42;
   mm4 = mm4 * m43;

   vec<T,4> v4(1);
   v4 = mm4 * v4;

   if(!err)
     cerr << "mat2, mat3, mat4 passed" << endl;

   return err;
}
