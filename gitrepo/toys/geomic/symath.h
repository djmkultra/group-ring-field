/// symbolic math stuffs: verbose naive brute-force ad-hoc hax


#ifndef __SYMBOLIC_MATHS_H
#define __SYMBOLIC_MATHS_H


#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <list>
#include <string>
#include <limits>
#include "vec.h"
#include "smartptr.h"




/// a modified string, captures operations as symbols

namespace Symath {
   
  /// Badness symbols, we hope to never see.
  const std::string WTF("??");
  const std::string NOTANUMBER("NAN");

  /// Important constants
  /// TODO: should just be 0 and 1
  const std::string ONE("1");
  const std::string ONE_("1");
  const std::string NEG_ONE("-1");
  const std::string ZERO("0");
  const std::string NEG_ZERO("-0");

  /// Operators and functions, these work directly on symbols: S1 * S2
  const std::string MINUS("-");
  const std::string NEG("-");
  const std::string PLUS("+");
  const std::string TIMES("*");
  const std::string DIV("/");

  /// See bottom of file for definitions that work with symbols: pow(S1,S2)
  const std::string POW("^");
  const std::string SIN("Sin");
  const std::string COS("Cos");

  /// TODO: Need to fill out "function" implementation, what about n-ary ops?

  /// These are used in parsing and print-out, not in symbols.
  const std::string OPR("(");
  const std::string CPR(")");
  const std::string NOP("?");
  const std::string NUM("#");
   
   
  ///----------------------------------------------------------
  //
  /// symbolic value type
  /// names instead of numbers, capable of some simplifications
  /// would be nice to make this really dance
  //
  // TODO fix use of Ptrs and clone, only copy on modify. almost done
  // TODO make neg and minus distinquishable at least for printing
  // TODO should ops be strings?
  // TODO functions n-ary

  template <class BASE_T = std::string, class OPER_T = std::string >   
    class Sym : public BASE_T, public gutz::Counted {
  public:
  /// NOTE an empty symbol means 1 (one, mult-identiy) NOT 0 (add-identity)
  /// ... though why we end up with empty symbols is weirdness.
    
  //-----------------------------------------------------------------------------
  /// This is a sub-class of std::string, expression stored in parent
  ///   getValue(), setValue() give you easy access, so as to express yourself 
  typedef BASE_T           base_type;
  typedef BASE_T           value_type;
  typedef OPER_T           operator_type;
  typedef Sym<BASE_T>      this_type;
      
  //-----------------------------------------------------------------------------
  /// smart pointer typedef, list-of typedef
  //
  typedef typename gutz::SmartPtr< this_type >   SymSP;
  typedef typename std::list< SymSP >            SymPVec;
  typedef typename SymPVec::iterator             SymPVecIter;
      
  //-----------------------------------------------------------------------------
  /// Key constants, you should use these!
  /// 1 the multiplicitve identity
  static const Sym& one() {
    static SymSP __one;
    if (!__one) 
	__one = new Sym(ONE);
    return *__one;
  }
  /// 0 the additive identity, multiplicitive null
  static const Sym& zero() {
    static SymSP __zero;
    if (!__zero) 
	__zero = new Sym(ZERO);
    return *__zero;
  }

  //-----------------------------------------------------------------------------
  ///   DATA: Operator & Operands
  ///  Name/symbolic-quantity is stored in the base_type
  /// TODO(djmk): remove mutable, make class immutable and use ptrs more, copies less.
  mutable operator_type _op;    /// operator
  mutable SymSP         _left;     /// left hand side
  mutable SymSP         _right;    /// right hand side

  // TODO use _dopple to insure that unique symbols all have the same ref
  // TODO unsed _dopple when expression changes (copy on write)
  /// twin of this sym, allows us to have unique symbols in an expression,
  /// rather than copies.
  mutable SymSP         _doppleganger;   
      
  //-----------------------------------------------------------------------------
  /// default constructor
  Sym() 
  : base_type(WTF), _op(WTF), _left(0), _right(0)
  {}
      
