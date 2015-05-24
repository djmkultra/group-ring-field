///////////////////////////////////////////////////////////////////////////
//              _____________  ______________________    ^    ----  _
//             /  ________  |  |   ___   ________   /   / \  /    \ |
//            |  |       |  |_ |  |_ |  |       /  /   /   \|       |
//            |  |  ___  |  || |  || |  |      /  /   / --- \   --- |
//            |  | |   \ |  || |  || |  |     /  /   /       \____/ |_____|
//            |  | |_@  ||  || |  || |  |    /  /          
//            |  |___/  ||  ||_|  || |  |   /  /_____________________
//             \_______/  \______/ | |__|  /___________________________
//                        |  |__|  |
//                         \______/
//                 University of New Mexico       
//                           2010
///////////////////////////////////////////////////////////////////////////

#ifndef _COMMAND_H
#define _COMMAND_H

#include <iostream>
#include <signalGutz.h>
#include <typeinfo>
#include <smartptr.h>
#include <list>
//#include <thread.h>

namespace gutz
{
   //-------------------------------------------------------------------------
   ///@{ strip_ref_ptr
   /// strip off any reference or pointer types, allows copy only transfer of values between threads
   
   template< typename T >
   class strip_ref_ptr 
   {
      /// Objects/Classes/Structs/Basic_Types (not a pointer or reference)
   public:
      typedef T        the_stripped_type;  ///< type of object when we remove pointer or ref
      typedef T        the_original_type;  ///< type of object we are passing to this traits-policy
      typedef T        cross_thread_type;  ///< safe storage type for cross-thread usage
      typedef const T& const_ref_type;     ///< const T&
      typedef T&       ref_type;           ///< T&
      
      /// copy the value: forces a copy
      static the_original_type copyValue(const_ref_type value ) { return value; }
      
      /// copy the stripped type, sometimes copies
      ///    for basic types (non-thread-safe-types) value is copied and stripped type is returned
      ///    if the type is cross-thread-safe then the original value reference is returned
      static cross_thread_type copy_the_stripped_type(const_ref_type value) { return value; }
   };
   
   template<typename T>
   class strip_ref_ptr<T*>
   {
      /// pointer to T (T* template specialization)
   public:
      typedef T        the_stripped_type;
      typedef T*       the_original_type;
      typedef T*       cross_thread_type; ///< Ahhh memory leak danger 
      typedef T* const const_ref_type;
      typedef T*       ref_type;
      
      static the_original_type copyValue(const_ref_type value ) { return new the_stripped_type(*value); }
      static cross_thread_type copy_the_stripped_type(const_ref_type value) { return copyValue( value ); }
   };
   
   template<typename T>
   class strip_ref_ptr<T&>
   {
      /// reference to T (T& template specialization)
   public:
      typedef T       the_stripped_type;
      typedef T&      the_original_type;
      typedef T       cross_thread_type;
      typedef const T const_ref_type;
      typedef T       ref_type;
      
      static ref_type copyValue(const_ref_type value) { return the_stripped_type(value); }
      static cross_thread_type copy_the_stripped_type(const_ref_type value) { return the_stripped_type(value); }
   };
   
   
   /// arg_value<T> is a class that wraps a target argument value (for cross-thread-safe signals & commands)
   ///   If we have a basic type, arg_value<T> will copy the value (see strip_ref_ptr<T> for x-thread copy opperations).
   ///   Otherwise, if we have a "lockable container" (one that contains a mutex) then we allow copy by 
   ///   reference or pointer. You can change the behavior, i.e. add new ways to pass data without
   ///   a copy by creating the associated/corresponding strip_ref_ptr speciallization
   ///   template< class T > class strip_ref_ptr< myLockableReferenceType< T > > {...};
   ///   
   template< typename T, int IS_SELF_LOCKING=0 >
   class arg_value
   {
   public:
      typedef strip_ref_ptr<T> strip; ///< helper insures we do something smart when we go cross-thread
      typedef T the_type;  ///< the type of the value
      /// NOTE: typename required in following statement because we have a nested type
      typedef typename strip::cross_thread_type store_type;  /// how should we store the type
      typedef typename strip::const_ref_type  const_ref_type;
      typedef typename strip::ref_type ref_type;
      
      arg_value(const_ref_type value)
      :_val( strip::copy_the_stripped_type(value) )
      {}
      
      
      /// getValue() returns a reference to the value 
      ref_type getValue() { return _val; }
      
