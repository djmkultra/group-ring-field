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
/// Basis Element, uses a bit vector for k-blade bases / Clifford Algebra
///  e0 represents scalars and is never printed out, bitvector-value = 0x0
///  e1,e2,...eN basis elements (bitvector e1 = 0x1, e2 = 0x2, e3 = 0x4 ...)
///  e1e2 = -e2e1 anti-communitive
///  <e1,e2> = 0  orthogonal
///  <e1,e1> = |e1|^2 = +/-1 unitary
///  a basis element preserves the sign-change as a result
///    of swapping to achieve a standard representation: e1e2e3...
//
/// Template Parameters:
/// int REAL_DIM number of positive-unitary basis elements
/// int IMAGINARY_DIM number of negative-unitary elements <eI,eI> = -1
/// BV_T: Type of bit-vector defaults to integer
//
/// NOTE: integers offer max 31 basis elements  long long integers 63
///  if more are required, std::vector<bool> is speciallized
///    as a bit-vector, use if ever we go beyond 63 dimensions
///  we are using the sign-bit to handle negation due to ordering
//
//-----------------------------------------------------------------
//-----------------------------------------------------------------

template< int REAL_DIM = 4, int IMAGINARY_DIM = 1, class BV_T = int >
class E {
 public:
 //-----------------------------------------------------------------
 //
 /// using signed data type here: sign bit is used to track sign changes
 //
 typedef int bit_vector;
 typedef bit_vector value_type;
 const static int max_dim = std::numeric_limits<bit_vector>::digits - 1;
 const static int real_dim = REAL_DIM;
 const static int imaginary_dim = IMAGINARY_DIM;
 // Does not count scalar element e0
 const static int element_count = REAL_DIM + IMAGINARY_DIM;

 //
 //-----------------------------------------------------------------

 //-----------------------------------------------------------------
 //
 /// use 0 for a scalar/"1s" basis element
 //
 explicit E( int element_id )
 :_id(element_id > 0 ? 1 << (element_id-1) : 0)
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

 enum PRODUCT_OPTIONS {
   standard   = 0,
   involution = 1<<6,
   reversion  = 1<<7,
   conjugate  = involution | reversion,
   PRODUCT_OPTION_LAST
 };

 /// Product constructor = el * er
 //
 E( const E& el, const E& er, int e1_opt=standard, int e2_opt=standard )
 :_id(0)
 {
   setProduct(el,er,e1_opt,e2_opt);
 }

 /// Creates real basis elements 1..REAL_DIM
 static E real(int element_id) { return E(element_id); }
 /// Creates imaginary basis elements 1..IMAGINARY_DIM
 static E imaginary(int element_id) { return E(element_id + REAL_DIM); }

 /// assignment operator
 E &operator=(const E &e) { _id = e._id; return *this; }


 //-----------------------------------------------------------------
 /// just handy numbers may not be meaningfull for anything but E<4,1>
 enum {
   SCALAR_ID      = 0,    /// construct with this ID for scalar element
   e0             = 0,    /// scalar element id "e0" - generally not printed out.
   e1             = 1<<0, /// first ortho/unitary basis element
   e2             = 1<<1,
   e3             = 1<<2,
   e4             = 1<<3,  /// e4 and eplus are the same in "conformal coordinates E<4,1>"
   eplus          = 1<<3,  /// common name for positive conformal coordinate e+e+ = +1
   eminus         = 1<<(REAL_DIM),  /// first negative-norm / imaginary basis element. e-e- = -1
   LAST_ELEM_ID   = REAL_DIM + IMAGINARY_DIM,

   /// used internally:
   SIGN_BIT       = 1 << max_dim /// we use the type's sign-bit to track sign changes internally
 };

 /// Get I_dim = e1e2e3..edim
 static E psuedoScalar( int dim )
 {
   value_type bv = value_type( 0 );
   for ( int i = 0; i < dim; ++i )
     bv |= 1 << i;
   return E( bv, true );
 }

 /// Get I for THIS basis, dim = REAL_DIM + IMAGINARY_DIM.
 static E psuedoScalar()
 {
   value_type bv = value_type(0);
   for ( int i = 0; i < LAST_ELEM_ID; ++i )
     bv |= 1 << i;
   return E( bv, true );
 }

