/// symbolic math stuffs: verbose naive brute-force ad-hoc hax


#ifndef __SYMBOLIC_MATHS_H
#define __SYMBOLIC_MATHS_H


#include <cassert>
#include <iostream>
#include <map>
#include <vector>
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
  /// smart pointer typedef, vector-of typedef
  //
  typedef typename gutz::SmartPtr< this_type >   SymSP;
  typedef typename std::vector< SymSP >          SymPVec;
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
  mutable SymSP         _dopple;   /// twin of this sym, _dopple == this if created on heap.
      
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
  : base_type(val), _op(op), _left(left), _right(right) 
  {}
      
  //-----------------------------------------------------------------------------
  /// copy constructor, shallow copy
  Sym( const Sym &s ) 
  : base_type(s), _op(s._op), _left(s._left), _right(s._right)
  {
  }
      
  //-----------------------------------------------------------------------------
  /// destructor
  ~Sym()
  {
  }
           
  //-----------------------------------------------------------------------------
  ///     CLONE : DEEP COPY, use when you modify any expression.
  /// deep copy, products (can) get freaky when compounded 
  ///   example A=B*C  C=A*A (bad since A has existing trees inside!) C=A-A + A*A (worse)
  SymSP clone() const 
  {
    SymSP ret;
    ///    TODO: got pretty close to hacking smartptr and counted into detecting 
    ///          stack versus heap variable creation.  you should ever point to stack objs
    ///
    //if ( gutz::Counted::isStackVar() ) ///< never been owed by a smart-ptr, likely stack obj
    //{
    /// because smart-pointers are self-deleteing, and heap detection is pretty unreliable
    ///   when in doubt. new
    ret = new Sym( *this );
    //}
    //else                                    ///< already referenced, no need to copy new                                  
    //{
    //   ret = const_cast<Sym*>(this);  
    //}
         
    /// recursive deep-copy( 
    if ( _left )
      ret->_left = _left->clone();
    if ( _right ) 
      ret->_right = _right->clone();
         
    return ret;
  }

  /// Only copy this symbol if we cannot verify that it was created on the heap.
  /// shallow copy
  SymSP copyMaybe() const 
  {
    if (_getCount() != STACK_VAR_ERROR_COUNT) 
      {
	return SymSP( const_cast<Sym*>(this) );
      }
    return SymSP( new Sym( *this ) );
  }
      
  //-----------------------------------------------------------------------------
  /// assignment operator, shallow copy, shared pointers
  Sym &operator=(const Sym &s)  
  { 
    setValue(s); 
    _op = s._op;
    _left = s._left;
    _right = s._right;
    //if (s._left)  _left = s._left->clone();
    //else _left = 0;
    //if (s._right) _right = s._right->clone();
    //else _right = 0;
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
    return val == ZERO || val == NEG_ZERO;
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

    // -(a-b) -> (b-a)
    if ( this->isMinusOp() )
      {
	return *_right - *_left;
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
    base_type l = getValue();
    base_type r = s.getValue();
         
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
         
    // Left only is negate, keep negate above, (-l) * r = -(l*r)
    if ( this->isNegateOp() )
      {
	return -( (*_left) * s );
      }

    //right hand side is negate, keep negate above l * (-r) = -(l*r)
    if ( s.isNegateOp() ) 
      { 
	return -( (*this) * (*s._left) );
      }
         
    /// TODO(djmk) this is actuallly broken we can't do powers greather than 2...
    if ( *this == s )  
      {
	return Sym( l + POW + base_type("2"),
		    POW,
		    this->copyMaybe(),
		    new Sym( base_type("2") )
		    );
      }
         
    if ( s < (*this) )  // swap order of operands
      {
	return Sym( r + "*" + l, 
		    TIMES, 
		    s.copyMaybe(),
		    this->copyMaybe() 
		    );
      }
         
    /// regular old *
    return Sym( l + "*" + r, 
		TIMES, 
		this->copyMaybe(), 
		s.copyMaybe() );
  }

  //-----------------------------------------------------------------------------
  // We should disallow sideffects... really breaks down the copying
  Sym &operator*=( const Sym &s )
  {
    return *this = (*this) * s;;
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
    /// (-a) - (-b) = b - a
    /// TODO this could be a problem for cannonical form.
    if ( this->isNegateOp() && s.isNegateOp() ) 
      {
	return *s._left - *_left;
      }
    /// a--b = a+b
    if ( s.isNegateOp() )
      {
	return (*this) + (*s._left); 
      }
    /// -a-b = -(a+b)
    if ( this->isNegateOp() ) 
      {
	return -((*_left) + s);
      }

    base_type expr = getValue() + MINUS + s.getValue();
    return Sym(expr, 
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
   
    /// Both negates : -l + -r = -(l + r)
    if ( this->isNegateOp() && s.isNegateOp() )
      {
	return -( *(this->_left) + *(s._left) );
      }
 
    // neg right, check for cancellation
    if ( s.isNegateOp() ) 
      {
	// a + (-a) = 0
	if ( *this == *s._left ) return zero();
	// a + (-b) = a - b
	return (*this) - *(s._left);
      }

    // neg left check for cancellation
    if ( this->isNegateOp() ) // neg left
      {
	// -a + a = 0
	if ( (*_left) == s ) return zero();
	// -a + b = b - a
	return s - (*_left);
      }
         
    /// Both sides are relevant

    /// always store in ascending sort order (for communitive ops anyway)
    if ( s < (*this) )
      {
	return Sym( s.getValue() + PLUS + getValue(),
		    PLUS,
		    s.copyMaybe(),
		    this->copyMaybe()
		    );
      }
    
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
    return getValue() < s.getValue();
  }

  //-----------------------------------------------------------------------------
  bool operator==( const Sym &s ) const 
  {
    // would be fast but unreliable to use
    // return getValue() == s.getValue()
    // because 0 and 1 are special...?
    if ( this->isOne() && s.isOne() ) return true;
    if ( this->isZero() && s.isZero() ) return true;

    if (!_left & !_right) // must be a variable
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
  void additiveSubexps( SymPVec &spv, int level = 0 )
  {
    if ( _op != PLUS )
      {
	spv.push_back( this );
	if ( ! level )
	  return;
	/// todo extract deeper levels
      }
    if ( _left  ) _left->additiveSubexps( spv );
    if ( _right ) _right->additiveSubexps( spv );
  }
      
  //-----------------------------------------------------------------------------
  /// if any nodes _op == ZERO, apply them
  void cancelZeros()
  {
         
    /// This node is zero  0
    if ( this->_op == ZERO || getValue() == ZERO )
      {
	nuke();
	this->_op = ZERO;
	return;
      }
         
    if ( _left == 0 )  ///< quick check for leafyness
      {
	return;
      }
         
    /// negate zero  -0
    if ( _op == NEG && _left->_op == ZERO )
      {
	clear();
	_op = ZERO;
	return;
      }
         
    /// multiply with a zero  0 * r or l * 0
    if ( _op == TIMES && (_left->_op == ZERO || ( _right && _right->_op == ZERO)))
      {
	nuke();
	setValue(ZERO);
	_op = ZERO;
      }
    /// add with left 0     0 + r
    else if ( _op == PLUS && (_left->_op == ZERO && (_right && _right->_op != ZERO)) )
      {
	_left->nuke();
	_left=0;
	Sym *save_right = _right;
	(*this) = (*save_right);
      }
    /// add with right 0     a + 0
    else if ( _op == PLUS && (_left->_op != ZERO && (_right && _right->_op == ZERO)) )
      {
	_right->nuke();
	Sym *left = _left;
	_right = 0;
	(*this) = (*left);
      }   
    /// add with both 0      0 + 0
    else if ( _op == PLUS && (_left->_op == ZERO && (_right && _right->_op == ZERO)) )
      {
	nuke();
	setValue(ZERO);
	_op = ZERO;
      }
    /// TODO: sub, div, etc....
         
    ///    RECURSION    ---------------------------------
    if ( _left  ) _left->cancelZeros();
    if ( _right ) _right->cancelZeros();
    /// -------------------------------------------------
         
    /// multi with a zerp or nothing   0/nil/a * 0/nil/a
    if ( _op == TIMES && ( (!_left || _left->_op == ZERO) || ( !_right || _right->_op == ZERO) ) )
      {
	nuke();
	setValue( ZERO );
	_op = ZERO;
      }
    /// add with zeros or nothings     0/nil + 0/nil
    else if ( _op == PLUS && ( (!_left || _left->_op == ZERO) && (!_right || _right->_op == ZERO)) )
      {
	nuke();
	setValue(ZERO);
	_op = ZERO;
      }
         
         
  }
      
  //-----------------------------------------------------------------------------
  /// search for sub-expressions that sum to zero
  Sym &cancelAdditions()
  {
    if ( _op != PLUS ) return *this;
    SymPVec subexps;
    /// brab all sub expression connected by addition (commute trivially)
    ///   recursive
    additiveSubexps( subexps );
         
    /// double loop checking to see if any will cancel each other
    ///   if they do, set the _op to Zero...
    for ( SymPVecIter spviA = subexps.begin(); spviA != subexps.end(); ++spviA )
      {
	if ( (*spviA)->_op == ZERO )
	  continue;
            
	for ( SymPVecIter spviB = spviA + 1; spviB != subexps.end(); ++spviB )
	  {
	    /// right is 0
	    if ( (*spviB)->_op == ZERO )
	      continue;
               
	    if ( (*spviA)->_op == NEG && (*spviB)->_op != NEG )
	      {
		///      -left + right = 0
		if (  base_type(*(*spviA)).substr(1) == base_type(*(*spviB)) )
                  {
		    (*spviA)->_op = ZERO;
		    (*spviA)->setValue( ZERO );
		    (*spviB)->_op = ZERO;
		    (*spviB)->setValue( ZERO );
                  }
	      }
	    else if ( (*spviA)->_op != NEG && (*spviB)->_op == NEG )
	      {
		///      left + -right = 0
		if ( base_type(*(*spviA)) == base_type(*(*spviB)).substr(1) )
                  {
		    (*spviA)->_op = ZERO;
		    (*spviA)->setValue( ZERO );
		    (*spviB)->_op = ZERO;
		    (*spviB)->setValue( ZERO );
                  }
	      }
	    /// otherwise no cancellation 
	  } /// end for B
      } /// end for A
         
    /// if any _op was set to zero, collapse the tree underneath, and fix addition and multi with zeros
    ///  recursive
    cancelZeros();
         
    return *this;
         
  }
      
  //-----------------------------------------------------------------------------
  /// Print as a tree
  /// TODO Seems broken, need to distinquish between neg an minus plus other stuff
  std::ostream& printTree(std::ostream &os, const std::string &indent = std::string("")) const
  {
    const std::string indent_sz("  ");
         
    /// negavie node, I want negs printed in-line  -*  or - A * B
    if ( _op == NEG )
      {
	if ( ! _left ) {
	  std::cout << "wtf" ;
	  return os;
	}
	Sym *sym = _left;
            
	/// double NEG??  This should NOT happen if tree was built properly!
	if ( sym->_op == NEG )
	  {
	    /// yes double neg at a leaf node
	    if (  sym->_left && sym->_left->isLeaf()  && !sym->_right )
	      {
		os << indent << "--+" << *(sym->_left) << "\n";
		return os;
	      }
	    /// subtraction op miss named
	    else if ( sym->_left && sym->_right )
	      {
		if ( sym->_left->isLeaf() && sym->_right->isLeaf() )
                  {
		    os << indent << *(sym->_left) << "+--+" << *(sym->_right) << "\n";
                  }
		else 
                  {
		    sym->_left->printTree(os,indent+indent_sz);
		    os << indent << "+-- \n";
		    sym->_right->printTree(os,indent+indent_sz);
                  }
	      }
	    /// double negative expression
	    else 
	      {   
		os << indent << _op << "?" << sym->_op << " [" << (*this) << "] vs \n";
		os << indent << _op << "?" << sym->_op << " [" << (*sym) << "] \n";
		if ( sym->_left )
                  {
		    sym->_left->printTree(os,indent+indent_sz);
                     
		    /// ah ha! we are miss named negation!
		    if (_right)
		      {
                        os << indent <<  "-?\n";
                        _right->printTree(os,indent+indent_sz);  
		      }
                  }
		else 
                  {
		    os << " SUPER BAD " << (*sym) <<  "\n";
                  }
                  
	      }
	    return os;
	  } /// END DOULBE NEGATIVE
            
            /// basic leaf neg
	if ( sym->isLeaf() )
	  {
	    os << indent << "-" << (*sym) << "\n";
	    return os;
	  } 
            
	/// operation leaf neg:  - A * B
	if ( sym->_op != NOP && sym->_left && sym->_left->isLeaf() && sym->_right && sym->_right->isLeaf() )
	  {
	    os << indent << "-( " << *(sym->_left) << " " << sym->_op << " " << *(sym->_right) << " )\n";
	    return os;
	  }
	/// operation node neg
	if (sym->_op != NOP && sym->_op != NEG )
	  {
	    if ( sym->_right ) sym->_right->printTree( os, indent + indent_sz + indent_sz );
	    os << indent << "-" << sym->_op << "\n";
	    if ( sym->_left  ) sym->_left->printTree( os, indent + indent_sz + indent_sz);
	    return os;
	  }
	else /// what else could there be???
	  {
	    os << indent << "-? (" << (*this) << ")\n";
	    return os;
	  }
            
      } /// END IF NEG NODE
         
    /// leaf operation  print out inline:   A * B  
    if ( _op != NOP  && _right && _right->isLeaf() && _left && _left->isLeaf() )
      {
	os << indent << " ( " << (*_left) << " " << _op << " " << (*_right) << " )\n";
	return os;
      }
         
    if ( _right )
      {
	_right->printTree( os, indent + indent_sz );
	//os << "\n";
      }
    if ( _op != NOP )  os << indent << _op << "\n";
    else if ( !base_type::empty() ) os << indent << (*this) << "\n";
    else os << indent << "1\n";
    if ( _left  )
      _left->printTree( os, indent + indent_sz );
    return os;
  }
      
  std::ostream& printInfix( std::ostream &os, const base_type &last_op = NOP ) const
  {
    bool parens = true;
    if ( last_op == NOP || last_op == PLUS || last_op == MINUS || (!_left && !_right) ) parens = false; 
    if ( isNegateOp() ) parens = false;
    if ( _op == TIMES || _op == DIV ) parens = false;
    // doesn't work because we cannot distinguish neg from minus damnit
    //if ( (last_op == PLUS && (_op == PLUS || _op == MINUS)) || 
    //	 (last_op == MINUS && (_op == PLUS || _op == MINUS)) ) parens = false; 
    //if ( last_op == TIMES && (_op == TIMES || _op == DIV) ) parens = false;
    
    // treat negate as -1 * x
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
