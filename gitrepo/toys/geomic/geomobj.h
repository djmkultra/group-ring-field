/// Geometric Object: for geometric algebras, 
///   a product of Spring 012-Summer-012 AGL Reading Group
/// djmk June 1 2012

#ifndef __GEOMETRIC_OBJECT_H
#define __GEOMETRIC_OBJECT_H

#include <map>
#include "vec.h"

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//
/// Basis Element, uses a bit vector for k-blade bases
///  0 represents scalars
///  a basis element also preserves the sign-change as a result
///      of swapping to achieve a standard representation.
//
/// Template Parameter BV_T: Type of bit-vector 
//
///  integers offer 31 basis elements  long long integers 63
///  if more are required, std::vector<bool> is speciallized
///    as a bit-vector, use if ever we go beyond 63 dimensions 
///  we are using the sign-bit to handle negation due to ordering
//
//-----------------------------------------------------------------
//-----------------------------------------------------------------

template< class BV_T = int >
class E {
public:
   //-----------------------------------------------------------------
   //
   /// using signed data type here: sign bit is used to track sign changes
   //
   typedef int bit_vector;   ///< int = max 32dim, long long int max 63dim..
   typedef bit_vector value_type;
   const static int max_dim = std::numeric_limits<bit_vector>::digits;
   
   //
   //-----------------------------------------------------------------
   
   //-----------------------------------------------------------------
   //
   /// use 0 for a scalar/"1s" basis element
   //
   explicit E( int element_id )  
   :_id(element_id > 0 ? 1<<element_id : 0)  
   { 
     if (element_id > LAST_ELEM_ID) 
       std::cout << "attempting to make a basis element with dimension out of range: " << element_id << " > " << LAST_ELEM_ID << std::endl;
   }
   E( int bitvector, bool is_bitvector ) 
   : _id( is_bitvector ? bitvector : (bitvector > 0 ? 1<<bitvector : 0))
   {
   }
   E(const E& e) 
   :_id(e._id) {}
   
   //--------------------------
   /// product options:
   enum PRODUCT_OPTIONS {
      standard   = 0,
      geometric  = 1<<1,
      dot        = 1<<2,
      wedge      = 1<<3,
      lefty      = 1<<4,
      righty     = 1<<5,
      involution = 1<<6,  
      reversion  = 1<<7,
      conjugate  = involution | reversion,
      PRODUCT_OPTION_LAST
   };
   
   /// Product constructor  
   //
   E( const E& el, const E& er, int e1_opt=standard, int e2_opt=standard )
   :_id(0)
   {
      setProduct(el,er,e1_opt,e2_opt);
   }
   
   /// assignment operator
   //
   E &operator=(const E &e) { _id = e._id; return *this; }
   
   //
   //-----------------------------------------------------------------
   
   //-----------------------------------------------------------------
   //
   /// handy numbers
   //
   enum {
      /// for external reference:
     FIRST_ELEM_ID  = 0,    /// index of first "proper" basis element (1, scalar)
     SCALAR_ID      = 0,    /// construct with this ID for scalar element
     e0             = 1<<0, /// scalar basis function, generally not printed out. 
     e1             = 1<<1,
     e2             = 1<<2,
     e3             = 1<<3,
     eminus         = 1<<25,  /// positive conformal coordinate e-e- = -1
     eplus          = 1<<26,  /// negative conformal coordinate e+e+ = +1
     LAST_ELEM_ID   = 27,     /// the rest are special   
      
     /// used internally:
     SCALAR_BIT_VEC = 0,   ///the scalar element is actually represented as 0 internally
     SIGN_BIT       = 1 << max_dim /// we use the type's sign-bit to track sign changes internally
   };
         
   /// Get I_dim = e1e2e3..edim
   static E psuedoScalar( int dim ) 
   {
      value_type bv = value_type( 0 );
      for( int i = 1; i <= dim; ++i ) 
	bv |= 1<<i;
      return E( bv, true );
   }
   
