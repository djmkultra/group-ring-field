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

#ifndef _exception_H
#define _exception_H

#include <string>
#include <exception>

////////////////////////////////////////////////////////////////////////////
/// Template class for gutz exceptions. The default gutz::Exception wraps
/// up a std::string for easy exception generation.
///
///
/// Here's an example of how to throw an exception:
///
/// \code
/// MyClass::MyClass(void)
/// {
///    // Set up a base error message for all exceptions that could be
///    // thrown in this method. Notice the space at the end. This 
///    // improves readibility when the exception is thrown
///    static const std::string base = "Error constructing MyClass: ";
///
///    switch(some_c_style_function())
///    {
///    case 0:
///      break;
///    case ENOMEM:
///      throw Exception(base + "Out of memory");
///      break;
///    default:
///      throw Exception(base + "Undocumented some_c_style_function() error");
///      break;
///  }
/// \endcode
///   
/// To extend the exception class to handle other types of exceptions, follow
/// the pattern for the Exception class and inherit _Exception<typename>.
/// You'll need to implement the what() function so that it returns a const char*
/// to your message. Be careful! This could result in a memory leak. If you
/// allocate a pointer to the const char* in the what() method, it will be lost
/// forever. 
////////////////////////////////////////////////////////////////////////////

namespace gutz
{
   template<typename T>
   class _Exception : public std::exception
   {
   public:
      _Exception(const T& whatHappened)
      : _whatHappened(whatHappened) {}
      
      _Exception(const _Exception& te)
      : _whatHappened(te._whatHappened) {}
      
      _Exception& operator=(const _Exception& te)
      {
         if(this != &te)
         {
            _whatHappened = te._whatHappened;
         }
         return *this;
      }
      
      virtual ~_Exception() throw() {}
      
      virtual const char* what(void) const throw() = 0;

   protected:
      T _whatHappened;

   private:
      _Exception(void) {}
   };

   class Exception : public _Exception<std::string>
   {
   public:
      typedef _Exception<std::string> base_type;
      Exception(const std::string& wh) : base_type(wh) {}
      virtual const char* what(void) const throw() { return _whatHappened.c_str(); }
   };
}

#endif      

