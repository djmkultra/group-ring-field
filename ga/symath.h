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
      static const Sym one() { return Sym(1,1); }
      /// 0 the additive identity, multiplicitive null
      static const Sym zero() { return Sym(0,1); }

     protected:
      //-----------------------------------------------------------------------------
      ///   DATA: Operator & Operands
      ///  Name/symbolic-quantity is stored in the base_type
      std::string   _op;       /// operator
      std::string   _value;
      SymSP         _left;     /// left hand side
      SymSP         _right;    /// right hand side
      int           _numerator;  /// a integral value
      int           _denominator;  /// a integral value 

      /// Twin of this symbol, allows us to have unique symbols in an expression,
      /// helps minimize copies while expressions are being built.
      mutable SymSP         _doppleganger;   
      mutable bool          _is_doppleganger;  // was object created on heap?
  
     public:
      //-----------------------------------------------------------------------------
      /// default constructor, undefined symbol defaults to "1" (multiplicitive identity)
     Sym() 
	: _value("#"), _op(), _left(0), _right(0),
	 _numerator(1), _denominator(1),
	 _doppleganger(0), _is_doppleganger(false)
      {}

      //-----------------------------------------------------------------------------
      /// rational constructor, #a/#b
      Sym( int numerator, int denominator = 1 ) 
	: _value("#"), _op(), _left(), _right(),
	_numerator(numerator), _denominator(denominator),
	_doppleganger(0), _is_doppleganger(false) 
      {
	 if (_denominator < 0)
	 {
	    _numerator = -_numerator;
	    _denominator = -_denominator;
	 }
      }
           
      //-----------------------------------------------------------------------------
      /// Create a variable : Sym a1("a1");  
     explicit Sym( const std::string& symbol ) 
	 : _value(symbol), _op(), _left(0), _right(0),
	 _numerator(-0), _denominator(-0),
	 _doppleganger(0), _is_doppleganger(false)
      {
	// For some bizzare reason, the compiler will choose this constructor rather than
	// the integer version when you call Sym(0) or Sym(1) from a templated class... WTF?
	if (symbol == "0") {
	  _value = "#";
	  _numerator = 0;
	  _denominator = 1;
	}
	if (symbol == "1") {
	  _value = "#";
	  _numerator = 1;
	  _denominator = 1;
	}
	assert(symbol != "-1");
	assert(!symbol.empty());
      }
      
      //-----------------------------------------------------------------------------
      //
      /// expression tree-node constructor
      //
     Sym( const std::string & op,                  ///< operation (intrisic symbol-values are NOP)
	  SymSP                 left,              ///< left operand (0 if not relevant)
	  SymSP                 right = SymSP(0) ) ///< right operand (0 if not relevant) 
	: _value(), _op(op), _left(left), _right(right),
	 _numerator(-0), _denominator(-0),
	 _doppleganger(0), _is_doppleganger(false)
      {
	 assert(!op.empty());
      }
      
      //-----------------------------------------------------------------------------
      /// copy constructor, shallow copy
     Sym( const Sym &s ) 
	: _value(s._value), _op(s._op), _left(s._left), _right(s._right),
	 _numerator(s._numerator), _denominator(s._denominator),
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
	_numerator = s._numerator;
	_denominator = s._denominator;
	if ( s._is_doppleganger )
	  _doppleganger = s.copyMaybe();
	return *this;
      }
      
      /// get symbol expression string
      const std::string &getValue() const { return _value; }

      //-----------------------------------------------------------------------------
      
      static bool numberCheck(const std::string& val) {  // Just check for "#"
	 return (!val.empty()) && (val.at(0) == '#');
      }

      // It's tricky to tell the difference between minus and negate, these help
      bool isNegateOp() const { return (this->_op == "-" && _left && !_right); }
      bool isMinusOp() const { return (this->_op == "-" && _left && _right); }
      bool isZero() const { return isRationalValue() && getNumerator() == 0; }
      bool isOne() const { return isRationalValue() && getNumerator() == getDenominator(); }
      bool isNegOne() const { return isRationalValue() && getNumerator() == -getDenominator(); }
      bool isVariable() const { return isLeaf() && _op.empty() && !numberCheck(getValue()); }
      bool isRational() const { return numberCheck(getValue()); }
      bool isRationalValue() const { return isRational() || (isNegateOp() && _left->isRationalValue()); }
      bool isLeaf() const { return _right == 0 && _left == 0; }

      /// Accessors for rational numbers
      int getNumerator() const { 
	 if ( this->isNegateOp() && _left->isRationalValue() )
	    return -_left->getNumerator();   // note the numerator will get the negate
	 if ( this->isRational() ) return _numerator;
	 return 1;
      }
      int getDenominator() const {
	 if ( this->isNegateOp() && _left->isRationalValue() )
	    return _left->getDenominator();  // no negate, we gave it to numerator.
	 if ( this->isRational() ) return _denominator;
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
         
	 if ( this->isRationalValue() )
	 {
	    return Sym(-getNumerator(), getDenominator());
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
	 // 0 * x = 0
	 if ( this->isZero() || s.isZero() )  
	 {
	    return zero();
	 }   
	 // rationals 
	 // a/b * c/d = (ac)/(bd)
	 if ( this->isRationalValue() && s.isRationalValue() )
	 {
	   //std::cout << "detected rational: " << this->toString() << "," << s.toString()  << std::endl;
	    return Sym( this->getNumerator() * s.getNumerator(), this->getDenominator() * s.getDenominator() );
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
	 return Sym( TIMES, 
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
	       return Sym( this->getNumerator() * rdenom  -  ldenom * s.getNumerator(), ldenom * rdenom );
	    return Sym( this->getNumerator() - s.getNumerator(), ldenom );
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
	    std::cerr << "Sym operator/() dividion by zero!!!!!!!! (" 
		      << this->toString() << " / " << s.toString() << std::endl;
	    return Sym("NaN");
	 }

	 if ( s.isRationalValue() )
	 {
	    return *this * Sym(s.getDenominator(), s.getNumerator());
	 }
	 
	 // -a/-b  = a/b
	 if ( this->isNegateOp() && s.isNegateOp() ) 
	 {
	    return *_left / *s._left;
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
	       return Sym( this->getNumerator() * rdenom  +  ldenom * s.getNumerator(), ldenom * rdenom );
	    return Sym( this->getNumerator() + s.getNumerator(), ldenom );
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
	    if ( this->isRationalValue() && s.isRationalValue() )
	       return this->getNumerator() * s.getDenominator() == s.getNumerator() * this->getDenominator();
	    if ( this->isRationalValue() || s.isRationalValue() ) return false;
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
	 if ( isRationalValue() ) // we have one or more negates in front of a number
	 {
	    return Sym( this->getNumerator(), this->getDenominator() );
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
	    return - _left->distribute();
	 }

	 if ( _op == TIMES )  // l*r
	 {
	    Sym left = _left->distribute();
	
	    if ( left._op == PLUS )  // (a + b) * c  --> (a*c) + (b*c)
	       return ((*left._left * *_right) + (*left._right * *_right)).distribute();
	
	    if ( left.isMinusOp() )  // (a - b) * c  --> (a*c) - (b*c)
	       return ((*left._left * *_right) - (*left._right * *_right)).distribute();

	    Sym right = _right->distribute();

	    if ( right.isRational() )  // Move rational to the left.
	       return right * left;
	    
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
	    return left * right;
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

	    if (right.isRational())
	       return (one()/right) * left;
	    
	    if ( right._op == DIV ) //  a / (b / c) --> (a*c)/b
	       return (( left * *right._right ) / *right._left).distribute();
	
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

	 return *this;
	 // return Sym(_op,
	 //	    _left ? _left->makeAdditive().copyMaybe() : NULL,
	 //	    _right ? _right->makeAdditive().copyMaybe() : NULL);
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
	 // loop over additive expressions
	 for (SymPVecIter aiter = addexps.begin(), aend = addexps.end(); aiter != aend; ++aiter)
	 {
	    // for each +subexp, gather multiplicitve subexpressions into flat list
	    SymPVec multexps;
	    bool neg = (*aiter)->getMultiplicitiveSubexps(&multexps);
	    if (multexps.size() > 1)  // more than one subexpression
	    {
	       // loop over multiplicitive expressions
	       for (SymPVecIter miter = multexps.begin(), mend = multexps.end(); miter != mend; ++miter)
	       {
		  // recurse, sorted these sub expressions first
		  (*miter) = (*miter)->sortedForm(false).copyMaybe();
	       }
	       // sort * subexps
	       multexps.sort(sym_ptr_comp);

	       // rebuild multiplicitive expression tree from sortedd subexps
	       Sym mexp(one());
	       if ( neg )	    // don't forget to return the negative we stripped off
		  mexp = -mexp; // keep negates as close to leaves as possible
	       for (SymPVecIter miter = multexps.begin(), mend = multexps.end(); miter != mend; ++miter)
	       {
		  mexp = mexp * *(*miter);
	       }
	       // replace expression in list
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

      // +/- rational scale factor applied to an expression ex: 2*a*3/-2 --> -3
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
      // distributes, sorts, and combines expressions.
      //  (a + b) * (b + a) --> a*a + 2*a*b + b*b
      // the == compare operator should return true when any pair if equivalent expressions
      // are both in normal form.  ie   -a * (b - a) + 3*a*b - b*-b --> a*a + 2ab + b*b
      Sym normalForm(bool sorted_form = true) const
      {
	 Sym std_form = sorted_form ? this->sortedForm() : (*this);
	 // no additive subexpressions to cancel at this level, recurse on subexpressions.
	 if ( ! (std_form._op == PLUS || std_form.isMinusOp()) )
	 {
	   if (std_form._left || std_form._right) {  // carefull to never copy leaf nodes.
	     if (std_form._op == TIMES) {
	       return std_form._left->normalForm() * std_form._right->normalForm();
	     }
	     //std::cout << "creating symbol for " << this->toString() << ",";
	     return Sym(std_form._op, 
			std_form._left ? std_form._left->normalForm().copyMaybe() : NULL,
			std_form._right ? std_form._right->normalForm().copyMaybe() : NULL);
	   }
	   return *this;
	 }

	 SymPVec subexps;
    
	 /// grab all sub expression connected by addition (commute trivially)    
	 std_form.getAdditiveSubexps( &subexps );
       
	 if ( subexps.size() <= 1 ) std::cerr << "badness in cancel" << std::endl;
  
	 /// double loop, summs up the multiplicity of repeated subexpressions, eliminates those that sum to 0
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
	    assert(left_multiple.isRational());
	    Sym left = (left_exp / left_multiple).sortedForm();

	    Sym new_multiple = left_multiple; 

	    SymPVecIter spviB = spviA;
	    //std::cout << " doing " << left_exp.toString() << ", " << left_multiple.toString() << " base: "  << left.toString()
	    //	      << std::endl;;
	    ++spviB;
	    while ( spviB != subexps.end() ) // check against every other remaining expression
	    {
	       if ( !(*spviB) ) std::cerr << " BOMB " << std::endl;
	       Sym& right_exp = **spviB;
	       Sym right_multiple = right_exp.GetMultiple();
	       assert(right_multiple.isRational());
	       /// is 0, remove
	       if ( right_exp.isZero() || right_multiple.isZero() )
	       {
		  subexps.erase(spviB++);
		  continue;
	       }
	       Sym right = (right_exp / right_multiple).sortedForm();
	       //std::cout << right.toString() << " ";
	       if ( left == right ) // A match!
	       {
		  //std::cout << "== ";
		  // sum their multiples:  3a + (1/3)a == 10/3a
		  new_multiple = new_multiple + right_multiple;
		  // delete duplicate expression
		  subexps.erase(spviB++);
		  continue;
	       }
	       ++spviB;
	    } /// end while B
	    //std::cout << std::endl;
	    // build the new summed expression. Don't introduce products with 0, 1 or -1
	    if ( new_multiple.isZero() ) 
	    {  // we can remove left, there are none
	       subexps.erase(spviA++);
	       continue;
	    } 
	    else if ( new_multiple == Sym(-1) )
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
	    // recurse on sub-expressions, no need to sorted again.
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
	    else if ( sym->isRational() )
	    {
	       std::stringstream ss;
	       ss << _numerator;
	       if (_denominator != 1)
		  ss << "/" << _denominator;
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
	 else if ( isRational() )
	 {
	    std::stringstream ss;
	    ss << _numerator;
	    if (_denominator != 1)
	       ss << "/" << _denominator;
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
	 else if ( this->isRational() && !isNegateOp() )
	 {
	    os << _numerator;
	    if (_denominator != 1)
	       os << "/" << _denominator;
	 }
	 else if ( !isNegateOp() && !getValue().empty())  os << getValue();
	 else if ( !isNegateOp() && getValue().empty() ) os << "?<" << _op << "|" << _numerator << "/" << _denominator << ">";

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