   protected:
      store_type _val;
   };
   
   // Need to specialize arg_value for self locking = 1
   
   template< typename T >
   class arg_value<T, 1> /// IS_SELF_LOCKING = 1
   {
   public:
      typedef strip_ref_ptr<T> strip; ///< helper insures we do something smart when we go cross-thread
      typedef T the_type;  ///< the type of the value
      /// NOTE: typename required in following statement because we have a nested type
      typedef typename strip::cross_thread_type store_type;  /// how should we store the type
      typedef typename strip::const_ref_type  const_ref_type;
      typedef typename strip::ref_type ref_type;
      
      arg_value(const_ref_type value)
      :_val( strip::copy_the_stripped_type(value) )
      {}
      
      
      /// getValue() returns a reference to the value 
      ref_type getValue() { return _val; }
      
   protected:
      store_type _val;
   };
   
   template<class A1=NA,class A2=NA,class A3=NA,class A4=NA,
   class A5=NA,class A6=NA,class A7=NA,class A8=NA>
   class arg_traits
   {
      typedef A1 arg1_type;   
      typedef A2 arg2_type;   
      typedef A3 arg3_type;   
      typedef A4 arg4_type;   
      typedef A5 arg5_type;   
      typedef A6 arg6_type;   
      typedef A7 arg7_type;   
      typedef A8 arg8_type;   
      
   };
   
   typedef arg_traits<>                        arg_traits0;
   //   typedef arg_traits<int> arg_traits1;
   
   //   template<typename A1>
   //   class ArgTraitsarg_traits0
   
   ///--------------------------------------------------------------------------------
	/// Pure virtual command class. This is the basis for all derived
	/// command classes
   ///--------------------------------------------------------------------------------
	class Command : public gutz::Counted
	{
	public:
		typedef gutz::SmartPtr<Command>      pointer;
      typedef std::list<pointer>           container;
      typedef std::list<pointer>::iterator iterator;
      
		Command() {}
		virtual void exec(void) = 0;
		~Command() { }
	};
	
   ///----------------------------------------------------------------------------
   /// The Command interface
   /// More concrete classes to follow
   ///----------------------------------------------------------------------------
   template<typename CE, typename F, typename A0, typename A1, typename A2, typename A3,
   typename A4, typename A5, typename A6, typename A7>
	class _CommandIF : public Command
	{
	public:	
      _CommandIF(CE* receiver, F fptr,
                 A0 a0 = NA(), A1 a1 = NA(), A2 a2 = NA(), A3 a3 = NA(),
                 A4 a4 = NA(), A5 a5 = NA(), A6 a6 = NA(), A7 a7 = NA())
      : _receiver(receiver), _fptr(fptr), _a0(a0), _a1(a1), _a2(a2), _a3(a3),
      _a4(a4), _a5(a5), _a6(a6), _a7(a7) {}
      
      virtual ~_CommandIF() {}
      virtual void exec(void) = 0;
      
      CE* _receiver;
      F   _fptr;
      A0 _a0; A1 _a1; A2 _a2; A3 _a3;
      A4 _a4; A5 _a5; A6 _a6; A7 _a7;
	};
	
   ///--------------------------------------------------------------------------------
	/// Non-specialized Command class with 8 args
   ///--------------------------------------------------------------------------------
	template<typename CE, typename F, typename A0, typename A1, typename A2, typename A3,
   typename A4, typename A5, typename A6, typename A7>
	class _Command : public _CommandIF<CE,F,A0,A1,A2,A3,A4,A5,A6,A7>
	{
	public:
		// Define the base type for easy use later
		typedef _CommandIF<CE,F,A0,A1,A2,A3,A4,A5,A6,A7> base_type;
		
		/// Constructor
		_Command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
      : base_type(receiver, fptr, a0, a1, a2, a3, a4, a5, a6, a7) {}
      
		/// Execute a command
		void exec(void)
		{
			(this->_receiver->*(this->_fptr))(this->_a0, this->_a1, this->_a2, this->_a3,
                                           this->_a4, this->_a5, this->_a6, this->_a7);
		}
	};
   
   template<class CE, class F, class A0, class A1, class A2, class A3,
   class A4, class A5, class A6, class A7>
   Command* new_command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
   {
      return new _Command<CE,F,A0,A1,A2,A3,A4,A5,A6,A7>(receiver,fptr,a0,a1,a2,a3,a4,a5,a6,a7);
   }
   