  //-----------------------------------------------------------------------------
  /// c-style string constructor
  explicit Sym( const char *c_str ) 
  : base_type(c_str), _op(NOP), _left(0), _right(0)
  {
    /// default behavior:
    ///   set name-string to the null-terminated cstring
    ///   unless the values are 0, 1, -1  (SKETCHY)
    if ( c_str == (char*)0 ) 
      {
	setValue(ZERO);
      }
    else if ( c_str == (char*)-1 )
      {
	setValue(NEG);
      }
    else if ( c_str == (char*)1 || getValue() == ONE ) 
      {
	setValue(ONE_);
      }
    else if ( getValue() == NEG_ONE )
      {
	_op = NEG;
	_left = one().copyMaybe();
      }
         
  }
      
  //-----------------------------------------------------------------------------
  /// string/basetype constructor
  explicit Sym( const base_type& val ) 
  : base_type(), _op(NOP), _left(0), _right(0)
  {
    setValue(val);
  }

  //-----------------------------------------------------------------------------
  /// char constructor (checks if you use the char for numeric values -1, 0, 1
  explicit Sym( const char c ) 
  : base_type(), _op(NOP), _left(0), _right(0) 
  { 
    if ( c == -1 )
      {
	setValue(NEG);
      }
    else if ( c == 0  )
      {
	setValue(ZERO);
      }
    else if ( c != 1 )
      {
	std::string charstr; charstr += c;
	setValue( charstr );
      }
  }
      
  //-----------------------------------------------------------------------------
  /// integer constructor, number-Syms
  explicit Sym( int i ) 
  : base_type(), _op(NOP), _left(0), _right(0) 
  { 
    if ( i ==  0 )      setValue(ZERO); 
    else if ( i == -0 ) setValue(ZERO);
    else if ( i == -1 ) 
      {
	setValue(NEG + ONE);
	_op = NEG;
	_left = one().copyMaybe();
      }
    else 
      {
	std::stringstream ss;
	ss << i;
	setValue( ss.str() );
      }         
  }
      
  //-----------------------------------------------------------------------------
  //
  /// full algebra tree-node constructor
  //
  Sym( const base_type     & val,               ///< expression string (all up to this node)
       const operator_type & op,                ///< operation (intrisic symbol-values are NOP)
       SymSP                 left,              ///< left operand (0 if not relevant)
       SymSP                 right = SymSP(0) ) ///< right operand (0 if not relevant) 
  : base_type(val), _op(op), _left(left), _right(right), _doppleganger(0)
  {}
      
  //-----------------------------------------------------------------------------
  /// copy constructor, shallow copy
  Sym( const Sym &s ) 
  : base_type(s), _op(s._op), _left(s._left), _right(s._right), 
    _doppleganger(s._doppleganger)
  {
  }
      
  //-----------------------------------------------------------------------------
  /// destructor
  ~Sym()
  {
  }
           
  /// Only copy this symbol once. Save the copy with the symbol so it can be reused.
  SymSP& copyMaybe() const 
  {
    if (!_doppleganger)
      _doppleganger = new Sym( *this );
    return _doppleganger;
  }
      
  //-----------------------------------------------------------------------------
  /// assignment operator, shallow copy, shared pointers
  Sym &operator=(const Sym &s)  
  { 
    setValue(s); 
    _op = s._op;
    _left = s._left;
    _right = s._right;
    _doppleganger = s._doppleganger;
    return *this;
  }
      
  //-----------------------------------------------------------------------------
  /// Set Sym expression string 
  void setValue( const base_type &v )
  {
    base_type::operator=( v );
  }
  /// get symbol expression string
  const base_type &getValue() const
  {
    return *this;
  }
  //-----------------------------------------------------------------------------
      
  //-----------------------------------------------------------------------------
  /// CLEAR
  /// delete's child nodes, but not (necessarily) their children...
  ///   does not "collapse children, just kills the nude
  void clear() const  ///< delete children (even if const)
  {
    _op = NOP;
    _left = 0;   ///< effectively delete with smart-pointers
    _right = 0;  ///<  ''
  }
      
  //-----------------------------------------------------------------------------
  /// NUKE
  /// make sure the branch whithers and dies 
  /// NOTE: use of smart=pointers probably makes this redundant 
  void nuke() const ///< collapse children (NUKE sub trees)
  {
    collapse( _left );
    collapse( _right );
    clear();
  }
      
