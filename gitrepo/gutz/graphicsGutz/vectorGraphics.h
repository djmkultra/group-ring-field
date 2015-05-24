/*
 * $Id: vectorGraphics.h,v 1.1.1.1 2006/02/25 23:47:30 jmk Exp $
 */

//vectorGraphics.h

#ifndef __VECTOR_GRAPHICS_DOT_H
#define __VECTOR_GRAPHICS_DOT_H

#include <quat.h>
#include <vec.h>
#include <mat4.h>

namespace gutz{

//=================================================================================
// Functions
//=================================================================================

///lookat
template <class T>
mat4<T>  look_at(const vec<T,3> & eye, const vec<T,3> & at, const vec<T,3> & up);

///perspective
template <class T>
mat4<T>  perspective(const T fovy, const T aspect, const T n, const T f);

///frustum
template <class T>
mat4<T>  frustum(const T l, const T r, const T b, const T t, const T n, const T f);

template <class T>
mat4<T>  ortho(const T l, const T r, const T b, const T t, const T n, const T f);

///trackball
template <class T>
quat<T>  trackball(const vec<T,2> & pt1, 
                   const vec<T,2> & pt2, 
                   const T trackballsize);

template <class T>
quat<T> & add_quats(quat<T>& p, const quat<T>& q1, const quat<T>& q2)
{
   quat<T> t1, t2;

   t1 = q1;
   t1.x() *= q2.w();
   t1.y() *= q2.w();
   t1.z() *= q2.w();

   t2 = q2;
   t2.x() *= q1.w();
   t2.y() *= q1.w();
   t2.z() *= q1.w();

   p.x() = (q2.y() * q1.z()) - (q2.z() * q1.y()) + t1.x() + t2.x();
   p.y() = (q2.z() * q1.x()) - (q2.x() * q1.z()) + t1.y() + t2.y();
   p.z() = (q2.x() * q1.y()) - (q2.y() * q1.x()) + t1.z() + t2.z();
   p.w() = q1.w() * q2.w() - (q1.x() * q2.x() + q1.y() * q2.y() + q1.z() * q2.z());

   return p;
}




//=================================================================================
// Function implementations
//=================================================================================

template <class T>
mat4<T> look_at(const vec<T,3> & eye, const vec<T,3> & at, const vec<T,3> & up)
{
   vec<T,3> x, y, z;
      
   // make rotation matrix
      
   // Z vector
   z = eye - at;
   z.normalize();
      
   // Y vector
   y = up;
      
   // X vector = Y cross Z
   x = cross(y,z);//y.cross(z);
      
   // Recompute Y = Z cross X
   y = cross(z,x);//z.cross(x);
      
   // cross product gives area of parallelogram, which is < 1.0 for
   // non-perpendicular unit-length vectors; so normalize x, y here
   x.normalize();
   y.normalize();
      
   return mat4<T>(x.x(),           y.x(),           z.x(),           0, 
                  x.y(),           y.y(),           z.y(),           0,
                  x.z(),           y.z(),           z.z(),           0,
                  -(eye.dot(x)), -(eye.dot(y)), -(eye.dot(z)), 1);
#if 0
      M.m[0]= x.x(); M.m[4]= x.y();M.m[8] = x.z();M.m[12] = -x.x()*eye.x()-x.y()*eye.y()-x.z()*eye.z();
      M.m[1]= y.x(); M.m[5]= y.y();M.m[9] = y.z();M.m[13] = -y.x()*eye.x()-y.y()*eye.y()-y.z()*eye.z();
      M.m[2]= z.x(); M.m[6]= z.y();M.m[10]= z.z(); M.m[14]= -z.x()*eye.x()-z.y()*eye.y()-z.z()*eye.z();
      M.m[3] = 0;    M.m[7]= 0;    M.m[11]= 0;   M.m[15] = 1;
      return M;
#endif
}

//---------------------------------------------------------------------------------
// Frustum
//---------------------------------------------------------------------------------

template <class T>
mat4<T>  frustum(const T l, const T r, const T b, const T t, const T n, const T f)
{
   return mat4<T> (
           T((2.0*n) / (r-l)),
           0.0,
           0.0,
           0.0,

           0.0,
           T(2.0*n) / (t-b),
           0.0,
           0.0,

           (r+l) / (r-l),
           (t+b) / (t-b),
           -(f+n) / (f-n),
           -1.0,

           0.0,
           0.0,
           -(2.0f*f*n) / (f-n),
           0.0);
}

//---------------------------------------------------------------------------------
// Perspective
//---------------------------------------------------------------------------------

template <class T>
mat4<T>  perspective(const T fovy, const T aspect, const T n, const T f)
{
   T xmin, xmax, ymin, ymax;

   ymax = n * tanf(fovy * M_PI/180 * 0.5);
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   return frustum(xmin, xmax, ymin, ymax, n, f);
}

//---------------------------------------------------------------------------------
// Orthographic
//---------------------------------------------------------------------------------

template <class T>
mat4<T>  ortho(const T l, const T r, const T b, const T t, const T n, const T f)
{
   return mat4<T> (
             T((2.0) / (r-l)),
             0.0,
             0.0,
             0.0,

             0.0,
             T(2.0) / (t-b),
             0.0,
             0.0,

             0.0,
             0.0,
             -T(2.0) / (f-n),
             0.0,

             -(r+l) / (r-l),
             -(t+b) / (t-b),
             -(f+n) / (f-n),
             1.0);
}


//---------------------------------------------------------------------------------
// Trackball
//---------------------------------------------------------------------------------

/// a helper function for the trackball
template <class T>
T tb_project_to_sphere(const T r, const T x, const T y)
{
   T z;

   const T d = sqrtf(x*x + y*y);
   if (d < r * 0.70710678118654752440) {    /* Inside sphere */
      z = sqrtf(r*r - d*d);
   } else {           /* On hyperbola */
      const T t = r / 1.41421356237309504880f;
      z = t*t / d;
   }
   return z;
}

/// The trackball function
template <class T>
quat<T>   trackball(const vec<T,2> & pt1, 
                    const vec<T,2> & pt2, 
                    const T trackballsize)
{

   vec<T,3> a; // Axis of rotation
   T phi;  // how much to rotate about axis
   vec<T,3> d;
   T t;

   if (pt1 == pt2) 
   {
      // Zero rotation
      return quatf();
   }

   // First, figure out z-coordinates for projection of P1 and P2 to
   // deformed sphere

   vec<T,3> p1(pt1.x(),pt1.y(),tb_project_to_sphere(trackballsize,pt1.x(),pt1.y()));
   vec<T,3> p2(pt2.x(),pt2.y(),tb_project_to_sphere(trackballsize,pt2.x(),pt2.y()));

   //  Now, we want the cross product of P1 and P2
   //a = p2.cross(p1);
   a = cross(p1,p2); //p1.cross(p2);

   //  Figure out how much to rotate around that axis.
   d = p1 - p2;
   t = d.norm() / (trackballsize);

   // Avoid problems with out-of-control values...

   if (t > 1)
      t = 1;
   if (t < -1) 
      t = -1;
   phi = 2 * asin(t);

   return quat<T>(phi,a);
}




}//end namespace gutz


#endif

