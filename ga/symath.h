/// A symbolic field.

#ifndef __SYMBOLIC_MATHS_H
#define __SYMBOLIC_MATHS_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include <limits>
#include "smartptr.h"  // TODO: maybe we should move away from smartptr?

namespace Symath {
  /// Operators:
  const std::string PLUS("+");  // associative & communitive
  const std::string TIMES("*"); // associative & communitive
  const std::string MINUS("-"); // additive inverse
  const std::string NEG("-");   // -a == -1*a
  const std::string DIV("/");   // multiplicitive inverse
  const std::string NOP("");    // Undefined

  /// See bottom of file for definitions that work with symbols: pow(S1,S2)
  /// TODO: flesh this out....
  const std::string POW("^");
  const std::string SIN("Sin");
  const std::string COS("Cos");

  /// These are used in parsing and print-out, not in symbols.
  const std::string OPR("(");
  const std::string CPR(")");
  const std::string NUM("#");

  ///----------------------------------------------------------
  //
  /// symbolic value type
  /// names instead of numbers, capable of some simplifications
  //  implements a communitive-field algebra
  // TODO functions n-ary

  class Sym : public gutz::Counted {
  public:
  /// NOTE: An empty symbol defaults to 1 (one, mult-identiy) NOT 0 (add-identity)
  /// ... though we should not end up with empty symbols.
    
  //-----------------------------------------------------------------------------
  /// smart pointer typedef, list-of typedef
  //
  typedef gutz::SmartPtr< Sym >         SymSP;
  typedef std::list< SymSP >            SymPVec;
  typedef SymPVec::iterator             SymPVecIter;
      
  //-----------------------------------------------------------------------------
  /// Key constants, you should use these!
  
  /// 1 the multiplicitve identity
  static const Sym one() { return Sym(1); }
  /// 0 the additive identity, multiplicitive null
  static const Sym zero() { return Sym(0); }

  protected:
  //-----------------------------------------------------------------------------
  ///   DATA: Operator & Operands
  ///  Name/symbolic-quantity is stored in the base_type
  std::string   _op;       /// operator
  std::string   _value;
  SymSP         _left;     /// left hand side
  SymSP         _right;    /// right hand side
  int           _integer;  /// a integral value 

  /// Twin of this symbol, allows us to have unique symbols in an expression,
  /// helps minimize copies while expressions are being built.
  mutable SymSP         _doppleganger;   
  mutable bool          _is_doppleganger;  // was object created on heap?
  
  public:
  //-----------------------------------------------------------------------------
  /// default constructor, undefined symbol defaults to "1" (multiplicitive identity)
  Sym() 
    : _value("#"), _op(), _left(0), _right(0), _integer(1), 
    _doppleganger(0), _is_doppleganger(false)
  {}
            
  //-----------------------------------------------------------------------------
  /// Create a variable : Sym a1("a1");  
  explicit Sym( const std::string& symbol ) 
    : _value(symbol), _op(), _left(0), _right(0), _integer(-0),
    _doppleganger(0), _is_doppleganger(false)
  {
    if ( isNumeral(symbol) )
      {
	if ( symbol.empty() ) 
	  {
	    (*this) = Sym(1);
	    return;
	  }
	else if ( symbol.at(0) == '#' )
	  {
	    std::stringstream ss(symbol.substr(1));
	    ss >> _integer;
	  }
	else
	  {
	    std::stringstream ss(symbol);
	    ss >> _integer;
	  }
	_value = "#";  // The integer token, so we know it's not a variable.
      }
  }

  //-----------------------------------------------------------------------------
  /// char constructor (checks if you use the char for numeric values -1, 0, 1
  explicit Sym( const char c ) 
    : _value(c,1), _op(), _left(0), _right(0), _integer(-0),
    _doppleganger(0), _is_doppleganger(false)
  { 
    if ( '0' <= c && '9' >= c )
      {
	(*this) = Sym(static_cast<int>( c - '0' ));
      }
  }
      