   ///--------------------------------------------------------------------------------
	/// Specialized Command class with 0 args
   ///--------------------------------------------------------------------------------
	template<typename CE, typename F>
	class _Command<CE,F,NA,NA,NA,NA,NA,NA,NA,NA> 
   : public _CommandIF<CE,F,NA,NA,NA,NA,NA,NA,NA,NA>
	{
	public:
		// Define the base type for easy use later
		typedef _CommandIF<CE,F,NA,NA,NA,NA,NA,NA,NA,NA> base_type;
      //		typedef _CommandIntermediate<CE,F> base_type;
		
		/// Constructor
		_Command(CE* receiver, F fptr) : base_type(receiver, fptr) {}
		
		/// Execute a command
		void exec(void)
		{
			(this->_receiver->*(this->_fptr))();
		}
	};
   template<class CE, class F>
   Command* new_command(CE* receiver, F fptr)
   {
      return new _Command<CE,F,NA,NA,NA,NA,NA,NA,NA,NA>(receiver,fptr);
   }
   
   template<class CE, class F, class A0, class A1, class A2>
   Command* new_command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2)
   {
      return new _Command<CE,F,A0,A1,A2,NA,NA,NA,NA,NA>(receiver,fptr, a0, a1, a2);
   }
   
   
   ///--------------------------------------------------------------------------------
	/// Specialized Command class with 1 arg
   ///--------------------------------------------------------------------------------
	template<typename CE, typename F, typename A0>
	class _Command<CE, F, A0,NA,NA,NA,NA,NA,NA,NA>
   : public _CommandIF<CE,F,A0,NA,NA,NA,NA,NA,NA,NA>
	{
	public:
		// Define the base type for easy use later
		typedef _CommandIF<CE,F,A0,NA,NA,NA,NA,NA,NA,NA> base_type;
		
		/// Constructor
		_Command(CE* receiver, F fptr, A0 a0) : base_type(receiver, fptr, a0) {}
      
		/// Execute a command
		void exec(void)
		{
			(this->_receiver->*(this->_fptr))(this->_a0);
		}
	};
   
   template<class CE, class F, class A0>
   Command* new_command(CE* receiver, F fptr, A0 a0)
   {
      return new _Command<CE,F,A0,NA,NA,NA,NA,NA,NA,NA>(receiver,fptr, a0);
   }
   
   
   ///--------------------------------------------------------------------------------
	/// Specialized Command class with 2 args
   ///--------------------------------------------------------------------------------
	template<typename CE, typename F, typename A0, typename A1>
	class _Command<CE,F,A0,A1,NA,NA,NA,NA,NA,NA>
   : public _CommandIF<CE,F,A0,A1,NA,NA,NA,NA,NA,NA>
	{
	public:
		// Define the base type for easy use later
		typedef _CommandIF<CE,F,A0,A1,NA,NA,NA,NA,NA,NA> base_type;
		
		/// Constructor
		_Command(CE* receiver, F fptr, A0 a0, A1 a1) : base_type(receiver, fptr, a0, a1) {}
      
		/// Execute a command
		void exec(void)
		{
			(this->_receiver->*(this->_fptr))(this->_a0, this->_a1);
		}
	};
   
   template<class CE, class F, class A0, class A1>
   Command* new_command(CE* receiver, F fptr, A0 a0, A1 a1)
   {
      return new _Command<CE,F,A0,A1,NA,NA,NA,NA,NA,NA>(receiver,fptr, a0, a1);
   }
   
   
   ///--------------------------------------------------------------------------------
	/// Specialized Command class with 3 args
   ///--------------------------------------------------------------------------------
	template<typename CE, typename F, typename A0, typename A1, typename A2>
	class _Command<CE,F,A0,A1,A2,NA,NA,NA,NA,NA>
   : public _CommandIF<CE,F,A0,A1,A2,NA,NA,NA,NA,NA>
	{
	public:
		// Define the base type for easy use later
		typedef _CommandIF<CE,F,A0,A1,A2,NA,NA,NA,NA,NA> base_type;
		
		/// Constructor
		_Command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2) : base_type(receiver, fptr, a0, a1, a2) {}
      
		/// Execute a command
		void exec(void)
		{
			(this->_receiver->*(this->_fptr))(this->_a0, this->_a1, this->_a2);
		}
	};
   