  //-----------------------------------------------------------------------------
  /// STATIC COLLAPSE recursively delete/nullify sub tree
  ///  NOTE: use of smart pointers probably renders this redundant 
  static void collapse( Sym *tree )
  {
    if ( !tree ) return;
         
    collapse( tree->_left );
    collapse( tree->_right );
    tree->_left = 0;
    tree->_right = 0;
  }

  static bool isEmpty(const std::string& val) 
  {
    return val.empty();
  }
  static bool isEmpty(double val)
  {
    return false;
  }
  static bool isOne(const base_type& val)
  {
    return (isEmpty(val) || val == ONE || val == ONE_);
  }
  static bool isOne(double val)
  {
    return val == 1.0;
  }
  static bool isNegOne(const std::string& val) 
  {
    return !val.empty() && (val.at(0) == '-') && isOne(val.substr(1));
  }
  static bool isNegOne(double val) 
  {
    return val == -1.0;
  }
  static bool isZero(const base_type& val)
  {
    return (!val.empty()) && (val == ZERO || val == NEG_ZERO);
  }

  // == "-"
  bool isNegSign() const 
  {
    return (getValue() == NEG && !_left && !_right);
  }
  // -a
  bool isNegateOp() const 
  {
    return (this->_op == NEG && _left && !_right);
  }
  // a - b
  bool isMinusOp() const
  {
    return (this->_op == MINUS && _left && _right);
  }
  bool isZero() const 
  {
    return isZero(getValue());
  }
  bool isOne() const
  {
    return isOne(getValue());
  }
  bool isVariable() const 
  {
    return !_left && !_right && _op == NOP && !isOne() && !isZero();
  }

  //-----------------------------------------------------------------------------
  /// -(*this)  NEGATE uinary
  Sym operator-() const   ///< NEGATE uninary
  {    
    /// -0
    if ( this->isZero() )
      {
	return zero();
      }
         
    /// --A -> A
    if ( this->isNegateOp() ) // double negative
      {
	if ( _left )
	  {
	    return *_left;
	  }
	else 
	  {
	    std::cerr << "bad operator-()" << std::endl;
	  }
      }

    /// new node - , A
    return Sym( NEG + getValue(),
		NEG,
		this->copyMaybe() 
		);
  }
      
  //-----------------------------------------------------------------------------
  /// this * s  MUL
  Sym operator*( const Sym &s ) const   ///< MULTIPLICATION
  {  
    // 1 * 1 = 1 because one is weird...
    if ( this->isOne() && s.isOne() )  
      {
	return one();
      }
	 
    // 0 * x = 0
    if ( this->isZero() || s.isZero() )  
      {
	return zero();
      }   

    // 1 * x
    if ( this->isOne() )
      {
	return s;
      }
    // x * 1
    if ( s.isOne() ) 
      {
	return *this;
      }
         
    /// Both sides are negates
    /// -l * -r = l * r
    if ( this->isNegateOp() && s.isNegateOp() )  /// both neg (now pos)
      {
	if ( _left && s._left )
	  {
	    return (*_left) * (*s._left);
	  }
	std::cout << " baaaaad * " << (*this) << " * " << s << std::endl;
      }
         
#if 0         
    /// TODO(djmk) this is actuallly broken we can't do powers greather than 2...
    if ( *this == s )  
      {
	return Sym( l + POW + base_type("2"),
		    POW,
		    this->copyMaybe(),
		    new Sym( base_type("2") )
		    );
      }
#endif    

    /// regular old *
    return Sym( this->getValue() + "*" + s.getValue(), 
		TIMES, 
		this->copyMaybe(), 
		s.copyMaybe() );
  }

  //-----------------------------------------------------------------------------
  // We should disallow sideffects... really breaks down the copying
  Sym &operator*=( const Sym &s )
  {
    return *this = (*this) * s;
  }
      