  //-----------------------------------------------------------------------------
  /// integer constructor, number-Symbols "#2" "#3" ... kept positive-rational
  explicit Sym( int i ) 
    : _value("#"), _op(), _left(0), _right(0), _integer(i),
    _doppleganger(0), _is_doppleganger(false) 
  {}
      
  //-----------------------------------------------------------------------------
  //
  /// expression tree-node constructor
  //
  Sym( const std::string & op,                ///< operation (intrisic symbol-values are NOP)
       SymSP                 left,              ///< left operand (0 if not relevant)
       SymSP                 right = SymSP(0) ) ///< right operand (0 if not relevant) 
    : _value(), _op(op), _left(left), _right(right), _integer(-0),
    _doppleganger(0), _is_doppleganger(false)
  {
    if ( op.empty() )
      {
	std::cerr << "Sym:invalid operator" << std::endl;
	_op = "?";
      }
  }
      
  //-----------------------------------------------------------------------------
  /// copy constructor, shallow copy
  Sym( const Sym &s ) 
    : _value(s._value), _op(s._op), _left(s._left), _right(s._right), _integer(s._integer),
    _doppleganger(s._doppleganger), _is_doppleganger(false)
  {
    if ( s._is_doppleganger )
      _doppleganger = s.copyMaybe();
  }
      
  //-----------------------------------------------------------------------------
  /// destructor
  virtual ~Sym() {}
           
  /// Only copy this symbol once. Save the copy with the symbol so it can be reused.
  /// mark the copy a copy so that it can always return itself.
  SymSP copyMaybe() const 
  {
    if ( this->_is_doppleganger )  // dopplegangers are safe to convert to smartptrs
      {  // we are the doppleganger/copy use us.
	return SymSP(const_cast<Sym*>(this));
      }
    if (!_doppleganger)  // we may or may-not have been created on the stack.
      {  // make a shallow copy of this expression on the heap to share with others.
	_doppleganger = new Sym( *this );
	_doppleganger->_is_doppleganger = true;
      }
    return _doppleganger;
  }
      
  //-----------------------------------------------------------------------------
  /// assignment operator, shallow copy, shared pointers
  Sym &operator=(const Sym &s)  
  {
    SymSP left = s._left;
    SymSP right = s._right;
    if ( this->_is_doppleganger )
      { // Arghhhh if you got here you tried to re-assign a clone,
	// but they need to be immutable so you don't change other expresions.
	// Create a new symbol instead.
	std::cerr << "Assignment to a clone not allowed, make a new symbol instead.\n";
	assert( !this->_is_doppleganger );
      }

    _value = s._value;
    _op = s._op;
    _left = s._left;
    _right = s._right;
    _doppleganger = s._doppleganger;
    _integer = s._integer;
    if ( s._is_doppleganger )
      _doppleganger = s.copyMaybe();
    return *this;
  }
      
  /// get symbol expression string
  const std::string &getValue() const { return _value; }

  //-----------------------------------------------------------------------------
      
  static bool isOne(const std::string& val)
  {  // one can be the empty string, or "one" or "1" or ....
    return val.empty() || val == "1" || val == "#1";
  }
  static bool isNegOne(const std::string& val) 
  {  // This should go away, not a valid symbol, should be -(1) (negate 1)
    return !val.empty() && (val == "-1" || val == "#-1");
  }
  static bool isZero(const std::string& val)
  {
    return !val.empty() && (val == "#0" || val == "-0" || val == "0");
  }
  static bool isNumeral(const std::string& val) {  // Symbolic integer "#" or "0"-"9"
    if (val.empty()) return false;
    char c = val.at(0);
    if (c == '#' || (c <= '9' && c >= '0')) return true;
    if (val.size() < 2) return false;
    char c2 = val.at(1);
    if (c == '-' && (c2 <= '9' && c2 >= '0')) return true;
    return false;
  }
  static bool numberCheck(const std::string& val) {  // Just check for "#"
    return (!val.empty()) && (val.at(0) == '#');
  }