   //-----------------------------------------------------------------
   //
   /// get the sign change if any
   //
   int getSign() const { return _id & SIGN_BIT ? -1 : 1; }
   
   //-----------------------------------------------------------------
   //
   // GRADE, how many basis elements are involved? dimension of basis element, if you like
   // 0 -> scalar,  1 -> vector,  2 -> 2Blade, 3 -> VolumeBlade,  4 -> HyperBlade??
   //
   int grade() const
   {
      /// TODO: need elegant way to count the number of 1's in a bit-vector
      int count = 0;
      for(int i=1; i<LAST_ELEM_ID; ++i) count += _id & 1<<i ? 1 : 0;
      return count;
   }
   
   //-----------------------------------------------------------------
   //
   /// Basis Element Product Operator
   /// operator*()
   //
   E operator*( E &right )  { return E( *this, right );  }
   
   //-----------------------------------------------------------------
   //
   /// NOTE: because of auto-cast, this has no effect
   //
   std::ostream &operator<<(std::ostream &os) const
   {
      //if( _id & SIGN_BIT ) os << "-";
      for(int i=0; i<LAST_ELEM_ID; ++i) {
	int bit = _id & 1<<i;
	if      (bit == eminus) os << "e-";
	else if (bit == eplus)  os << "e+";
	else if (bit == e0)     os << "_";
	else if (bit)           os << "e" << i;
      }
      return os;
   }
   
   //-----------------------------------------------------------------
   //
   /// AUTO CAST: ( NOTE sign-bit is excluded ) for consistent hashing
   ///  this allows the class to have all the native properties of the 
   ///  value_type used internally.  This is a dangerous trick, so beware.
   //
   operator bit_vector () const { return _id & ~(SIGN_BIT); }
   
protected:
   bit_vector _id;
   
   /// TODO: this isn't right yet
   /// set product: because we represent all k-bases the same way
   ///  a product may induce a sign change,
   /// sets this instance to the product and sets the sign-bit if negative.
   void setProduct( const E &left, const E &right, int left_opt, int right_opt )
   {
      const int lgrade = left.grade();
      
      int lcount = 0; ///< how many elements of left basis have been seen already
      int rcount = 0;
      int swaps  = 0; ///< swaps required to sort assending product
      
      /// TODO: there must be a faster/more efficient way than checking every bit 
      for(int i=1; i < LAST_ELEM_ID; ++i)  
      { 
         /// count number of basis swaps required to sort
         lcount += left  & 1<<i ? 1 : 0;
         swaps  += right & 1<<i ? lgrade - lcount + rcount : 0;
         rcount += right & 1<<i ? 1 : 0;
      } 

      /// after cancellation of identical basis elements we have xor!
      _id = left ^ right;

      /// Argh, conformal special elements need special helps.
      bool emleft   = left  & eminus;
      bool emright  = right & eminus;

      _id = swaps % 2 ? _id | SIGN_BIT : _id;  ///< sign is neg for odd swaps, pos otherwise;

      /// e-e- = -1
      if (emleft && emright) {
	_id |= SIGN_BIT & ~(_id & SIGN_BIT);
      }
            /// options:
      _id = left_opt  & involution ? lgrade   % 2 ? _id ^ SIGN_BIT : _id : _id;
      _id = left_opt  & reversion  ? lgrade/2 % 2 ? _id ^ SIGN_BIT : _id : _id;
      _id = right_opt & involution ? rcount   % 2 ? _id ^ SIGN_BIT : _id : _id;
      _id = right_opt & reversion  ? rcount/2 % 2 ? _id ^ SIGN_BIT : _id : _id;
   }
};

/// some named bases -------------------------------
const E<>    e0     (0);
const E<>    e1     (1);
const E<>    e2     (2);
const E<>    e3     (3);
const E<>    e4     (4);
const E<>    e5     (5); 
///....
const E<>    eminus (25);  // conformal coordinate e+
const E<>    eplus  (26);  // conformal coordinate e- | e-e- = -1