  //-----------------------------------------------------------------------------
  Sym operator-( const Sym &s ) const 
  {
    /// a-a=0
    if ( *this == s )   return zero();
    
    /// 1-1=0 because one is weird
    // TODO not needed, handled by ==
    if ( this->isOne() && s.isOne() ) 
      {
	return zero();
      }
    /// 0-a=-a
    if ( this->isZero() )
      { 
	return -s;
      }
    /// a-0=a
    if ( s.isZero() )
      {
	return *this;
      }
    /// a--b = a+b
    if ( s.isNegateOp() )
      {
	return (*this) + (*s._left); 
      }

    return Sym(getValue() + MINUS + s.getValue(), 
	       MINUS, 
	       this->copyMaybe(), 
	       s.copyMaybe()
	       ); 
  }
  //-----------------------------------------------------------------------------
  // TODO disallow sideffects
  Sym &operator-=( const Sym &s )
  {
    return *this = (*this) - s;
  }
      
  //-----------------------------------------------------------------------------
  Sym operator/( const Sym &s ) const 
  {
    if ( s.isOne() )
      {
	return (*this);
      }
    if ( this->isZero() )
      {
	return zero();
      }
    if ( s.isZero() )
      {
	std::cerr << "Sym operator/() dividion by zero!!!!!!!!" << std::endl;
	return Sym(NOTANUMBER);
      }

    // -a/-b
    if ( this->isNegateOp() && s.isNegateOp() ) 
      {
	return *_left / *s._left;
      }
    // -a/b  = -(a/b)
    if ( this->isNegateOp() )
      {
	return -( *_left / s );
      }
    // a/-b  = -(a/b)
    if ( s.isNegateOp() )
      {
	return -( *this / *s._left );
      }
         
    /// because one is weird.
    if ( this->isOne() )
      {
	return Sym( ONE + DIV + s.getValue(), 
		    DIV, 
		    one().copyMaybe(), 
		    s.copyMaybe() 
		    );
      }
         
    return Sym( getValue() + DIV + s.getValue(), 
		DIV, 
		this->copyMaybe(), 
		s.copyMaybe()
		); 
  }
  //-----------------------------------------------------------------------------
  /// TODO disallow side effects
  Sym &operator/=( const Sym &s )
  {
    return *this = (*this) / s;
  }
      
  //-----------------------------------------------------------------------------
  /// this + s   ADD
  Sym operator+( const Sym &s ) const   ///< ADDITION 
  {           
    base_type l = getValue();
    base_type r = s.getValue();
         
    /// 0 + 0 = 0
    if ( this->isZero() && s.isZero() )
      {
	return zero();
      }
    
    /// 0 + s
    if ( this->isZero() )
      { 
	return s; 
      }
    /// this + 0
    if ( s.isZero() )
      {
	return *this;
      }
   
 
    // neg right, check for cancellation
    if ( s.isNegateOp() ) 
      {
	// a + (-a) = 0
	if ( *this == *s._left ) return zero();
      }

    // neg left check for cancellation
    if ( this->isNegateOp() ) // neg left
      {
	// -a + a = 0
	if ( (*_left) == s ) return zero();
      }
         
    /// Both sides are relevant
    
    return Sym( getValue() + PLUS + s.getValue(), 
		PLUS, 
		this->copyMaybe(), 
		s.copyMaybe() 
		);
  }
      
      
  //-----------------------------------------------------------------------------
  Sym &operator+=( const Sym &s ) 
  { 
    return *this = *this + s;
  }
      
  //-----------------------------------------------------------------------------
  operator bool() const 
  { 
    return !isZero();
  }
  //-----------------------------------------------------------------------------
  bool operator<( int i ) const /// WTF?
  { 
    if ( getValue()[0] == '-' ) return true; 
    std::stringstream iss;
    iss << i;
    if ( getValue() < iss.str() ) return true;
    return false; 
  }