 /// Compute the sign of a grade-dim reversal e1e2 = -e2e1, e1 = e1 etc...
 /// sign(~X_dim) = (-1)^(k(k-1)/2) | k = dim % 4
 static int reversalSign(int dim)
 {
   const int k = dim % 4;
   return (dim * (dim - 1) / 2) % 2 ? -1 : 1;
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
 // TODO(djmk): kinda expensive to do this all the time, maybe cache bit count.
 int grade() const
 {
   /// TODO: need elegant way to count the number of 1's in a bit-vector
   /// this is probably best for a small number of bits to check.
   int count = 0;
   for (int i = 0; i < LAST_ELEM_ID; ++i)
     count += _id & (1<<i) ? 1 : 0;
   return count;
   /// For more bits, this may be faster: SWAR algorithm.
   // int i = _id - ((_id >> 1) & 0x55555555);
   // i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
   // return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
 }

 //-----------------------------------------------------------------
 //
 /// Basis Element Product Operator
 /// operator*()
 //
 E operator*( E &right ) const { return E( *this, right ); }

 //-----------------------------------------------------------------
 /// Print, because of auto-cast, this must be called from external << op.
 std::ostream &operator<<(std::ostream &os) const
 {
   for (int i = 0; i < LAST_ELEM_ID; ++i) {
     int bit = _id & 1<<i;
     if (bit)
       {
	 os << "e" << i + 1;
	 if ( i > REAL_DIM - 1 ) os << "i";  // mark imaginary elements
       }
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

 /// TODO: this isn't right yet, but it's right-ish
 /// set product: because we represent all k-bases the same way
 ///  a product may induce a sign change,
 /// sets this instance to the product and sets the sign-bit if negative.
 void setProduct( const E &left, const E &right, int left_opt, int right_opt )
 {
   _id = 0x0;

   const int lgrade = left.grade();

   int lcount = 0; ///< how many elements of left basis have been seen already
   int rcount = 0; ///< will sum to the right grade
   int swaps  = 0; ///< swaps required to sort assending product, tracks sign changes.

   /// TODO: there must be a faster/more efficient way than checking every bit
   for (int i=0; i < LAST_ELEM_ID; ++i)
     {
       /// count number of basis swaps required to sort
       lcount += left  & 1<<i ? 1 : 0;
       swaps  += right & 1<<i ? lgrade - lcount : 0;
       rcount += right & 1<<i ? 1 : 0;
     }

   /// cancellation of identical basis elements is xor!
   _id = left ^ right;

   /// handle imaginary elements eiei = -1
   for (int i = REAL_DIM; i < LAST_ELEM_ID; ++i)
     {
       // swaps tracks negations
       swaps += ((left & 1<<i) && (right & 1<<i)) ? 1 : 0;
     }

   /// options: I did this so long ago, I am not sure if it's correct.
   /// NOT TESTED
   /// involution sign = (-1)^k
   /// reversion sign = (-1)^(k(k-1)/2)   | k = k % 4
   /// conjugation sign = (-1)^(k(k+1)/2) | k = k % 4
   swaps += left_opt  & involution ? lgrade   % 2 ? 1 : 0 : 0;
   swaps += left_opt  & reversion  ? lgrade/2 % 2 ? 1 : 0 : 0;
   swaps += right_opt & involution ? rcount   % 2 ? 1 : 0 : 0;
   swaps += right_opt & reversion  ? rcount/2 % 2 ? 1 : 0 : 0;

   ///< sign is neg for odd swaps, pos otherwise;
   _id = swaps % 2 ? _id | SIGN_BIT : _id;
 }
};

/// Despite auto-cast, this external operator<< works, it resolves before the cast
template<int R, int I, class BV_T >
  std::ostream &operator<<( std::ostream &os, const E<R,I,BV_T> &e )
{
  return e.operator<<(os);
}

/// some named bases -------------------------------
/// probably NOT good to keep these around long-term.
const E<>    e0     (E<>::e0, true);  // scalar element, dimensionless
const E<>    e1     (E<>::e1, true);
const E<>    e2     (E<>::e2, true);
const E<>    e3     (E<>::e3, true);
const E<>    e4     (E<>::e4, true);
///....
const E<>    eminus (E<>::eminus, true); // conformal coordinate e- | e-e- = -1
const E<>    eplus  (E<>::eplus, true);  // conformal coordinate e+

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//
/// Geometric Object T=Value B=Basis (which defaults to E<4,1>)
///  This approach uses a map from basis-element to value
///  Basis-elements compute sign changes as part of their product op
///  The map allows us to collect like terms trivially, by simply
///  adding each new term to its unique basis bin.
//
/// E<4,1> is commonly used for "conformal coordinate" graphics.
//
//-----------------------------------------------------------------
//-----------------------------------------------------------------

template<class T, class B=E<4,1> >
  class GO {
 public:
 //----------------------------------------------------------
 typedef T     value_type;   ///< ex float, double, etc... coefficients
 typedef B     basis_type;   ///< ex E(0), E(1)... see basis description above.
 //----------------------------------------------------------

 //----------------------------------------------------------
 /// ELEMENT MAP TYPE: key = Basis (E)  value = coefficient
 typedef typename std::map<basis_type,value_type> ElementMap;
 typedef typename ElementMap::iterator            EMapIter;
 typedef typename ElementMap::const_iterator      EMapCIter;
 //----------------------------------------------------------

 //----------------------------------------------------------
 /// Constructors
 //----------------------------------------------------------
 GO() {}

 /// Construct a 3D Geobj vector,
 GO(const value_type &scalar,  /// basis element "1" (scalar part)
    const value_type &ve1 = value_type(0), /// basis element e0 e.g. x
    const value_type &ve2 = value_type(0), /// basis element e1 e.g. y
    const value_type &ve3 = value_type(0)) /// basis element e2 e.g. z
 {
   if ( scalar )
     _coefs[basis_type(0)] = scalar;
   if ( ve1 )
     _coefs[basis_type(1)] = ve1;
   if ( ve2 )
     _coefs[basis_type(2)] = ve2;
   if ( ve3 )
     _coefs[basis_type(3)] = ve3;
 }

 /// Construct a 3D Geobj from coefficients and basis elements
 /// one coefficient (great for making psuedoScalars n stuff)
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
 GO( const GO& go ) : _coefs( go._coefs ) {}

 static GO n0() {  // n0 = e- + e+  "zero vector"
   return GO(value_type(1), eminus, value_type(1), eplus);
 }

 static GO ni() {  // ni = e-/2 - e+/2  "infinity vector"
   return GO(value_type(1) / value_type(2), eminus, -(value_type(1) / value_type(2)), eplus);
 }

 /// create a null / conformal vector F(v) = v + n0 + v^2*ni/2
 static GO conformal(value_type v1, value_type v2, value_type v3)
 {
   const value_type vsquared_half = value_type((v1*v1 + v2*v2 + v3*v3)) / value_type(2);
   return GO(value_type(0), v1, v2, v3) + n0() + ni() * vsquared_half;
 }
 /// extract the native vector from its conformal nullVector
 static GO extract(const GO& c)
 {
   /// first normalize by 1/(-conformal.ni) to get us on the null cone.
   GO result = c * (value_type(1) / (-(c.inner(ni()))));
   /// drop the homogenious components..
   EMapCIter n0i = result._coefs.find(eminus);
   if (n0i != result._coefs.end()) result._coefs.erase(n0i);
   EMapCIter nii = result._coefs.find(eplus);
   if (nii != result._coefs.end()) result._coefs.erase(nii);
   return result;
 }

 /// conformal tranlation versor:
 ///   V = e^(ni*a/2) = 1 + ni*a/2 + (ni*a/2)^2/2! +... = 1+ni*a/2 since (na)(na)=0
 /// alternatively using parallel planes, but that's more expensive setup...
 ///   Translation vector a, V = (a/|a| + |a|/2 * ni) * (a/|a| + |a| * ni)
 static GO translateVersor(const GO& a)
 {
   /// Rotor solution
   return GO(value_type(1), e0) + ni()*a*(value_type(1)/value_type(2));
   /// Pair of planes solution
   const value_type mag = a.inner(a);
   const value_type half_mag = mag / value_type(2);
   GO P(a * (value_type(1) / mag));  // normalized
   return (P + half_mag * ni()) * (P + mag * ni());
 }
 /// inverse conformal translation versor, you can also just invert V
 static GO invTranslateVersor(const GO& a)
 {
   /// Rotor solution
   return GO(value_type(1), e0) - ni()*a*(value_type(1)/value_type(2));
   /// Pair of planes solution
   const value_type mag = a.inner(a);
   const value_type half_mag = mag / value_type(2);
   GO P(a * (-value_type(1) / mag));  // normalized and negated.
   return (P + half_mag * ni()) * (P + mag * ni());
 }

 /// assignment operator
 GO &operator=( const GO& go )
 {
   _coefs = go._coefs;
   return *this;
 }

 //-------------------------------------------------------
 // Product selectors
 struct INNER { // Scalar grade of product. (a.x)  Sum_jk ((a)_j(x)_k)_0
   static bool select(const basis_type& left, const basis_type& right, const basis_type& prod) {
     return prod.grade() == 0;
   }
 };
 struct FATDOT { // All grades reduced by product  Sum_jk ((a)_j(x)_k)_(|k-j|)
   static bool select(const basis_type& left, const basis_type& right, const basis_type& prod) {
     return prod.grade() == (abs(left.grade() - right.grade()));
   }
 };
 struct HESTENES { // Reduced grades, w/o scalar   Sum_jk ((a)_j(x)_k)_(|k-j|) k,j != 0
   static bool select(const basis_type& left, const basis_type& right, const basis_type& prod) {
     return prod.grade() == (abs(left.grade() - right.grade())) && left.grade() && right.grade();
   }
 };
 struct LEFT { // Left contraction aJX             Sum_jk ((a)_j(x)_k)_(k-j) | k-j >= 0
   static bool select(const basis_type& left, const basis_type& right, const basis_type& prod) {
     return prod.grade() == (right.grade() - left.grade());
   }
 };
 struct RIGHT { // Right contraction ALx           Sum_jk ((a)_j(x)_k)_(j-k) | j-k >= 0
   static bool select(const basis_type& left, const basis_type& right, const basis_type& prod) {
     return prod.grade() == (left.grade() - right.grade());
   }
 };
 struct WEDGE { // All grades increased by product Sum_jk ((a)_j(x)_k)_(j+k)
   static bool select(const basis_type& left, const basis_type& right, const basis_type& prod) {
     return prod.grade() == (left.grade() + right.grade());
   }
 };
 struct GEOMETRIC { // Everything                  Sum_jk (a)_j(x)_k
   static bool select(const basis_type& left, const basis_type& right, const basis_type& prod) {
     return true;
   }
 };

 // The product operator, use selector to change type of product.
 template<class Selector>
 static GO product( const GO& left, const GO& right )
 {
   GO prod;
   const ElementMap &lem = left._coefs;
   const ElementMap &rem = right._coefs;

   for ( EMapCIter lefti = lem.begin(), LEnd = lem.end(); lefti != LEnd; ++lefti ) 
     {
       for ( EMapCIter righti = rem.begin(), REnd = rem.end(); righti != REnd; ++righti ) 
	 {
	   /// new basis element (product of two current left & right bases)
	   const basis_type pbase( (*lefti).first, (*righti).first );
	   /// ask selector if we want this product element
	   if (!Selector::select((*lefti).first, (*righti).first, pbase)) continue;
	   /// product of left-right elements, NOTE multiplied by base-product sign
	   const value_type pv =  value_type( pbase.getSign() ) * ((*lefti).second * (*righti).second);
	   
	   /// ignore zeros
	   /// TODO: should consider an epsilon critera here? (hard to get floats back to zero!
	   if ( !(pv == value_type(0) || pv == value_type(-0)) )  
	     {
	       if ( prod._coefs.find(pbase) == prod._coefs.end() ) 
		 { ///< first coefficient instance with this basis elem.
		   prod._coefs[ pbase ] =  pv;
		 } 
	       else 
		 { ///< basis element already exists, add coefficents
		   prod._coefs[ pbase ] += pv;
		 }
	     }
	 }
     }
   /// additional pass to get rid of "0" valued elements
   EMapIter pi = prod._coefs.begin(), END = prod._coefs.end();
   while (pi != END)
     {
       value_type v = prod.simplify((*pi).second);
       if ( v == value_type(0) || v == value_type(-0) )
         {
	   prod._coefs.erase( pi++ );
         } 
       else 
	 {
	   (*pi).second = v;
	   ++pi;
	 }
     }
   
   /// make sure we have at least one element, even if it is zero.
   if (prod._coefs.empty())
     prod._coefs[e0] = value_type(0);
   return prod;
 }

 //----------------------------------------------------------
 /// GEOMETRIC PRODUCT, [operator*()]:  returns [this_object] * [right]
 ///  includes both scalar-inner and wedge-exterior products all in one
 GO operator*( const GO &right ) const
 {
   return product<GEOMETRIC>(*this, right);
 }

 //----------------------------------------------------------
 /// Scalar product, on right hand side, left hand should be done outside class
 GO operator*( const value_type &vt ) const
 {
   GO prod;
   for ( EMapCIter lefti = _coefs.begin(), END = _coefs.end(); lefti != END; ++lefti )
     {
       prod._coefs[ (*lefti).first ] = simplify((*lefti).second * vt);
     }
   return prod;
 }


 //----------------------------------------------------------
 /// WEDGE PRODUCT (exterior), same as geometric product but drop the scalar part
 GO wedge( const GO &right ) const
 {
   return product<WEDGE>(*this, right);
 }

 //----------------------------------------------------------
 /// INNER PRODUCT (dot)  same as [this_obj] dot [right]
 value_type inner( const GO &right ) const
 {
   return product<INNER>(*this, right).getScalar();
 }

 //----------------------------------------------------------
 // "Fatdot" INNER PRODUCT see above
 GO fatdot( const GO &right ) const
 {
   return product<FATDOT>(*this, right);
 }

 //----------------------------------------------------------
 // "Hestenes" INNER PRODUCT see above
 GO hestenes( const GO &right ) const
 {
   return product<HESTENES>(*this, right);
 }

 //----------------------------------------------------------
 // Left Contraction:  aJX  (a is this, X is right)
 GO left( const GO &right ) const
 {
   return product<LEFT>(*this, right);
 }

 //----------------------------------------------------------
 // Right Contraction:  XLa  (X is this, a is right)
 GO right( const GO &right ) const
 {
   return product<RIGHT>(*this, right);
 }

 //----------------------------------------------------------
 /// get the scalar part of THIS Geobj
 value_type getScalar() const
 {
   EMapCIter imi = _coefs.find( e0 );
   if ( imi != _coefs.end() )
     return (*imi).second;
   return value_type(0);
 }

 //----------------------------------------------------------
 /// INVERSE = X^(-1) = X~/(X.X~)
 GO inverse() const
 {
   GO rev((*this).reverse());
   const value_type sqr_mag = (*this).inner(rev);
   return rev * (value_type(1)/sqr_mag);
 }

 //----------------------------------------------------------
 /// REVERSE X~ negates the right basis elements.
 GO reverse() const
 {
   GO r(*this);
   for ( EMapIter emi = r._coefs.begin(), END = r._coefs.end(); emi != END; ++emi )
     {
       const int grade = (*emi).first.grade();
       const value_type sign(basis_type::reversalSign(grade));
       (*emi).second =  sign * (*emi).second;
     }
   return r;
 }

 //----------------------------------------------------------
 /// Complement this * I_(dim)^(-1) = this * I_(dim)~
 GO dual( int dim ) const
 {
   const basis_type I(basis_type::psuedoScalar( dim ));
   GO compme;
   /// sign change for reversing the psuedoScalar
   int sign = basis_type::reversalSign(dim);
   for ( EMapCIter emi = _coefs.begin(), END = _coefs.end(); emi != END; ++emi )
     {
       const basis_type dual_e( (*emi).first, I );
       /// sign change for applying psuedoScalar
       value_type new_sign(sign * dual_e.getSign());
       compme._coefs[dual_e] = new_sign * (*emi).second;
     }
   return compme;
 }

 //----------------------------------------------------------
 /// subtraction
 GO operator-(const GO& right) const
 {
   GO sub;
   const ElementMap &rem = right._coefs;
   EMapCIter righti = rem.begin(), REnd = rem.end();
   EMapCIter lefti = _coefs.begin(), LEnd = _coefs.end();
   while((righti != REnd) || (lefti != LEnd))
     {
       if ((righti != REnd) && (lefti != LEnd))
	 {
	   if ((*righti).first < (*lefti).first)
	     {
	       sub._coefs[(*righti).first] = -(*righti).second;
	       ++righti;
	     }
	   else if ((*lefti).first < (*righti).first)
	     {
	       sub._coefs[(*lefti).first] = (*lefti).second;
	       ++lefti;
	     }
	   else
	     {  // must be equal
	       if ((*lefti).first != (*righti).first) std::cout << "bang\n";
	       const value_type diff = simplify((*lefti).second - (*righti).second);
	       if (!(diff == value_type(0)))
		 sub._coefs[(*lefti).first] = diff;
	       ++righti;
	       ++lefti;
	     }
	 }
       else if (righti != REnd)
	 {
	   sub._coefs[(*righti).first] = -(*righti).second;
	   ++righti;
	 }
       else if (lefti != LEnd)
	 {
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
 GO operator+(const GO& right) const {
   GO add;
   const ElementMap &rem = right._coefs;
   EMapCIter righti = rem.begin(), REnd = rem.end();
   EMapCIter lefti = _coefs.begin(), LEnd = _coefs.end();
   while((righti != REnd) || (lefti != LEnd))
     {
       if ((righti != REnd) && (lefti != LEnd))
	 {
	   if ((*righti).first < (*lefti).first) {
	     add._coefs[(*righti).first] = (*righti).second;
	     ++righti;
	   } else if ((*lefti).first < (*righti).first)
	     {
	       add._coefs[(*lefti).first] = (*lefti).second;
	       ++lefti;
	     }
	   else
	     {  // must be equal
	       if ((*lefti).first != (*righti).first) std::cout << "bang\n";
	       const value_type sum = simplify((*lefti).second + (*righti).second);
	       if ( !(sum == value_type(0)) )
		 add._coefs[(*lefti).first] = sum;
	       ++righti;
	       ++lefti;
	     }
	 }
       else if (righti != REnd)
	 {
	   add._coefs[(*righti).first] = (*righti).second;
	   ++righti;
	 }
       else if (lefti != LEnd)
	 {
	   add._coefs[(*lefti).first] = (*lefti).second;
	   ++lefti;
	 }
       else
	 {
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
   std::cout << "negating maybe not what you want" << std::endl;
   GO negme;
   for ( EMapCIter emi = _coefs.begin(), END = _coefs.end(); emi != END; ++emi )
     negme._coefs[ (*emi).first ] = -(*emi).second;
   return negme;
 }

 //----------------------------------------------------------
 /// stream print, but std::streams can't find it behind the templating...
 ///   called from a operator<<() defined outside this class (below)
 std::ostream &operator<<(std::ostream &os) const
 {
   for ( EMapCIter emi = _coefs.begin(); emi != _coefs.end(); ++emi )
     {
       if ( emi == _coefs.begin() )  // first element, no +/-
         {
	   os << (*emi).second;
	 }
       else
	 {
	   if ( (*emi).second > 0 )
	     os << " + " << (*emi).second;
	   else  // make negative component into a subtraction, no +-Nem
	     os << " - " << -(*emi).second;
         }
       os << (*emi).first;  // the basis element.
     }
   if (_coefs.empty()) os << "0";  // incase we have nothing.
   return os;
 }

 template<class V>
 inline V simplify(const V& value) const { return value; }

#ifdef __SYMBOLIC_MATHS_H
 Symath::Sym simplify(const Symath::Sym& sym) const { 
   return sym.cancelAdditions(); 
 }
#endif


 //------------------------------------------------------------
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

/// left contraction aJX
template< class T, class B >
  GO<T,B> left( const GO<T,B>& ga, const GO<T,B>& gx )
{
  return ga.left(gx);
}

/// right contraction XLa
template< class T, class B >
  GO<T,B> right( const GO<T,B>& gx, const GO<T,B>& ga )
{
  return gx.right(ga);
}

/// inverse ( A )
template< class T, class B >
  GO<T,B> inverse( const GO<T,B> &ga )
{
  return ga.inverse();
}

/// dualize or complement ( A, dim )  [dim] is the dimension of the of the space embedding [A]
///   example: the dual of 1e12 in 3D is 1e3
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
typedef GO<Symath::Sym> GOsym;

/// SIMPLIFY ( symbolic geometric object )
inline
GOsym simplify( const GOsym &g )
{
  typedef GOsym GOS;
  typedef GOS::EMapCIter  EMapIter;

  GOS ret;

  for ( EMapIter emi = g._coefs.begin(); emi != g._coefs.end(); ++emi )
    {
      Symath::Sym sca = (*emi).second.cancelAdditions();
 
      if ( sca == Symath::Sym::zero() )
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