/// Despite auto-cast, this external method works, it resolves before the cast
template< class BV_T >
std::ostream &operator<<( std::ostream &os, const E<BV_T> &e )
{
   return e.operator<<(os);
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//
/// Geometric Object T=Value B=Basis which defaults to class E
///  This approach uses a map from basis-element to value
///  Basis-elements compute sign changes as part of their product op
///  The map allows us to collect like terms trivially, by simply 
///  adding each new term to its unique basis bin.
//
/// The operator*() function implements the general geometric product
//
//-----------------------------------------------------------------
//-----------------------------------------------------------------

template<class T, class B=E<int> >
class GO {
public:
   //----------------------------------------------------------
   typedef T     value_type;   ///< ex float, double, etc... coefficients
   typedef B     basis_type;   ///< ex E(0), E(1)... replaced with real basis later
   //----------------------------------------------------------
   
   //----------------------------------------------------------
   /// ELEMENT MAP TYPE: key = Basis (E)  value = coefficient 
   typedef typename std::map<basis_type,value_type> ElementMap;
   typedef typename ElementMap::iterator                  EMapIter;
   typedef typename ElementMap::const_iterator            EMapCIter;
   //----------------------------------------------------------
   
   //----------------------------------------------------------
   /// Constructors
   //----------------------------------------------------------
   GO() {}
   
   /// Construct a 3D Geobj, 
   GO(const value_type &scalar,  /// basis element "1" (scalar part)
      const value_type &ve1 = 0, /// basis element e0 e.g. x  
      const value_type &ve2 = 0, /// basis element e1 e.g. y
      const value_type &ve3 = 0) /// basis element e2 e.g. z
   {
      if( scalar )
         _coefs[basis_type(0)] = scalar;
      if( ve1 )
         _coefs[basis_type(1)] = ve1;
      if( ve2 )
         _coefs[basis_type(2)] = ve2;
      if( ve3 )
         _coefs[basis_type(3)] = ve3;
   }
   
   /// Construct a 3D Geobj from coefficients and basis elements
   /// one coefficient (great for making psuedoScalars n stuff
   GO( const value_type &va, const basis_type &ba )
   {
      _coefs[ba] = va;
   }
   GO( const value_type &va, const basis_type &ba, const value_type &vb, const basis_type &bb )
   {
      _coefs[ba] = va;
      _coefs[bb] = vb;
   }
   GO( const value_type &va, const basis_type &ba, const value_type &vb, const basis_type &bb,
       const value_type &vc, const basis_type &bc )
   {
      _coefs[ba] = va;
      _coefs[bb] = vb;
      _coefs[bc] = vc;
   }
   GO( const value_type &va, const basis_type &ba, const value_type &vb, const basis_type &bb,
       const value_type &vc, const basis_type &bc, const value_type &vd, const basis_type &bd )
   {
      _coefs[ba] = va;
      _coefs[bb] = vb;
      _coefs[bc] = vc;
      _coefs[bd] = vd;
   }


   /// copy constructor
   GO( const GO &go ) : _coefs( go._coefs ) {}
   
   static GO n0() {  // n0 = e- + e+  "zero vector"
     return GO(value_type(1), eminus, value_type(1), eplus);
   }

   static GO ni() {  // ni = e-/2 - e+/2  "infinity vector"
     return GO(value_type(1) / value_type(2), eminus, -(value_type(1) / value_type(2)), eplus);
   }

   /// create a null (conformal) vector F(v) = v + n0 + v^2/2ni
   static GO nullVector(value_type v1, value_type v2, value_type v3) {
     const value_type vsquared_half = value_type((v1*v1 + v2*v2 + v3*v3)) / value_type(2); 
     return GO(value_type(0), v1, v2, v3) + n0() + ni() * vsquared_half;
   }

   /// assignment operator
   GO &operator=( const GO &go )
   {
      _coefs = go._coefs;
      return *this;
   }
   
   //----------------------------------------------------------
   /// gutz::vec assignment
   /// put a gutz::vec up in there
   ///  ex.  A.assign<> ( vec3f( 1.0, 2.1, 3.2) );
   template< class VT, int D >
   GO &assign( const gutz::vec<VT,D> &v )
   {
      for(int i=0; i<D; ++i)
         if( v[i] )
            _coefs[basis_type(i)] = v[i];
   }

   //----------------------------------------------------------
   /// GEOMETRIC PRODUCT, [operator*()]:  returns [this_object] * [right]
   ///  includes both scalar-inner and wedge-exterior products all in one
   GO operator*( const GO &right ) const
   {
      GO prod;
      const ElementMap &rem = right._coefs;
      /// NOTE the new method for iterating: capture the end condition in first/init block, 
      ///     this keeps end() from being evaluated every iteration, as it is in the middle/terminate block
      ///   THIS IS MUCH FASTER if you are not modifying the container in the loop.
      for( EMapCIter lefti = _coefs.begin(), LEnd = _coefs.end(); lefti != LEnd; ++lefti )
      {
         for( EMapCIter righti = rem.begin(), REnd = rem.end(); righti != REnd; ++righti )
         {
            /// new basis element (product of two current left & right bases)
            const basis_type pbase( (*lefti).first, (*righti).first );
            
            /// product of left-right elements, NOTE multiplied by base-product sign
            const value_type pv =  value_type( pbase.getSign() ) * ((*lefti).second * (*righti).second);
            
            /// ignore zeros, TODO: should consider an epsilon critera here? (hard to get floats back to zero!
            if( pv != value_type(0) && pv != value_type(-0) )  
            {
               if( prod._coefs.find(pbase) == prod._coefs.end() ) ///< first coefficient instance with this basis elem.
               {
                  prod._coefs[ pbase ] =  pv;
               }
               else                                               ///< basis element exists, add coefficents
               {
                  prod._coefs[ pbase ] += pv;
               }

            }
         }
      }    
      /// additional pass to get rid of "0" valued elements
      /// NOTE: we are not using the early END capture in this case since we expect the size of the
      ///   map that we are iterating over to change ( which may change the value of end() )
      EMapIter pi = prod._coefs.begin(), END = prod._coefs.end();
      while (pi != END)
      {
         if( (*pi).second == value_type(0) || (*pi).second == value_type(-0) )
         {
            prod._coefs.erase( pi++ );
         } else {
	   ++pi;
	 }
      }
      if (prod._coefs.empty())
	prod._coefs[e0] = 0;
      return prod; 
   }
   
   //----------------------------------------------------------
   /// Scalar product, on right hand side, left hand should be done outside class
   GO operator*( const value_type &vt ) const 
   {
      GO prod;
      for( EMapCIter lefti = _coefs.begin(); lefti != _coefs.end(); ++lefti )
      {
         prod._coefs[ (*lefti).first ] = (*lefti).second * vt;
      }
      return prod;
   }
   
   
   //----------------------------------------------------------
   /// WEDGE PRODUCT (exterior), same as geometric product but drop the scalar part
   GO wedge( const GO &right ) const 
   {
      GO wgo = *this * right; ///< geometric product
      EMapIter smi = wgo._coefs.find( e0 ); ///< find scalar element
      if( smi != wgo._coefs.end() )  wgo._coefs.erase( smi ); ///< delete it
      return wgo;  ///< viola, wedge
   }
   
   //----------------------------------------------------------
   /// get the wedge-part of THIS Geobj
   GO getWedge() const
   {
      GO W( *this );
      return W.makeWedge();
   }
   
   //----------------------------------------------------------
   /// SIDE-EFFECT WEDGE, MAKE *THIS* Geobj a wedge (drop scalar part)
   GO &makeWedge()
   {
      EMapIter smi = _coefs.find( e0 );
      if( smi != _coefs.end() ) _coefs.erase(smi);
      return *this;
   }
   
   //----------------------------------------------------------
   /// INNER PRODUCT (dot)  same as [this_obj] dot [right]
   value_type inner( const GO &right ) const
   {
      GO gp = *this * right;
      EMapIter imi = gp._coefs.find( e0 );
      if( imi != gp._coefs.end() ) return (*imi).second;
      return value_type(0);
   }
   
   //----------------------------------------------------------
   /// get the inner-part of THIS Geobj
   value_type getInner() const 
   {
      EMapCIter imi = _coefs.find( e0 );
      if( imi != _coefs.end() ) return (*imi).second;
      return value_type(0);
   }
   
   //----------------------------------------------------------
   /// INVERSE of this Geobj
   GO inverse() const 
   {
      GO TT = (*this) * (*this);
      return (*this) * (value_type(1)/TT.getInner());
   }
   
   //----------------------------------------------------------
   /// Complement this * -I^dim
   GO dual( int dim ) const
   {
     const basis_type I(basis_type::psuedoScalar( dim ));
     GO compme;
     for( EMapCIter emi = _coefs.begin(), END = _coefs.end(); emi != END; ++emi )
       compme._coefs[ basis_type( (*emi).first, I ) ] = (*emi).second;
     return compme;
   }

   //----------------------------------------------------------
   /// subtraction
   GO operator-(const GO& right) {
     GO sub;
     const ElementMap &rem = right._coefs;
     EMapCIter righti = rem.begin(), REnd = rem.end();
     EMapCIter lefti = _coefs.begin(), LEnd = _coefs.end();
     while((righti != REnd) || (lefti != LEnd)) {
       if ((righti != REnd) && (lefti != LEnd)) {
	 if ((*righti).first < (*lefti).first) {
	   sub._coefs[(*righti).first] = -(*righti).second;
	   ++righti;
	 } else if ((*lefti).first < (*righti).first) {
	   sub._coefs[(*lefti).first] = (*lefti).second;
	   ++lefti;
	 } else {  // must be equal
	   if ((*lefti).first != (*righti).first) std::cout << "bang\n";
	   const value_type diff = (*lefti).second - (*righti).second;
	   if (diff != value_type(0))
	     sub._coefs[(*lefti).first] = diff;
	   ++righti;
	   ++lefti;
	 }
       } else if (righti != REnd) {
	 sub._coefs[(*righti).first] = -(*righti).second;
	 ++righti;
       } else if (lefti != LEnd) {
	 sub._coefs[(*lefti).first] = (*lefti).second;
	 ++lefti;
       }
     }
     if (sub._coefs.empty())
       sub._coefs[ e0 ] = value_type(0);
     return sub;
   }

   //----------------------------------------------------------
   /// addition
   GO operator+(const GO& right) {
     GO add;
     const ElementMap &rem = right._coefs;
     EMapCIter righti = rem.begin(), REnd = rem.end();
     EMapCIter lefti = _coefs.begin(), LEnd = _coefs.end();
     while((righti != REnd) || (lefti != LEnd)) {
       if ((righti != REnd) && (lefti != LEnd)) {
	 if ((*righti).first < (*lefti).first) {
	   add._coefs[(*righti).first] = (*righti).second;
	   ++righti;
	 } else if ((*lefti).first < (*righti).first) {
	   add._coefs[(*lefti).first] = (*lefti).second;
	   ++lefti;
	 } else {  // must be equal
	   if ((*lefti).first != (*righti).first) std::cout << "bang\n";
	   const value_type sum = (*lefti).second + (*righti).second;
	   if (sum != value_type(0))
	     add._coefs[(*lefti).first] = sum;
	   ++righti;
	   ++lefti;
	 }
       } else if (righti != REnd) {
	 add._coefs[(*righti).first] = (*righti).second;
	 ++righti;
       } else if (lefti != LEnd) {
	 add._coefs[(*lefti).first] = (*lefti).second;
	 ++lefti;
       } else {
	 std::cout << "wtf";
       }
     }
     if (add._coefs.empty())
       add._coefs[e0] = value_type(0);
     return add;
   }
   
   //----------------------------------------------------------
   /// negation
   GO operator-() const
   {
      GO negme;
      for( EMapCIter emi = _coefs.begin(), END = _coefs.end(); emi != END; ++emi )
         negme._coefs[ (*emi).first ] = -(*emi).second;
      return negme;
   }
   
   //----------------------------------------------------------
   /// stream print, but std::streams can't find it behind the templating...
   ///   called from a operator<<() defined outside this class (below)
   std::ostream &operator<<(std::ostream &os) const
   {
      for( EMapCIter emi = _coefs.begin(); emi != _coefs.end(); ++emi )
      {
         if( emi != _coefs.begin() )
         {
            if( (*emi).second > 0 )  os << " + " << (*emi).second;
            else                     os << " - " << -(*emi).second;
         }
         else                        os << (*emi).second;
         
         os << (*emi).first;
      }
      if (_coefs.empty()) os << "0";
      return os;
   }
      
   /// the ELEMENT MAP key=Basis value=coefficients
   ElementMap _coefs;
   
};

/// left scalar product  s * A
template< class T, class B >
GO<T,B> operator*(const T &scalar, const GO<T,B> &g)
{
   return g*scalar; ///< scalar multiplication commutes
}

/// Psuedo-Scalar 
template< class T, class B >
GO<T,B> I( int dim )
{
   return GO<T,B>( T(1), B::psuedoScalar( dim ) );
}

/// wedge ( A, B )
template< class T, class B >
GO<T,B> wedge( const GO<T,B> &ga, const GO<T,B> &gb )
{
   return ga.wedge(gb);
}

/// inner ( A, B )
template< class T, class B >
GO<T,B> inner( const GO<T,B> &ga, const GO<T,B> &gb )
{
   return ga.inner(gb);
}

/// inverse ( A )
template< class T, class B >
GO<T,B> inverse( const GO<T,B> &ga )
{
   return ga.inverse();
}

/// dualize or complement ( A, dim )  [dim] is the dimension of the of the space embedding [A]
///   example: the dual of 1e01 in 3D is 1e2
///   example: the dual of a ^ b in 3D identical to the cross-product a x b
///   definition:  dual(A) = AI^(-1) where [I] is the unit [dim]-blade or psuedo-scalar of grade [dim]
//
template< class T, class B >
GO<T,B> dual( const GO<T,B> &ga, int dim )
{
   return ga.dual(dim);
}

/// meet ( A, B )  the intersection of A and B
///   definition: meet(A,B) = A V B = dual(A) J B = (dual(A) ^ dual(B))I 
template< class T, class B >
GO<T,B> meet( const GO<T,B> &ga, const GO<T,B> &gb, int dim )
{
   //return left_inner( dual(ga,dim), gb );
   
   return dual(ga, dim).wedge( dual(gb, dim) ) * I<T,B>( dim );
}



/// you can ditch tedious template parameters with typedefs

typedef GO<float>  GOf;
typedef GO<double> GOd;

/// operator<<() std::streams DO see this version for output
template< class T, class B >
std::ostream &operator<<(std::ostream &os, const GO<T,B> &g)
{
   /// calls the one defined inside GO
   return g.operator<<(os);
}


#ifdef __SYMBOLIC_MATHS_H

/// Symbolic Geometric Object
typedef GO<Symath::Symbol> GOsym;

/// SIMPLIFY ( symbolic geometric object )
inline
GOsym simplify( const GOsym &g )
{
   typedef GOsym GOS;
   typedef GOS::EMapCIter  EMapIter;
   
   GOS ret;
   
   for( EMapIter emi = g._coefs.begin(); emi != g._coefs.end(); ++emi )
   {
      Symath::Symbol sca = (*emi).second;
      sca.cancelAdditions();
      if( sca == Symath::ZERO )
      {
         // do nothing  
      }
      else
      {
         ret._coefs[(*emi).first] = sca;
      }
      
   }
   
   return ret;
}


#endif



#endif