  //-----------------------------------------------------------------------------
  // usefull for a cannonical form, but what is it?
  bool operator<( const Sym &s ) const
  {
    // ignore negates (for now)
    if ( this->isNegateOp() )
      {
	if ( s.isNegateOp() )
	  {
	    return *(this->_left) < *(s._left);
	  }
	// -a < a
	if ( *(this->_left) == s ) 
	  {
	    return true;
	  }
	return *(this->_left) < s;
      }
    if ( s.isNegateOp() )
      {
	// a > -a
	if ( *this == *(s._left) ) 
	  {
	    return false;
	  }
	return *this < *(s._left);
      }

    // both leafs
    if ( this->isLeaf() && s.isLeaf() )
      return this->getValue() < s.getValue();

    // just left is leaf
    if ( isLeaf() ) return true;
    // just right is leaf
    if ( s.isLeaf() ) return false;

    // non-operators < operators (not sure what the conditions are to get here...)
    if ( _op != NOP && s._op == NOP ) return false;
    if ( _op == NOP && s._op != NOP ) return true;

    // two operators
    if ( _op != NOP && s._op != NOP )
      {
	if ( _op < s._op ) return true;
	if ( s._op < _op ) return false;
	
	// operators must be the same, compare operands
	if ( _left && s._left ) 
	  {
	    if ( *_left < *s._left ) return true;
	    if ( *s._left < *_left ) return false;
	  }
	if ( _right && s._right ) 
	  {
	    if ( *_right < *s._right ) return true;
	    if ( *s._right < *_right ) return false;
	  }
	// everything the same!
	return false;
      }
    // both are non-ops
    return getValue() < s.getValue();
  }

  //-----------------------------------------------------------------------------
  // tests if the two expressions are EXACTLY the same, not equivalent 
  bool operator==( const Sym &s ) const 
  {
    // because 0 and 1 are special...?
    if ( this->isOne() && s.isOne() ) return true;
    if ( this->isZero() && s.isZero() ) return true;

    if (!_left && !_right) // must be a variable
      {
	return getValue() == s.getValue();
      }
    
    if ( _op != s._op ) return false;
    
    if ( _left )
      {
	if (!s._left) return false;
	if (!(*_left == *s._left)) return false;
      }
    else if (s._left) return false;

    if ( _right )
      {
	if (!s._right) return false;
	if (!(*_right == *s._right)) return false; 
      }
    else if (s._right) return false;

    return true; 
  }
  //-----------------------------------------------------------------------------
  bool operator==( const value_type &s ) const 
  { 
    if ( this->isOne() && isOne(s) ) return true;
    if ( this->isZero() && isZero(s) ) return true;
    if ( _left || _right ) return false;
    return getValue() == s; 
  }
      
      
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
  bool isLeaf() const { return (_right == 0 && _left == 0 ); }
      
  //-----------------------------------------------------------------------------
  // distribute products through sums, make sure all +/- are above * in tree
  // (a + b) * (c + d) = ac + bc + ad + bd (sum of products only)
  Sym distribute() const {
    if ( _op == TIMES )  // l*r
      {
	Sym left = _left->distribute();
	
	if ( left._op == PLUS )
	  return ((*left._left * *_right) + (*left._right * *_right)).distribute();
	
	if ( left.isMinusOp() )
	  return ((*left._left * *_right) - (*left._right * *_right)).distribute();

	Sym right = _right->distribute();

	if ( right._op == PLUS ) 
	  return ((left * *right._left) + (left * *right._right)).distribute();

	if ( right.isMinusOp() ) 
	  return ((left * *right._left) - (left * *right._right)).distribute();

	// We tried, did left, did right, but no luck, done.
	return *this;
      }

    if ( _left && _right )
      return Sym(getValue(), _op, 
		 _left->distribute().copyMaybe(), _right->distribute().copyMaybe());
    if ( _left )
      return Sym(getValue(), _op, _left->distribute().copyMaybe());
    if ( _right )
      return Sym(getValue(), _op, NULL, _right->distribute().copyMaybe());

    return *this;
  }

  /// Remove all subtractions by negating, push all negates down to non-PLUS nodes.
  /// -(a + (b*a - c)) -> (-a) + ((-b)*a + c)
  Sym makeAdditive() const 
  {
    if ( !_left && !_right )
      return *this;

    // a - b = a + (-b)
    if ( isMinusOp() )
      {
	return _left->makeAdditive() + (-*_right).makeAdditive();
      }

    // -(e)
    if ( isNegateOp() ) 
      {
	// -(a+b) -> (-a) + (-b)
	if ( _left->_op == PLUS )
	  {
	    return ((-*_left->_left).makeAdditive() + (-*_left->_right).makeAdditive());
	  }
	// -(a-b) -> (-a) + b
	if ( _left->isMinusOp() )
	  {
	    return ((-*_left->_left).makeAdditive() + *_left->_right).makeAdditive();
	  }
      }

    return Sym(getValue(), _op,
	       _left ? _left->makeAdditive().copyMaybe() : NULL,
	       _right ? _right->makeAdditive().copyMaybe() : NULL);
  }