  // It's tricky to tell the difference between minus and negate, these help
  bool isNegateOp() const { return (this->_op == "-" && _left && !_right); }
  bool isMinusOp() const { return (this->_op == "-" && _left && _right); }
  bool isZero() const { return isNumeral() && _integer == 0; }
  bool isOne() const { return isNumeral() && _integer == 1; }
  bool isNegOne() const { return isNumeral() && _integer == -1; }
  bool isVariable() const { return isLeaf() && _op.empty() && !numberCheck(getValue()); }
  bool isNumeral() const { return isLeaf() && numberCheck(getValue()); }
  bool isNumeralValue() const { return isNumeral() || (isNegateOp() && _left->isNumeralValue()); }
  bool isRationalFraction() const {  // is this an integer / integer?
    return _op == DIV && _left->isNumeral() && _right->isNumeral();
  }
  bool isRational() const { return isNumeral() || isRationalFraction(); }
  bool isRationalValue() const { return isRational() || (isNegateOp() && _left->isRationalValue()); }
  bool isLeaf() const { return _right == 0 && _left == 0; }

  /// Accessors for rational numbers
  int getNumerator() const { 
    if ( this->isNegateOp() && _left->isRationalValue() )
      return -_left->getNumerator();   // note the numerator will get the negate
    if ( this->isNumeral() ) return _integer;
    if ( this->isRationalFraction() ) return _left->_integer;
    return 1;
  }
  int getDenominator() const {
    if ( this->isNegateOp() && _left->isRationalValue() )
      return _left->getDenominator();  // no negate, we gave it to numerator.
    if ( this->isRationalFraction() ) return _right->_integer;
    return 1;
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
         
    if ( this->isNumeralValue() )
      {
	return Sym(-getNumerator());
      }
    if ( this->isRationalValue() )
      {
	return Sym(-getNumerator()) / Sym(getDenominator());
      }

    /// --A -> A double negative
    if ( this->isNegateOp() ) 
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
    return Sym( NEG,
		this->copyMaybe() );
  }
      
