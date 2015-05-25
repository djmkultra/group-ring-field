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
   
   const std::string WTF("??");
   const std::string ONE("1");
   const std::string ONE_("1");
   const std::string NEG_ONE("-1");
   const std::string ZERO("0");
   const std::string NEG_ZERO("-0");
   const std::string MINUS("-");
   const std::string NEG("-");
   const std::string PLUS("+");
   const std::string TIMES("*");
   const std::string DIV("/");
   const std::string POW("^");
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

   template <class BASE_T = std::string, class OPER_T = std::string >   
   class Sym : public BASE_T, public gutz::Counted {
   public:
      /// NOTE an empty symbol means 1 (one, mult-identiy) NOT 0 (add-identity)
      
      //-----------------------------------------------------------------------------
      /// This is a sub-class of std::string, expression stored in parent
      ///   getValue(), setValue() give you easy access, so as to express yourself 
      typedef BASE_T           base_type;
      typedef BASE_T           value_type;
      typedef OPER_T           operator_type;
      typedef Sym<BASE_T>      this_type;
      //typedef Sym            this_type;
      
      //-----------------------------------------------------------------------------
      /// container: vector of symbol smart pointers
      //
      typedef typename gutz::SmartPtr< this_type >   SymSP;
      typedef typename std::vector< SymSP >          SymPVec;
      typedef typename SymPVec::iterator             SymPVecIter;
      
      //-----------------------------------------------------------------------------
      ///   DATA: Operator & Operands
      ///  Name/symbolic-quantity is stored in the base_type
      mutable operator_type _op;    /// operator
      mutable SymSP         _left;     /// left hand side
      mutable SymSP         _right;    /// right hand side
      
      //-----------------------------------------------------------------------------
      /// default constructor
      Sym() 
      : base_type(WTF), _op(WTF), _left(0), _right(0)
      {}
      
      //-----------------------------------------------------------------------------
      /// c-style string constructor
      Sym( const char *c_str ) 
      : base_type(c_str), _op(NOP), _left(0), _right(0)
      {
         /// default behavior:
         ///   set name-string to the null-terminated cstring
         ///   unless the values are 0, 1, -1
         if( c_str == (char*)0 ) setValue(ZERO);
         else if( c_str == (char*)-1 )
         {
            setValue(NEG);
         }
         else if( c_str == (char*)1 || getValue() == ONE ) 
            setValue(ONE_);
         else if( getValue() == NEG_ONE )
         {
            setValue(NEG_ONE);
         }
         
      }
      
      //-----------------------------------------------------------------------------
      /// char constructor (checks if you use the char for numeric values -1, 0, 1
      Sym( const char c ) 
      : base_type(), _op(NOP), _left(0), _right(0) 
      { 
         if( c == -1 )
         {
            setValue(NEG);
         }
         else if( c == 0  )
         {
            setValue(ZERO);
         }
         else if( c != 1 )
         {
            std::string charstr; charstr += c;
            setValue( charstr );
         }
      }
      
      //-----------------------------------------------------------------------------
      /// integer constructor, number-Syms
      Sym( int i ) 
      : base_type(), _op(NOP), _left(0), _right(0) 
      { 
         if( i ==  0 )      setValue(ZERO); 
         else if( i == -0 ) setValue(ZERO);
         else if( i == -1 ) setValue(NEG);
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
      Sym( const base_type     & val,         ///< expression string (all up to this node)
           const operator_type & op    = NOP, ///< operation (intrisic symbol-values are NOP)
           SymSP                 left  = SymSP(0),   ///< left operand (0 if not relevant)
           SymSP                 right = SymSP(0)    ///< right operand (0 if not relevant)
         ) 
      : base_type(val), _op(op), _left(left), _right(right) 
      {}
      
      //-----------------------------------------------------------------------------
      /// copy constructor
      explicit Sym( const Sym &s ) 
      : base_type(s), _op(s._op), _left(0), _right(0)
      {
         if( s._left  ) _left  = s._left->clone();
         if( s._right ) _right = s._right->clone();
      }
      
      //-----------------------------------------------------------------------------
      /// destructor
      ~Sym()
      {
      }
      
      
      //-----------------------------------------------------------------------------
      ///     CLONE : don't call new, eventually this will do...
      /// deep copy, products (can) get freaky when compounded 
      ///   example A=B*C  C=A*A (bad since A has existing trees inside!) C=A-A + A*A (worse)
      SymSP clone() const 
      {
         SymSP ret;
         ///    TODO: got pretty close to hacking smartptr and counted into detecting 
         ///          stack versus heap variable creation.  you should ever point to stack objs
         ///
         //if( gutz::Counted::isStackVar() ) ///< never been owed by a smart-ptr, likely stack obj
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
	 // already done in copy constructor!
         //if( _left )
	 //ret->_left = _left->clone();
         //if( _right ) 
	 // ret->_right = _right->clone();
         
         return ret;
      }
      
      //-----------------------------------------------------------------------------
      /// assignment operator, shallow copy, shared pointers
      Sym &operator=(const Sym &s)  
      { 
         setValue(s); 
         _op = s._op;
         if(s._left)  _left = s._left->clone();
	 else _left = 0;
         if(s._right) _right = s._right->clone();
	 else _right = 0;
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
         if( !tree ) return;
         
         collapse( tree->_left );
         collapse( tree->_right );
         tree->_left = 0;
         tree->_right = 0;
      }
      
      //-----------------------------------------------------------------------------
      /// -(*this)  NEGATE uinary
      Sym operator-() const   ///< NEGATE uninary
      { 
         
         /// -0
         if( getValue() == ZERO || getValue() == NEG_ZERO )  
            return Sym(ZERO);
         
         /// -1
         if( isEmpty(getValue()) || getValue() == ONE || getValue() == ONE_ )
         {
            return Sym( NEG + ONE , 
			NEG,
			new Sym(ONE));
         }
         
         /// --A -> A
         if( this->_op == NEG ) // double negative
         {
            if( _left )
            {
               return Sym( *_left );
            }
            else 
            {
               std::cerr << "bad operator-()" << std::endl;
            }
         }
         
         /// new node - , A
         return Sym( NEG + getValue(),
                     NEG,
                     new Sym(ONE) 
                   );
      }
      
      bool isEmpty(const std::string& val) const {
	return val.empty();
      }
      bool isEmpty(double val) const {
        return false;
      }

      //-----------------------------------------------------------------------------
      /// this * s  MUL
      Sym operator*( const Sym &s ) const   ///< MULTIPLICATION
      {  
         base_type l = getValue();
         base_type r = s.getValue();
         
         if ((isEmpty(l) || l == ONE_ || l == ONE) && (isEmpty(r) || r == ONE || r == ONE_)) {
	   return Sym(ONE);
	 }
	 
         if( l == ZERO || l == NEG_ZERO || r == ZERO || r == NEG_ZERO )
         {
            return Sym(ZERO);
         }   
         
         if( (l == NEG) && (_left == 0)  && !isEmpty(r) && (r != ONE_) && (r != ONE) && (s._op != NEG) )  /// fresh neg
         {
            
            return Sym( getValue() + r, 
                          NEG, 
                          s.clone() 
                          );
         }
         
         if( isEmpty(l) || l == ONE_ || l == ONE )
         {
            return s;
         }
         
         if( isEmpty(r) || r == ONE_ || r == ONE ) 
         {
            return Sym(*this);
         }
         
         /// RETURN TO POSITIVE
         /// -l * -r
         if( _op == NEG && s._op == NEG )  /// both neg (now pos)
         {
            l = l.substr(1);
            r = r.substr(1);
            if( isEmpty(l) || l == ONE || l == ONE_ ) 
            {
               if( s._left )  // maybe there was a fresh neg to your right???
                  return Sym(*(s._left));
               std::cout << " wtf?  " << getValue() << " * " << s.getValue() << std::endl;
               return Sym( ONE_ );
            }
            if( isEmpty(r) || r == ONE || r == ONE_ )
            {
               if( _left )
               {
                  return Sym( *(this->_left) );
               }
               
               return Sym( ONE_ );
            }
            
            if( !s._left || !_left )
               std::cout << " fcuk * bad " << std::cout;
            
            
            /// swap order, both neg, use left from each
            if( r < l && s._left && !_left )
            {
               return Sym( r + TIMES + l, 
                             TIMES, 
                             s._left->clone(), 
                             _left->clone() 
                             );
               
            }
            
            return Sym( l + TIMES + r, 
                          TIMES, 
                          _left->clone(), 
                          s._left->clone() 
                          );
            
         }
         
         if( _op == NEG ) // left side is negative
         {
	   if( (l.substr(1) == ONE || l.substr(1) == ONE_ || isEmpty(l.substr(1))) && (r == ONE || r == ONE_ || isEmpty(r))) {
	     return Sym( NEG + ONE,
			 NEG,
			 new Sym(ONE));
	   }

            if( l.substr(1) > r )
            {
               return Sym( NEG + r + TIMES + l.substr(1), 
			   NEG, 
			   new Sym( r + "*" + l.substr(1), 
					  TIMES, 
					  s.clone(), 
					  _left->clone() 
					  )
			   );
            }
	    else if ( r == ONE || r == ONE_ || isEmpty(r)) {
	      return Sym( NEG + ONE,
			  NEG,
			  new Sym(ONE));
	    }
            //else this is already a neg node, replace left and your done
	    SymSP left = _left->clone();
	    SymSP right = s.clone();
	    SymSP out = new Sym( l.substr(1) + "*" + r, 
				 TIMES, 
				 left, 
				 right 
				 );

            return Sym( l + TIMES + r, 
			NEG, 
			out
			);
	    
         }
         if( s._op == NEG && s._left )  //right hand side is negative
         {
	   if( (r.substr(1) == ONE || r.substr(1) == ONE_ || isEmpty(r.substr(1))) && (l == ONE || l == ONE_ || isEmpty(l))) {
	       return Sym( NEG + ONE, NEG, new Sym(ONE));
	     }
            if( r.substr(1) < l )
               return Sym( r + "*" + l,
                             NEG, 
                             new Sym( r.substr(1) + "*" + l, 
                                        TIMES, 
                                        s._left->clone(), 
                                        this->clone() 
                                        ) 
                             );
            else 
               return Sym( NEG + l + "*" + r.substr(1), 
			   NEG, 
                             new Sym( l + "*" + r.substr(1), 
                                        TIMES, 
                                        this->clone(), 
                                        s._left->clone() 
                                        ) 
                             );
         }
         
         
         if( getValue() == s.getValue() )  /// power TODO: shoud be a full sub-tree check... can't really rely on strings
         {
            return Sym( l + POW + base_type("2"),
                          POW,
                          this->clone(),
                          new Sym( base_type("2") )
                          );
         }
         
         if( s.getValue() < getValue() )
         {
            return Sym( r + "*" + l, 
                          TIMES, 
                          s.clone(), 
                          this->clone() 
                          );
         }
         
         
         return Sym( l + "*" + r, 
                       TIMES, 
                       this->clone(), 
                       s.clone() 
                       );
      }
      //-----------------------------------------------------------------------------
      //
      Sym &operator*=( const Sym &s )
      {
         return *this= *this * s;;
      }
      
      //-----------------------------------------------------------------------------
      Sym operator-( const Sym &s ) const 
      {
         /// a-a=0
         if( getValue() == s.getValue() )   return Sym(ZERO);
         /// 0-a=-a
         if( getValue() == ZERO )            return -s;
         /// a-0=a
         if( s.getValue()     == ZERO )      return *this;
         /// a--b = a+b
         if( s._op == NEG && s._left )
         {
            return *this + Sym(*s._left); 
         }
         /// a-b
         base_type expr = OPR +  getValue() + MINUS + s.getValue() + CPR;
         return Sym(expr, 
                       MINUS, 
                       this->clone(), 
                       s.clone()
                      ); 
      }
      //-----------------------------------------------------------------------------
      Sym &operator-=( const Sym &s )
      {
         return *this = *this - s;
      }
      
      //-----------------------------------------------------------------------------
      Sym operator/( const Sym &s ) const 
      {
         if( s.getValue() == ONE ) return (*this);
         if( getValue() == ZERO )
         {
            s.clear();
            return Sym(ZERO);
         }
         if( s.getValue() == ZERO )
         {
            std::cerr << "Sym operator/() dividion by zero!!!!!!!!" << std::endl;
            clear();
            return Sym(ZERO);
         }
         
         if( base_type::empty() )
         {
            return Sym( base_type("1/") + s.getValue(), 
                          DIV, 
                          new Sym(ONE), 
                          s.clone() 
                         );
         }
         
         return Sym( getValue() + "/" + s.getValue(), 
                       DIV, 
                       this->clone(), 
                       s.clone() 
                       ); 
      }
      //-----------------------------------------------------------------------------
      Sym &operator/=( const Sym &s )
      {
         return *this = *this / s;
      }
      
      //-----------------------------------------------------------------------------
      /// this + s   ADD
      Sym operator+( const Sym &s ) const   ///< ADDITION 
      {  
         
         base_type l = getValue();
         base_type r = s.getValue();
         
         /// 0 + s
         if( l == ZERO || l == NEG_ZERO ) 
         { 
            return s; 
         }
         /// this + 0
         if( r == ZERO || r == NEG_ZERO )  return *this;
         
         /// 1s
         if( r.empty() ) r = ONE;
         if( l.empty() ) l = ONE;
         
         /// +-this += -s
         if( s._op == NEG ) //neg right
         {
            /// this += -s == 0? 
            if( _op != NEG )
            {
               /// CANCELATOIN OF IDENTICAL TERMS: see if we cancel and become zero
               if( l == r.substr(1) )  /// left 
               {
                  return Sym(ZERO);
               }
            }
         }
         else /// +-this += s
         {
            /// -this += s == 0?
            if( _op == NEG ) // neg left
            {
               /// CANCELLATION OF IDENTICAL TERMS chcek to see if we cancel
               if( l.substr(1) == r )  // see if a-a = 0
               {
                  return Sym(ZERO);
               }
            }
         }
         
         /// Both sides are relevant
                  
         /// always store in ascending sort order (for communitive ops anyway)
         if( r < l )
         {
            return Sym( OPR + s.getValue() + PLUS + getValue() + CPR,
                           PLUS,
                           s.clone(),
                           this->clone()
                          );
         }
         else 
         
         return Sym( OPR + getValue() + PLUS + s.getValue() + CPR, 
                       PLUS, 
                       this->clone(), 
                       s.clone() 
                       );
      }
      
      
      //-----------------------------------------------------------------------------
      Sym &operator+=( const Sym &s ) 
      { 
         base_type l = getValue();
         base_type r = s.getValue();
         
         /// 0 += s
         if( l == ZERO || l == NEG_ZERO ) 
         { 
            *this = s; 
            return *this; 
         }
         /// this += 0
         if( r == ZERO || r == NEG_ZERO )  return *this;
         
         /// 1s
         if( r.empty() ) r = ONE;
         if( l.empty() ) l = ONE;
         
         /// +-this += -s
         if( s._op == NEG ) //neg right
         {
            /// this += -s == 0? 
            if( _op != NEG )
            {
               /// CANCELATOIN OF IDENTICAL TERMS: see if we cancel and become zero
               if( l == r.substr(1) )  /// left 
               {
                  clear();
                  s.clear();
                  setValue(ZERO);
                  return *this;
               }
            }
         }
         else /// +-this += s
         {
            /// -this += s == 0?
            if( _op == NEG ) // neg left
            {
               /// CANCELLATION OF IDENTICAL TERMS chcek to see if we cancel
               if( l.substr(1) == r )  // see if a-a = 0
               {
                  clear();
                  setValue(ZERO);
                  s.clear();  
                  return *this;
               }
            }
         }
         
         /// Both sides are relevant
         
         SymSP me = this->clone();
         SymSP rt = s.clone();
         
         
         /// always store in ascending sort order (for communitive ops anyway)
         if( r < l )
         {
            setValue( OPR + r + PLUS + l + CPR );
            _op    = PLUS;
            _left  = rt;
            _right = me;
         }
         else 
         {
            setValue( OPR + l + PLUS + r + CPR );
            _op       = PLUS;
            _left     = me;
            _right    = rt;
         }
         
         return *this;
      }
      
      //-----------------------------------------------------------------------------
      //-----------------------------------------------------------------------------
      bool operator<( int i ) const 
      { 
         if( getValue()[0] == '-' ) return true; 
         std::stringstream iss;
         iss << i;
         if( getValue() < iss.str() ) return true;
         return false; 
      }
      //-----------------------------------------------------------------------------
      bool operator<( const Sym &s )
      {
         return getValue() < s.getValue();
      }
      //-----------------------------------------------------------------------------
      operator bool() const 
      { 
         if( _op == ZERO || getValue() == ZERO || getValue() == NEG_ZERO ) 
            return false;  
         return true; 
      }
      //-----------------------------------------------------------------------------
      bool operator==( const Sym &s ) const 
      { 
         return getValue() == s.getValue(); 
      }
      //-----------------------------------------------------------------------------
      bool operator==( const value_type &s ) const 
      { 
         return getValue() == s; 
      }
      
      
      //-----------------------------------------------------------------------------
      //-----------------------------------------------------------------------------
      bool isLeaf() const { return (_right == 0 && _left == 0 ); }
      
      //-----------------------------------------------------------------------------
      void additiveSubexps( SymPVec &spv, int level = 0 )
      {
         if( _op != PLUS )
         {
            spv.push_back( this );
            if( ! level )
               return;
            /// todo extract deeper levels
         }
         if( _left  ) _left->additiveSubexps( spv );
         if( _right ) _right->additiveSubexps( spv );
      }
      
      //-----------------------------------------------------------------------------
      /// if any nodes _op == ZERO, apply them
      void cancelZeros()
      {
         
         /// This node is zero  0
         if( this->_op == ZERO || getValue() == ZERO )
         {
            nuke();
            this->_op = ZERO;
            return;
         }
         
         if( _left == 0 )  ///< quick check for leafyness
         {
            return;
         }
         
         /// negate zero  -0
         if( _op == NEG && _left->_op == ZERO )
         {
            clear();
            _op = ZERO;
            return;
         }
         
         /// multiply with a zero  0 * r or l * 0
         if( _op == TIMES && (_left->_op == ZERO || ( _right && _right->_op == ZERO)))
         {
            nuke();
            setValue(ZERO);
            _op = ZERO;
         }
         /// add with left 0     0 + r
         else if( _op == PLUS && (_left->_op == ZERO && (_right && _right->_op != ZERO)) )
         {
            _left->nuke();
            _left=0;
            Sym *save_right = _right;
            (*this) = (*save_right);
         }
         /// add with right 0     a + 0
         else if( _op == PLUS && (_left->_op != ZERO && (_right && _right->_op == ZERO)) )
         {
            _right->nuke();
            Sym *left = _left;
            _right = 0;
            (*this) = (*left);
         }   
         /// add with both 0      0 + 0
         else if( _op == PLUS && (_left->_op == ZERO && (_right && _right->_op == ZERO)) )
         {
            nuke();
            setValue(ZERO);
            _op = ZERO;
         }
         /// TODO: sub, div, etc....
         
         ///    RECURSION    ---------------------------------
         if( _left  ) _left->cancelZeros();
         if( _right ) _right->cancelZeros();
         /// -------------------------------------------------
         
         /// multi with a zerp or nothing   0/nil/a * 0/nil/a
         if( _op == TIMES && ( (!_left || _left->_op == ZERO) || ( !_right || _right->_op == ZERO) ) )
         {
            nuke();
            setValue( ZERO );
            _op = ZERO;
         }
         /// add with zeros or nothings     0/nil + 0/nil
         else if( _op == PLUS && ( (!_left || _left->_op == ZERO) && (!_right || _right->_op == ZERO)) )
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
         if( _op != PLUS ) return *this;
         SymPVec subexps;
         /// brab all sub expression connected by addition (commute trivially)
         ///   recursive
         additiveSubexps( subexps );
         
         /// double loop checking to see if any will cancel each other
         ///   if they do, set the _op to Zero...
         for( SymPVecIter spviA = subexps.begin(); spviA != subexps.end(); ++spviA )
         {
            if( (*spviA)->_op == ZERO )
               continue;
            
            for( SymPVecIter spviB = spviA + 1; spviB != subexps.end(); ++spviB )
            {
               /// right is 0
               if( (*spviB)->_op == ZERO )
                  continue;
               
               if( (*spviA)->_op == NEG && (*spviB)->_op != NEG )
               {
                  ///      -left + right = 0
                  if(  base_type(*(*spviA)).substr(1) == base_type(*(*spviB)) )
                  {
                     (*spviA)->_op = ZERO;
                     (*spviA)->setValue( ZERO );
                     (*spviB)->_op = ZERO;
                     (*spviB)->setValue( ZERO );
                  }
               }
               else if( (*spviA)->_op != NEG && (*spviB)->_op == NEG )
               {
                  ///      left + -right = 0
                  if( base_type(*(*spviA)) == base_type(*(*spviB)).substr(1) )
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
      void printTree(std::ostream &os, const std::string &indent = std::string("")) const
      {
         const std::string indent_sz("  ");
         
         /// negavie node, I want negs printed in-line  -*  or - A * B
         if( _op == NEG )
         {
	   if( ! _left ) {
	     std::cout << "wtf" ;
	     return;
	   }
            Sym *sym = _left;
            
            /// double NEG??  This should NOT happen if tree was built properly!
            if( sym->_op == NEG )
            {
               /// yes double neg at a leaf node
               if(  sym->_left && sym->_left->isLeaf()  && !sym->_right )
               {
                  os << indent << "--+" << *(sym->_left) << "\n";
                  return;
               }
               /// subtraction op miss named
               else if( sym->_left && sym->_right )
               {
                  if( sym->_left->isLeaf() && sym->_right->isLeaf() )
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
                  if( sym->_left )
                  {
                     sym->_left->printTree(os,indent+indent_sz);
                     
                     /// ah ha! we are miss named negation!
                     if(_right)
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
               return;
            } /// END DOULBE NEGATIVE
            
            /// basic leaf neg
            if( sym->isLeaf() )
            {
               os << indent << "-" << (*sym) << "\n";
               return;
            } 
            
            /// operation leaf neg:  - A * B
            if( sym->_op != NOP && sym->_left && sym->_left->isLeaf() && sym->_right && sym->_right->isLeaf() )
            {
               os << indent << "-( " << *(sym->_left) << " " << sym->_op << " " << *(sym->_right) << " )\n";
               return;
            }
            /// operation node neg
            if(sym->_op != NOP && sym->_op != NEG )
            {
               if( sym->_right ) sym->_right->printTree( os, indent + indent_sz + indent_sz );
               os << indent << "-" << sym->_op << "\n";
               if( sym->_left  ) sym->_left->printTree( os, indent + indent_sz + indent_sz);
               return;
            }
            else /// what else could there be???
            {
               os << indent << "-? (" << (*this) << ")\n";
               return;
            }
            
         } /// END IF NEG NODE
         
         /// leaf operation  print out inline:   A * B  
         if( _op != NOP  && _right && _right->isLeaf() && _left && _left->isLeaf() )
         {
            os << indent << " ( " << (*_left) << " " << _op << " " << (*_right) << " )\n";
            return;
         }
         
         if( _right )
         {
            _right->printTree( os, indent + indent_sz );
            //os << "\n";
         }
         if( _op != NOP )  os << indent << _op << "\n";
         else if( !base_type::empty() ) os << indent << (*this) << "\n";
         else os << indent << "1\n";
         if( _left  )
            _left->printTree( os, indent + indent_sz );
         
      }
      
      void printInfix( std::ostream &os, const base_type &last_op = NOP ) const
      {
         bool parens = true;
         if( last_op == NOP || last_op == PLUS || (!_left && !_right) ) parens = false; 
         if( _op == TIMES && last_op == NEG ) parens = false;
         
         if( parens ) os << OPR;
         if( _op == NEG ) os << NEG;
         if( _left )  _left->printInfix( os, _op );
         if( _op != NOP && _op != NEG ) os << _op;
         else if( _op != NEG )             os << getValue();
         if( _right ) _right->printInfix( os, _op );
         if( parens ) os << CPR;
         
      }
      
      
      //-----------------------------------------------------------------------------
      /// standard ostream << printer
      std::ostream &operator<<( std::ostream &os ) const
      {
         printInfix(os, TIMES);
         return os;
         
         if( _op == PLUS ) os << "(";
         if( _op == NEG )  os << "-";
         if( _left ) _left->operator<<(os);
         if( _op != NOP ) os << _op;
         else             os << getValue();
         if( _right ) _right->operator<<(os);
         if( _op == PLUS ) os << ")";
         return os;
      }
      
   };
   
   typedef Sym<>   Symbol;
   
   const  Symbol   one(ONE);
   const  Symbol   zero(ZERO);
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
//// specialized the output for symbol since for whatever reason I made the empty symbol 1
std::ostream &operator<<( std::ostream &os, const Symath::Symbol &s  )  
{
   return s.operator<<( os );
   
   
   if( s.empty() ) os << "1";
   else 
   {
      os << std::string(s);
   }
   return os;
}



#endif