  //-----------------------------------------------------------------------------
  // fills vector with expressions that sum from top of tree
  // works best if you call distribute before gathering subexpressions
  void getAdditiveSubexps(SymPVec* spv) const
  {
    // a-b -> a + (-b) 
    if ( isMinusOp() ) 
      {
	_left->getAdditiveSubexps(spv);
	// negate right, then traverse
	(-*_right).getAdditiveSubexps(spv);
	return;
      }
    // -(a+b) -> (-a) + (-b)
    if ( isNegateOp() && (_left->_op == PLUS || _left->isMinusOp()) ) 
      {
	// we are a negate op, see if it helps to "makeAdditive"
	this->makeAdditive().getAdditiveSubexps(spv);
	return;
      }
    if ( _op != PLUS )  // non sum node, add this subexpression
      {
	spv->push_back( this->copyMaybe() );
	return;
      }
    if ( _left  ) _left->getAdditiveSubexps( spv );
    if ( _right ) _right->getAdditiveSubexps( spv );
  }

  // returns true if there was a negation.
  // works best if you call "makeAdditive" first
  bool getMultiplicitiveSubexps(SymPVec *spv) const
  {
    bool flip_sign = false;
    if ( isNegateOp() ) 
      {
	flip_sign = !flip_sign;
	if (_left->getMultiplicitiveSubexps( spv ))
	  flip_sign = !flip_sign;
	return flip_sign;
      }
    if ( _op != TIMES )
      {
	spv->push_back( this->copyMaybe() );
	return flip_sign;
      }
    if ( _left ) 
      if (_left->getMultiplicitiveSubexps( spv ))
	flip_sign = !flip_sign;
    if ( _right ) 
      if (_right->getMultiplicitiveSubexps( spv ))
	flip_sign = !flip_sign;
    return flip_sign;
  }

  //--------------------------------------------------------------
  // Returns an expression that is the sum of multiplies, with all 
  // expressions sorted.  If two different but equivalent expressions
  // are placed in normal form, we should be able to detect equivalence
  // a*(a-b) + (a+b)(a+b) + c = c + -a*b + a*a + a*a + a*b + a*b + b*b
  Sym normalForm(bool distrib = true) const {
    if ( isLeaf() )
      return *this;

    struct SymPComp {
      bool operator() (const SymSP& a, const SymSP& b) {
	return (*a) < (*b);
      }
    } sym_ptr_comp;

    // if |distrib| is false, we are probably inside a recursion, it's already done.
    // Distribute terms, make sum of products
    Sym norm = distrib ? this->distribute() : (*this);

    // Gather all additive subexpressions into a flat list
    SymPVec addexps;
    norm.getAdditiveSubexps(&addexps);
    
    for (auto aiter = addexps.begin(), aend = addexps.end(); aiter != aend; ++aiter)
      {
	// for each +subexp, gather multiplicitve subexpressions into flat list
	SymPVec multexps;
	bool neg = (*aiter)->getMultiplicitiveSubexps(&multexps);
	if (multexps.size() > 1)  // more than one subexpression
	  {
	    for (auto miter = multexps.begin(), mend = multexps.end(); miter != mend; ++miter)
	      {
		// recurse, normalize these sub expressions first
		(*miter) = (*miter)->normalForm(false).copyMaybe();
	      }
	    // sort * subexps
	    multexps.sort(sym_ptr_comp);
	    // rebuild multiplicitive expression tree from normalized subexps
	    Sym mexp(one());
	    for (auto miter = multexps.begin(), mend = multexps.end(); miter != mend; ++miter)
	      {
		mexp = mexp * *(*miter);
	      }
	    // don't forget to return the negative we stripped off
	    if ( neg )
	      mexp = -mexp;
	    (*aiter) = mexp.copyMaybe();
	  }
	else // only one expression, do not recurse, put the negative back tho
	  {
	    if (neg)
	      (*multexps.begin()) = (-*(*multexps.begin())).copyMaybe();
	  }
      }
    // sort additive subexpressions
    addexps.sort(sym_ptr_comp);
    // rebuild entire expression
    Sym aexp(zero());
    for (auto aiter = addexps.begin(), aend = addexps.end(); aiter != aend; ++aiter)
      {
	aexp = aexp + *(*aiter);
      }
    return aexp;
  }
      