  //-----------------------------------------------------------------------------
  /// this * s  MUL
  Sym operator*( const Sym &s ) const   ///< MULTIPLICATION
  { 
    // rationals 
    // a/b * c/d = (ac)/(bd)
    if ( this->isRationalValue() && s.isRationalValue() )
      {
	return Sym( this->getNumerator() * s.getNumerator() ) / 
	  Sym( this->getDenominator() * s.getDenominator() );
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
    if ( this->isNegOne() )
      {
	return -s;
      }
 
    /// Both sides are negates
    /// -l * -r = l * r
    if ( this->isNegateOp() && s.isNegateOp() )  /// both neg (now pos)
      {
	return (*_left) * (*s._left);
      }
     
    // -1 * x = -x
    if ( this->isNegateOp() && _left->isOne() )
      {
	return -s;
      }
    // x * -1 = -x
    if ( s.isNegateOp() && s._left->isOne() ) 
      {
	return -(*this);
      }

    /// regular old *
    return Sym(	TIMES, 
		this->copyMaybe(), 
		s.copyMaybe() );
  }

  //-----------------------------------------------------------------------------
  // side-effect multiply
  Sym &operator*=( const Sym &s )
  {
    return *this = (*this) * s;
  }
      
  //-----------------------------------------------------------------------------
  Sym operator-( const Sym &s ) const 
  {
    /// a-a=0
    if ( *this == s ) 
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

    /// numbers a/b - c/d = (ad - cb)/(bd)
    if ( this->isRationalValue() && s.isRationalValue() )
      {
	int ldenom = this->getDenominator();
	int rdenom = s.getDenominator(); 
	if ( ldenom != rdenom )
	  return Sym( this->getNumerator() * rdenom  -  ldenom * s.getNumerator() ) /
	    Sym( ldenom * rdenom );
	return Sym( this->getNumerator() - s.getNumerator() ) /
	  Sym( ldenom );
      }

    /// a--b = a+b
    if ( s.isNegateOp() )
      {
	return (*this) + (*s._left); 
      }

    return Sym(	MINUS, 
		this->copyMaybe(), 
		s.copyMaybe() ); 
  }

  //-----------------------------------------------------------------------------
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
    
    // a/a = 1   NOTE: this may turn 0/0 into 1, I think that's bad....
    if ( (*this) == s ) 
      {
	return one();
      }

    if ( s.isZero() )
      {
	std::cerr << "Sym operator/() dividion by zero!!!!!!!! (";
	printInfix(std::cerr) << " / ";
	printInfix(std::cerr) << std::endl ;
	return Sym("NaN");
      }

    // rationals a/b / c/d = a/b * d/c = ad/bc
    // if both are integers don't recurse, make rational below
    if ( !(this->isNumeralValue() && s.isNumeralValue()) &&  
	 this->isRationalValue() && s.isRationalValue() )
      {
	return Sym( this->getNumerator() * s.getDenominator() ) /
	  Sym( this->getDenominator() * s.getNumerator() );
      }
    if ( this->isNumeralValue() && s.isNumeralValue() )
      {
	int numerator = this->getNumerator();
	int denominator = s.getNumerator();
	if ( numerator == denominator )
	  return one();
	if ( numerator == -denominator )
	  return Sym(-1);
      }

    // -a/-b  = a/b
    if ( this->isNegateOp() && s.isNegateOp() ) 
      {
	return *_left / *s._left;
      }
    if ( s.isNumeralValue() && s.getNumerator() < 0 )
      {
	return (-*this) / Sym(-s.getNumerator());
      }

    // a/-b  = (-a)/b  TODO unnecessary reduction... messes up others
    if ( s.isNegateOp() )
      {
	return (-*this) / *s._left;
      }
         
    return Sym(	DIV, 
		this->copyMaybe(), 
		s.copyMaybe() ); 
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

    // rationals
    // a/b + c/d = (ad + cb)/(bd)
    if ( this->isRationalValue() && s.isRationalValue() )
      {
	int ldenom = this->getDenominator();
	int rdenom = s.getDenominator(); 
	if ( ldenom != rdenom )
	  return Sym( this->getNumerator() * rdenom  +  ldenom * s.getNumerator() ) /
	    Sym( ldenom * rdenom );
	return Sym( this->getNumerator() + s.getNumerator() ) /
	  Sym( ldenom );
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
    
    return Sym(	PLUS, 
		this->copyMaybe(), 
		s.copyMaybe() );
  }
          
  //-----------------------------------------------------------------------------
  Sym &operator+=( const Sym &s ) 
  { 
    return *this = *this + s;
  }

  //-----------------------------------------------------------------------------
  // important for generating the "normalForm"
  bool operator<( const Sym &s ) const
  {
    // integers need to go at the end for normal form to work right now
    if ( this->isRationalValue() && s.isRationalValue() ) 
      {
	return this->getNumerator() * s.getDenominator() < 
	  s.getNumerator() * this->getDenominator();
      }
    if ( this->isRationalValue() ) return true;
    if ( s.isRationalValue() ) return false;

    // ignore negates for comparison, unless we have -a vs a
    // -a < a, but a < -b 
    if ( this->isNegateOp() )
      {
	if ( s.isNegateOp() )
	    return *(this->_left) < *(s._left);

	// -a < a
	if ( *(this->_left) == s ) 
	    return true;

	return *(this->_left) < s;
      }
    if ( s.isNegateOp() )
      {
	// a > -a
	if ( *this == *(s._left) ) 
	    return false;

	return *this < *(s._left);
      }

    // both leafs, compare values
    if ( this->isLeaf() && s.isLeaf() )
      return this->getValue() < s.getValue();

    // just left is leaf, a < a*a
    if ( isLeaf() ) return true;
    // just right is leaf a*a > a
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
	return false;  // a*a !< a*a
      }
    // both are non-ops
    return getValue() < s.getValue();
  }