#if 0
   template<class CE, class F, class A0, class A1, class A2>
   Command* new_command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2)
   {
      return new _Command<CE,F,A0,A1,A2,NA,NA,NA,NA,NA>(receiver,fptr,a0,a1,a2);
   }
#endif
   
   ///--------------------------------------------------------------------------------
	/// Specialized Command class with 4 args
   ///--------------------------------------------------------------------------------
	template<typename CE, typename F, typename A0, typename A1, typename A2, typename A3>
	class _Command<CE,F,A0,A1,A2,A3,NA,NA,NA,NA>
   : public _CommandIF<CE,F,A0,A1,A2,A3,NA,NA,NA,NA>
	{
	public:
		// Define the base type for easy use later
		typedef _CommandIF<CE,F,A0,A1,A2,A3,NA,NA,NA,NA> base_type;
		
		/// Constructor
		_Command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2, A3 a3)
      : base_type(receiver, fptr, a0, a1, a2, a3) {}
      
		/// Execute a command
		void exec(void)
		{
			(this->_receiver->*(this->_fptr))(this->_a0, this->_a1, this->_a2, this->_a3);
		}
	};
   
   template<class CE, class F, class A0, class A1, class A2, class A3>
   Command* new_command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2, A3 a3)
   {
      return new _Command<CE,F,A0,A1,A2,A3,NA,NA,NA,NA>(receiver,fptr, a0, a1, a2, a3);
   }
   
   ///--------------------------------------------------------------------------------
	/// Specialized Command class with 5 args
   ///--------------------------------------------------------------------------------
	template<typename CE, typename F, typename A0, typename A1, typename A2, typename A3,
   typename A4>
	class _Command<CE,F,A0,A1,A2,A3,A4,NA,NA,NA>
   : public _CommandIF<CE,F,A0,A1,A2,A3,A4,NA,NA,NA>
	{
	public:
		// Define the base type for easy use later
		typedef _CommandIF<CE,F,A0,A1,A2,A3,A4,NA,NA,NA> base_type;
		
		/// Constructor
		_Command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2, A3 a3,
               A4 a4)
      : base_type(receiver, fptr, a0, a1, a2, a3, a4) {}
      
		/// Execute a command
		void exec(void)
		{
			(this->_receiver->*(this->_fptr))(this->_a0, this->_a1, this->_a2, this->_a3,
                                           this->_a4);
		}
	};
   
   template<class CE, class F, class A0, class A1, class A2, class A3, class A4>
   Command* new_command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4)
   {
      return new _Command<CE,F,A0,A1,A2,A3,A4,NA,NA,NA>(receiver,fptr,a0,a1,a2,a3,a4);
   }
   
   ///--------------------------------------------------------------------------------
	/// Specialized Command class with 6 args
   ///--------------------------------------------------------------------------------
	template<typename CE, typename F, typename A0, typename A1, typename A2, typename A3,
   typename A4, typename A5>
	class _Command<CE,F,A0,A1,A2,A3,A4,A5,NA,NA>
   : public _CommandIF<CE,F,A0,A1,A2,A3,A4,A5,NA,NA>
	{
	public:
		// Define the base type for easy use later
		typedef _CommandIF<CE,F,A0,A1,A2,A3,A4,A5,NA,NA> base_type;
		
		/// Constructor
		_Command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2, A3 a3,
               A4 a4, A5 a5)
      : base_type(receiver, fptr, a0, a1, a2, a3, a4, a5) {}
      
		/// Execute a command
		void exec(void)
		{
			(this->_receiver->*(this->_fptr))(this->_a0, this->_a1, this->_a2, this->_a3,
                                           this->_a4, this->_a5);
		}
	};
   
   template<class CE, class F, class A0, class A1, class A2, class A3,
   class A4, class A5>
   Command* new_command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
   {
      return new _Command<CE,F,A0,A1,A2,A3,A4,A5,NA,NA>(receiver,fptr,a0,a1,a2,a3,a4,a5);
   }
   
   
   ///--------------------------------------------------------------------------------
	/// Specialized Command class with 7 args
   ///--------------------------------------------------------------------------------
	template<typename CE, typename F, typename A0, typename A1, typename A2, typename A3,
   typename A4, typename A5, typename A6>
	class _Command<CE,F,A0,A1,A2,A3,A4,A5,A6,NA>
   : public _CommandIF<CE,F,A0,A1,A2,A3,A4,A5,A6,NA>
	{
	public:
		// Define the base type for easy use later
		typedef _CommandIF<CE,F,A0,A1,A2,A3,A4,A5,A6,NA> base_type;
		
		/// Constructor
		_Command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2, A3 a3,
               A4 a4, A5 a5, A6 a6)
      : base_type(receiver, fptr, a0, a1, a2, a3, a4, a5, a6) {}
      
		/// Execute a command
		void exec(void)
		{
			(this->_receiver->*(this->_fptr))(this->_a0, this->_a1, this->_a2, this->_a3,
                                           this->_a4, this->_a5, this->_a6);
		}
	};
   
   template<class CE, class F, class A0, class A1, class A2, class A3,
   class A4, class A5, class A6>
   Command* new_command(CE* receiver, F fptr, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
   {
      return new _Command<CE,F,A0,A1,A2,A3,A4,A5,A6,NA>(receiver,fptr,a0,a1,a2,a3,a4,a5,a6);
   }
   
   ///--------------------------------------------------------------------------------
	/// Create a new command. No, you can't use a constructor directly,
	/// it needs to be wrapped up in a function for template parameter
	/// class type deduction goodness. Because you really don't want to 
	/// have to figure this out on your own. Let the compiler do the work.
   ///--------------------------------------------------------------------------------
   //	template<class CE, class F> Command* new_command(CE* ce, F fptr);
   
   ///--------------------------------------------------------------------------------
   /// Holder for arguments to functions in a Command
   ///--------------------------------------------------------------------------------
	class ArgHolder : public Counted
	{
	public:
		typedef gutz::SmartPtr<ArgHolder> pointer;
		ArgHolder () {}
		~ArgHolder() { std::cerr << "Destructed gutz::ArgHolder" << std::endl; }
	};
	
	template<class A0 = NA, class A1 = NA, class A2 = NA, class A3 = NA,
   class A4 = NA, class A5 = NA, class A6 = NA, class A7 = NA>
	class _ArgHolder : public ArgHolder
	{
	public:
		static NA empty;
#if 0
		_ArgHolder(A0 a0, A1 a1, A2 a2, A3 a3,
                 A1 a4, A2 a5, A3 a6, A4 a7) :
		ArgHolder(),
		_a0(a0), _a1(a1), _a2(a2), _a3(a3),
 		_a4(a4), _a5(a5), _a6(a6), _a7(a7) {}
#endif
		_ArgHolder()                           : ArgHolder(),
		_a0(empty), _a1(empty), _a2(empty), _a3(empty), _a4(empty), _a5(empty), _a6(empty), _a7(empty) {}
      
		_ArgHolder(A0 a0)                      : ArgHolder(),
      _a0(a0) {}
      
		_ArgHolder(A0 a0, A1 a1)               : ArgHolder(),
      _a0(a0), _a1(a1) {}
      
		_ArgHolder(A0 a0, A1 a1, A2 a2)        : ArgHolder(),
      _a0(a0), _a1(a1), _a2(a2) {}
      
		_ArgHolder(A0 a0, A1 a1, A2 a2, A3 a3) : ArgHolder(),
      _a0(a0), _a1(a1), _a2(a2), _a3(a3) {}
      
		_ArgHolder(A0 a0, A1 a1, A2 a2, A3 a3,
                 A4 a4)                      : ArgHolder(),
      _a0(a0), _a1(a1), _a2(a2), _a3(a3),
      _a4(a4) {}
      
		_ArgHolder(A0 a0, A1 a1, A2 a2, A3 a3,
                 A4 a4, A5 a5)               : ArgHolder(),
      _a0(a0), _a1(a1), _a2(a2), _a3(a3),
      _a4(a4), _a5(a5) {}
      
		_ArgHolder(A0 a0, A1 a1, A2 a2, A3 a3,
                 A4 a4, A5 a5, A5 a6)        : ArgHolder(),
      _a0(a0), _a1(a1), _a2(a2), _a3(a3),
      _a4(a4), _a5(a5), _a6(a6) {}
      
		_ArgHolder(A0 a0, A1 a1, A2 a2, A3 a3,
                 A4 a4, A5 a5, A5 a6, A7 a7) : ArgHolder(), 
      _a0(a0), _a1(a1), _a2(a2), _a3(a3),
      _a4(a4), _a5(a5), _a6(a6), _a7(a7) {}
      
		A0 _a0; A1 _a1; A2 _a2; A3 _a3;
		A1 _a4; A2 _a5; A3 _a6; A4 _a7;
	};
	
} // End namespace gutz

#endif