  //-----------------------------------------------------------------------------
  /// search for sub-expressions that sum to zero
  /// normalize_form = true applies transformations to stadardize the expression form
  Sym cancelAdditions(bool normalize_form = true) const
  {
    Sym std_form = normalize_form ? this->normalForm() : (*this);

    // no additive subexpressions to cancel at this level, recurse on subexpressions.
    if ( ! (std_form._op == PLUS || std_form.isMinusOp()) )
      {
	if (std_form._left || std_form._right)  // carefull to never copy leaf nodes.
	  return Sym(std_form.getValue(), std_form._op, 
		     std_form._left ? std_form._left->cancelAdditions().copyMaybe() : NULL,
		     std_form._right ? std_form._right->cancelAdditions().copyMaybe() : NULL);
	return *this;
      }

    SymPVec subexps;
    
    /// distribute and grab all sub expression connected by addition (commute trivially)    
    std_form.getAdditiveSubexps( &subexps );
         
    /// double loop checking to see if any subexpression will cancel each others
    ///   if they do, remove both from list
    SymPVecIter spviA = subexps.begin();
    while ( spviA != subexps.end() ) 
      {
	if ( !(*spviA) ) std::cout << " BOOM " << std::endl;
	if ( (*spviA)->isZero() )  // already zero, remove
	  {
	    subexps.erase(spviA++);
	  }
	bool increment = true;
	SymPVecIter spviB = spviA;
	++spviB;
	while ( spviB != subexps.end() ) 
	  {
	    if ( !(*spviB) ) std::cout << " BOMB " << std::endl;
	    /// is 0, remove
	    if ( (*spviB)->isZero() )
	      {
		subexps.erase(spviB++);
		continue;
	      }
	    ///  -left + right
	    if ( ((*spviA)->isNegateOp() && !(*spviB)->isNegateOp()) )
	      {
		if ( (*(*spviA)->_left) == (*(*spviB)) )
		  {  // CANCEL, nuke both
		    increment = false;
		    subexps.erase(spviB);  // you MUST delete this one first
		    subexps.erase(spviA++); // so this increment works
		    break;
		  }
	      }
	    else if ( !(*spviA)->isNegateOp() && (*spviB)->isNegateOp() )
	      {  //      left + -right = 0
		if ( *(*spviA) == *(*spviB)->_left )
                  {  // CANCEL, nuke
		    increment = false;
		    subexps.erase(spviB);   // same as above
		    subexps.erase(spviA++);
		    break;
                  }
	      }
	    ++spviB;
	  } /// end while B
	//std::cout << std::endl;
	if (increment) 
	  ++spviA;
      } /// end while A

    // Assemble new expression.
    Sym sym(zero());
    for (SymPVecIter iter = subexps.begin(), end = subexps.end(); iter != end; ++iter)
      {
	// recurse on sub-expressions, no need to normalize again.
	Sym canceled = (*iter)->cancelAdditions(false);
	sym = sym + canceled;
      }

    return sym;
  }
      
  //-----------------------------------------------------------------------------
  /// Print as a tree
  // helper, gets the right indent for any given value/operator name length
  inline std::string getIndent(int indent) const 
  {
    return std::string(indent, ' ');
  }