  //-----------------------------------------------------------------------------
  // tests if the two expressions are EXACTLY the same, not equivalent 
  bool operator==( const Sym &s ) const 
  {
    if ( this->isLeaf() ) // must be a variable or number
      {
	if ( !s.isLeaf() ) return false;
	// Numbers?
	if ( this->isNumeral() && s.isNumeral() ) return this->_integer == s._integer;
	if ( this->isNumeral() || s.isNumeral() ) return false;
	// Variable.
	return getValue() == s.getValue();
      }

    // Must be an operator
    if ( _op != s._op ) return false;
    
    if ( _left && s._left )
      {
	if (!(*_left == *s._left)) return false;
      }
    else if (_left || s._left) return false;

    if ( _right && s._right )
      {
	if (!(*_right == *s._right)) return false; 
      }
    else if ( _right || s._right) return false;

    return true; 
  }
  //-----------------------------------------------------------------------------
  bool operator==( const std::string &s ) const 
  { 
    return *this == Sym(s); 
  }
                  
  //-----------------------------------------------------------------------------
  // distribute products through sums, 
  // push negation down and to the left
  // push division down and to the right
  // (a/(-e) + b) * (c + d) = -(a)c/e + bc + -(a)d/e + bd (sum of products only)
  Sym distribute() const {

    if ( isRational() ) return *this;  // effectively a leaf, could be #1/#2
    if ( isRationalValue() ) // we have one or more negates infront of a number
      {
	// negates get reduced and folded into integers
	return Sym( this->getNumerator() ) / Sym( this->getDenominator() );
      }

    if ( isNegateOp() ) // push negates down to leaves of product expressions -(a*b) --> (-a) * b
      {
	if ( _left->_op == TIMES ) // -(a * b) --> (-a) * b
	  {
	    return ((-*(_left->_left)) * *(_left->_right)).distribute();
	  }
	if ( _left->_op == PLUS ) // -(a + b) --> (-a) + (-b)
	  {
	    return ((-*(_left->_left)) + (-*(_left->_right))).distribute();
	  }
	if ( _left->_op == DIV )  // -(a / b ) --> (-a) / b
	  {
	    return ((-*(_left->_left)) / (*(_left->_right))).distribute();
	  }
	if ( _left->isMinusOp() )  // -(a - b) --> (-a) + b
	  {
	    return ((-*(_left->_left)) + (*(_left->_right))).distribute();
	  }
	if ( _left->isNegateOp() )  // -(-a) --> a
	  {
	    return _left->_left->distribute();
	  }
	if (!_left->isLeaf())
	  {
	    std::cerr << " **** WTF **** ";
	  }
      }

    if ( _op == TIMES )  // l*r
      {
	Sym left = _left->distribute();
	
	if ( left._op == PLUS )  // (a + b) * c  --> (a*c) + (b*c)
	  return ((*left._left * *_right) + (*left._right * *_right)).distribute();
	
	if ( left.isMinusOp() )  // (a - b) * c  --> (a*c) - (b*c)
	  return ((*left._left * *_right) - (*left._right * *_right)).distribute();

	Sym right = _right->distribute();

	// (a / b) * (c / d) --> (a*c) / (b * d)
	if ( left._op == DIV && right._op == DIV ) 
	  return ((*left._left * *right._left) / (*left._right * *right._right)).distribute();

	// (a / b) * c --> c * (a / b)
	if ( left._op == DIV ) 
	  return (right * left).distribute();

	// a * (b + c)  --> (a*b) + (a*c)
	if ( right._op == PLUS ) 
	  return ((left * *right._left) + (left * *right._right)).distribute();

	// a * (b - c)  --> (a*b) - (a*c)
	if ( right.isMinusOp() ) 
	  return ((left * *right._left) - (left * *right._right)).distribute();

	if ( left._op == TIMES )
	  { 
	    // (a * (b / c))*d --> (a*b*d)/c  // looks like this could be a*(b/c) --> (a*b)/c
	    if (left._right->_op == DIV) 
	      return ((*left._left * *left._right->_left * right) / *left._right->_right).distribute();
	  }

	// We tried, did left, did right, but no luck, done.
	return *this;
      }
    
    if ( _op == DIV ) // l/r
      {
	Sym left = _left->distribute();
	
	if ( left._op == PLUS )  // (a + b) / c  --> (a/c) + (b/c)
	  return ((*left._left / *_right) + (*left._right / *_right)).distribute();

	if ( left.isMinusOp() )  // (a - b) / c  --> (a/c) - (b/c)
	  return ((*left._left / *_right) - (*left._right / *_right)).distribute();

	if ( left._op == DIV ) // (a / b) / c --> a / (b * c)
	  return (*left._left / (*left._right * *_right)).distribute();

	if ( left._op == TIMES )
	  {
	    if (left._right->_op == DIV) // (a*(b/c))/d --> (a*b)/(d*c)
	      return ((*left._left * *left._right->_left) / (*_right * *left._right->_right)).distribute();
	  }
	
	Sym right = _right->distribute();

	if ( right._op == DIV ) //  a / (b / c) --> (a*c)/b
	  return (( left * *right._right ) / *right._left).distribute();
	
	if ( !(left.isNumeralValue() && right.isNumeralValue()) &&
 	    left.isRationalValue() && right.isRationalValue() )
	  {
	    std::cerr << "how the fuck";
	    return Sym(left.getNumerator() * right.getDenominator()) /
	      Sym(left.getDenominator() * right.getNumerator());
	  }

	// a / b --> a * (1/b) (unless they are rationals)
	if ( ! (left.isOne() || left.isNegOne())  )  
	  return (left * ( one() / right)).distribute();

	return left / right;
      }

    if ( _left && _right )
      return Sym(_op, 
		 _left->distribute().copyMaybe(), _right->distribute().copyMaybe());
    if ( _left )
      return Sym(_op, _left->distribute().copyMaybe());
    if ( _right )
      return Sym(_op, NULL, _right->distribute().copyMaybe());

    return *this;
  }

  /// convert subtractions to negations, push all negates down to non-PLUS nodes.
  /// -(a + (b*a - c)) -> (-a) + ((-b)*a + c)
  Sym makeAdditive() const 
  {
    if ( isLeaf() )
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

    return Sym(_op,
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
  // works best if you distribute and makeAdditive first
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

  struct SymPComp {
    bool operator() (const SymSP& a, const SymSP& b) {
      return (*a) < (*b);
    }
  } sym_ptr_comp;

  //--------------------------------------------------------------
  // Returns an expression that is the sum of multiplies, with all 
  // expressions sorted.  If two different but equivalent expressions
  // are placed in normal form, we should be able to detect equivalence
  // a*(a-b)+(a+b)(a+b)+c --> c + -a*b + a*a + a*a + a*b + a*b + b*b
  Sym sortedForm(bool distrib = true) const {
    if ( isLeaf() )
      return *this;

    // if |distrib| is false, we are probably inside a recursion, it's already done.
    // Distribute terms: make sum of products
    Sym norm = distrib ? this->distribute() : (*this);

    // Gather all additive subexpressions into a flat list
    SymPVec addexps;
    norm.getAdditiveSubexps(&addexps);
    
    for (SymPVecIter aiter = addexps.begin(), aend = addexps.end(); aiter != aend; ++aiter)
      {
	// for each +subexp, gather multiplicitve subexpressions into flat list
	SymPVec multexps;
	bool neg = (*aiter)->getMultiplicitiveSubexps(&multexps);
	if (multexps.size() > 1)  // more than one subexpression
	  {
	    for (SymPVecIter miter = multexps.begin(), mend = multexps.end(); miter != mend; ++miter)
	      {
		// recurse, sortedize these sub expressions first
		(*miter) = (*miter)->sortedForm(false).copyMaybe();
	      }
	    // sort * subexps
	    multexps.sort(sym_ptr_comp);
	    // rebuild multiplicitive expression tree from sortedized subexps
	    Sym mexp(one());
	    if ( neg )	    // don't forget to return the negative we stripped off
	      mexp = -mexp; // keep negates as close to leaves as possible

	    for (SymPVecIter miter = multexps.begin(), mend = multexps.end(); miter != mend; ++miter)
	      {
		mexp = mexp * *(*miter);
	      }

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
    for (SymPVecIter aiter = addexps.begin(), aend = addexps.end(); aiter != aend; ++aiter)
      {
	aexp = aexp + *(*aiter);
      }
    return aexp;
  }

  // +/- rational scale factor applied to a symbol
  Sym GetMultiple() const {
    if ( this->isRational() ) return *this;
    if ( this->isNegateOp() ) return -(this->_left->GetMultiple());
    if (!(_op == TIMES || _op == DIV)) return one();
    if ( _op == TIMES )
      {
	return _left->GetMultiple() * _right->GetMultiple();
      }
    if ( _op == DIV )
      {
	return _left->GetMultiple() / _right->GetMultiple();
      }
    std::cerr << " Should not get here " << std::endl;
    return one();
  }
      
  //-----------------------------------------------------------------------------
  /// search for sub-expressions that sum to zero, eliminate them
  /// sortedize_form = true applies transformations to stadardize the expression form
  Sym normalForm(bool sortedize_form = true) const
  {
    Sym std_form = sortedize_form ? this->sortedForm() : (*this);
    // no additive subexpressions to cancel at this level, recurse on subexpressions.
    if ( ! (std_form._op == PLUS || std_form.isMinusOp()) )
      {
	if (std_form._left || std_form._right)  // carefull to never copy leaf nodes.
	  return Sym(std_form._op, 
		     std_form._left ? std_form._left->normalForm().copyMaybe() : NULL,
		     std_form._right ? std_form._right->normalForm().copyMaybe() : NULL);
	return *this;
      }

    SymPVec subexps;
    
    /// grab all sub expression connected by addition (commute trivially)    
    std_form.getAdditiveSubexps( &subexps );
       
    if ( subexps.size() <= 1 ) std::cerr << "badness in cancel" << std::endl;
  
    /// double loop checking to see if any subexpression will cancel each others
    ///   if they do, remove both from list, O(N^2)
    SymPVecIter spviA = subexps.begin();
    while ( spviA != subexps.end() ) 
      {
	if ( !(*spviA) ) std::cerr << " BOOM " << std::endl;
	Sym& left_exp = **spviA;
	if ( left_exp.isZero() )  // already zero, remove
	  {
	    subexps.erase(spviA++);
	    continue;
	  }
       
	Sym left_multiple = left_exp.GetMultiple();
	Sym left = (left_exp / left_multiple).sortedForm();

	Sym new_multiple = left_multiple; 

	SymPVecIter spviB = spviA;
	++spviB;
	while ( spviB != subexps.end() ) // check against every other
	  {
	    if ( !(*spviB) ) std::cerr << " BOMB " << std::endl;
	    Sym& right_exp = **spviB;
	    Sym right_multiple = right_exp.GetMultiple();
	    /// is 0, remove
	    if ( right_exp.isZero() || right_multiple.isZero() )
	      {
		subexps.erase(spviB++);
		continue;
	      }
	    Sym right = (right_exp / right_multiple).sortedForm();
	    if ( left == right ) // A match!
	      {
		// sum their multiples:  3a + (1/3)a == (3+1/3)a
		new_multiple = new_multiple + right_multiple;
		// delete duplicate expression
		subexps.erase(spviB++);
		continue;
	      }
	    ++spviB;
	  } /// end while B

	if ( new_multiple.isZero() ) 
	  { // we can remove left, there are none
	    subexps.erase(spviA++);
	    continue;
	  }

	Sym mul_sym = new_multiple * left;

	// build the new summed expression. Don't introduce products with 1 or -1
	if ( new_multiple == Sym(-1) )
	  *spviA = (-left).copyMaybe();  // have to copy since we are assigning ourself.
	else if ( new_multiple == Sym(1) )
	  *spviA = left.copyMaybe();
	else if ( !(new_multiple == left_multiple) )
	  {
	    *spviA = (new_multiple * left).copyMaybe();
	  }
	++spviA;
      } /// end while A

    // Assemble new expression.
    Sym sym(zero());
    for (SymPVecIter iter = subexps.begin(), end = subexps.end(); iter != end; ++iter)
      {
	// recurse on sub-expressions, no need to sortedize again.
	Sym canceled = (*iter)->normalForm(false);
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
	else if ( sym->isNumeral() )
	  {
	    std::stringstream ss;
	    ss << _integer;
	    str += ss.str();
	  }
	else if ( !sym->getValue().empty() )
	  {
	    str += sym->getValue();
	  }
	else  // one I guess? 
	  {
	    str += std::string("<?>");
	  }

	if (sym->_left) sym->_left->printTree(os, str.length() + 1);
	os << str << "\n";
	if (sym->_right) sym->_right->printTree(os, str.length() + 1);
	return os;
      } /// END IF NEG NODE


    std::string str(getIndent(indent));
    if ( _op != NOP ) // operator
      {
	str += _op;
      }
    else if ( isNumeral() )
      {
	std::stringstream ss;
	ss << _integer;
	str += ss.str();
      }
    else if ( !_value.empty() )  // not operator, must be value_type
      {
	str += _value;
      }
    else // WTF?
     {
       str += "<?>";
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
      
  std::string toString() const 
  {
    std::stringstream ss;
    printInfix(ss);
    return ss.str();
  }
  
  std::ostream& printInfix( std::ostream &os, const std::string &last_op = NOP ) const
  {
    bool parens = true;
    if ( last_op == NOP || last_op == PLUS || last_op == MINUS || isLeaf() ) parens = false; 
    if ( _op == TIMES || _op == DIV ) parens = false;
    if ( isNegateOp() ) parens = false;
    if ( last_op == DIV && _op != NOP ) parens = true;

    // treat negate as -1 * x so we don't confuse neg and minus when we recurse
    std::string this_op = isNegateOp() ? TIMES : _op;

    if ( isNegateOp() ) os << NEG;
    if ( parens ) os << OPR;
    if ( _left )  _left->printInfix( os, _op != DIV ? this_op : TIMES );
    
    if ( _op != NOP && !isNegateOp() )
      {
	if (_op == PLUS || isMinusOp()) os << " ";
	os << _op;
	if (_op == PLUS || isMinusOp()) os << " ";
      }
    else if ( this->isNumeral() && !isNegateOp() ) os << _integer;
    else if ( !isNegateOp() && !getValue().empty())  os << getValue();
    else if ( !isNegateOp() && getValue().empty() ) os << "?<" << _op << "|" << _integer << ">";

    if ( _right ) _right->printInfix( os, this_op );
    if ( parens ) os << CPR;

    return os;
  }
      
      
  //-----------------------------------------------------------------------------
  /// standard ostream << printer
  std::ostream &operator<<( std::ostream &os ) const
  {
    return printInfix(os, TIMES);
  }
      
};
      
}  /// end namespace Symath


//-----------------------------------------------------------------------------
// Print operator, do we really need this?
std::ostream &operator<<( std::ostream &os, const Symath::Sym &s  )  
{
  return s.operator<<( os );
}

// Functions added to global namespace
// TODO could we do this in the namespace?

Symath::Sym sin(const Symath::Sym& s) 
{
  return Symath::Sym( Symath::SIN,
		      NULL,       // right associative function?? still working on it.
		      s.copyMaybe());
}

Symath::Sym cos(const Symath::Sym& s) 
{
  return Symath::Sym( Symath::COS,
		      NULL,       // right associative function?? still working on it.
		      s.copyMaybe());
}

// TODO(djmk): the rest of cmath...

#endif