  std::ostream& printTree(std::ostream &os, int indent = 0) const
  {
    /// negate node, or unitary func I want negs printed in-line  -*  or - A * B
    if ( this->_op != NOP && 
	 ((this->_left && !this->_right) || (this->_right && !this->_left))  )
      {
	Sym* sym = _left ? _left : _right;
	std::string str(getIndent(indent) + _op + " ");
	if ( sym->_op != NOP ) // operator
	  {
	    str += sym->_op;
	  }
	else if ( !sym->getValue().empty() )
	  {
	    str += sym->getValue();
	  }
	else  // one I guess? 
	  {
	    str += "<1>";
	  }

	if (sym->_left) sym->_left->printTree(os, str.length() + 1);
	os << str << "\n";
	if (sym->_right) sym->_right->printTree(os, str.length() + 1);
	return os;
      } /// END IF NEG NODE

#if 0         
    /// leaf operation  print out inline:   A * B, Sin x, x ^ y etc...  
    if ( _op != NOP  && 
	 (!_right || (_right && _right->isLeaf())) &&  // maybe right 
	 (!_left || (_left && _left->isLeaf())) )  // maybe left
      {
	os << getIndent(indent) << "( ";
	if ( _left )
	  os << (*_left) << " ";
	os << _op;
	if ( _right )
	  os << " " << (*_right) << " )\n";
	return os;
      }
#endif

    std::string str(getIndent(indent));
    if ( _op != NOP ) // operator
      {
	str += _op;
      }
    else if ( !base_type::empty() )  // not operator, must be value_type
      {
	str += getValue();
      }
    else // One, I guess?
      {
	str += "<1>";
      }

    // print right tree.
    if ( _left )
      {
	_left->printTree( os, str.length() + 1 );
      }
    
    os << str << "\n";

    if ( _right  )
      {
	_right->printTree( os, str.length() + 1 );
      }

    return os;
  }
      
  std::ostream& printInfix( std::ostream &os, const base_type &last_op = NOP ) const
  {
    bool parens = true;
    if ( last_op == NOP || last_op == PLUS || last_op == MINUS || (!_left && !_right) ) parens = false; 
    if ( isNegateOp() ) parens = false;
    if ( _op == TIMES || _op == DIV ) parens = false;
    
    // treat negate as -1 * x so we don't confuse neg and minus
    base_type this_op = isNegateOp() ? TIMES : _op;

    if ( parens ) os << OPR;
    if ( isNegateOp() ) os << NEG;
    if ( _left )  _left->printInfix( os, this_op );
    if ( _op != NOP && !isNegateOp() ) os << _op;
    else if ( !isNegateOp() )  os << getValue();
    if ( _right ) _right->printInfix( os, this_op );
    if ( parens ) os << CPR;
    return os;
  }
      
      
  //-----------------------------------------------------------------------------
  /// standard ostream << printer
  std::ostream &operator<<( std::ostream &os ) const
  {
    printInfix(os, TIMES);
    return os;
         
    if ( _op == PLUS ) os << "(";
    if ( _op == NEG )  os << "-";
    if ( _left ) _left->operator<<(os);
    if ( _op != NOP ) os << _op;
    else             os << getValue();
    if ( _right ) _right->operator<<(os);
    if ( _op == PLUS ) os << ")";
    return os;
  }
      
  };
   
  typedef Sym<>   Symbol;

  /// These are handy but still verbose to use outside the namespace.
  const  Symbol   one = Symbol::one();
  const  Symbol   zero = Symbol::zero();
  const  Symbol   a("a"); 
  const  Symbol   b("b"); 
  const  Symbol   c("c"); 
  const  Symbol   d("d"); 
  const  Symbol   e("e"); 
  const  Symbol   f("f"); 
  const  Symbol   g("g"); 
   
  const  Symbol   x("x"); 
  const  Symbol   y("y"); 
  const  Symbol   z("z"); 
  const  Symbol   w("w"); 
   
   
}/// end namespace Symath


//-----------------------------------------------------------------------------
// Print operator, do we really need this?
std::ostream &operator<<( std::ostream &os, const Symath::Symbol &s  )  
{
  return s.operator<<( os );
}

// Functions added to global namespace, could we do this in the namespace?

Symath::Symbol sin(const Symath::Symbol& s) 
{
  return Symath::Symbol(Symath::SIN + Symath::OPR + s.getValue() + Symath::CPR,
			Symath::SIN,
			NULL,       // right associative function?? still working on it.
			s.copyMaybe());
}

Symath::Symbol cos(const Symath::Symbol& s) 
{
  return Symath::Symbol(Symath::COS + Symath::OPR + s.getValue() + Symath::CPR,
			Symath::COS,
			NULL,       // right associative function?? still working on it.
			s.copyMaybe());
}

// TODO(djmk): the rest of cmath...

#endif
