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

///////////////////////////////////////////////////////////////////////////
// Test up and down casting of SmartPtrs between base and derived types
//
// Author: Jeff Bowles <jbowles@cs.unm.edu>
///////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <smartptr.h>
#include <exceptionGutz.h>

namespace test
{
   // Ewww, global variables. This keeps track of memory leaks and
   // the number of failed tests. I would never do this in the real
   // world. At least they're confined to the test namespace.
   static int numBase      = 0;
   static int numDerived   = 0;
   static int numUnrelated = 0;
   static int failedTests  = 0;
   
   // Base class
   class Base : public gutz::Counted 
   {
   public:
      // Pointer type for this class
      typedef gutz::SmartPtr<Base> pointer;
      
      Base()
      {
         numBase++;
      }
      
      ~Base()
      {
         numBase--;
      }
      
      virtual const std::string whoami(void) const
      {
         return std::string("Base::whoami()");
      }
   };
   
   // Derived class
   class Derived : public Base
   {
   public:
      typedef gutz::SmartPtr<Derived> pointer;
      
      Derived(const std::string& name = "noname") : Base(), _name(name)
      {
         numDerived++;
      }
      
      ~Derived()
      {
         numDerived--;
      }
      
      virtual const std::string whoami(void) const
      {
         return std::string("Derived::whomai() ") + _name;
      }
      
   private:
      std::string _name;
   };
   
   // Unrelated class.
   class Unrelated : public gutz::Counted
   {
   public:
      typedef gutz::SmartPtr<Unrelated> pointer;
      
      Unrelated()
      {
         numUnrelated++;
      }
      
      ~Unrelated()
      {
         numUnrelated--;
      }
      
      const std::string myName(void) const
      {
         return std::string("Unrelated::myName()");
      }
   };
   
   
   /// Test for equality. If the test passes, don't output anything. Only output failures
   template<class T>
   void testEqual(const T& actual, const T& expected, const std::string& message = "")
   {
      if(actual != expected)
      {
         std::cout << "Fail: " << message << " expected: \"" << expected << "\", actual: \"" << actual << "\"" << std::endl;
         failedTests++;
      }
   }
   
   /// Run tests on up and down casting
   void runTests(void)
   {
      // Create a Base instance
      Base::pointer basePtr = new Base();
      
      // Create a Derived instance
      Derived::pointer derivedPtr = new Derived("derived");
      
      // Make sure that whoami() returns the expected string
      const std::string expectedForBase    = "Base::whoami()";
      const std::string expectedForDerived = "Derived::whomai() derived";
      
      testEqual(basePtr->whoami(),    expectedForBase,    "basePtr->whoami()");
      testEqual(derivedPtr->whoami(), expectedForDerived, "derivedPtr->whoami()");
      
      // Test up casting - cast a smart pointer to an instance of Derived to
      // a smart pointer pointing to Base
      Base::pointer bPtr = gutz::up_cast<Base>(derivedPtr);
      testEqual(bPtr->whoami(), expectedForDerived, "Up cast from Derived to Base, whoami() should return Derived::whoami()");
      
      // Test down casting. Cast the previously upcasted smart pointer back to 
      // the derived class
      Derived::pointer dPtr = gutz::down_cast<Derived>(bPtr);
      testEqual(dPtr->whoami(), expectedForDerived, "Down cast from Base to Derived, whoami() should return Derived::whoami()");
      
      // Down cast something to an unrelated type. This should fail and throw an exception
      bool downCastUnrelatedWorked = true;
      Unrelated::pointer uPtr = gutz::down_cast<Unrelated>(bPtr);
      if(!uPtr)
      {
         downCastUnrelatedWorked = false;
      }
      testEqual(downCastUnrelatedWorked, false, "Down cast was able to down cast to an unrelated type");
   }
}   

using namespace test;

int main(int argc, char** argv)
{
   runTests();
   
   // Check for memory leaks
   testEqual(numBase, 0,      "Memory leak for Base class");
   testEqual(numDerived, 0,   "Memory leak for Derived class");
   testEqual(numUnrelated, 0, "Memory leak for Unrelated class");
   
   if(failedTests <= 0)
   {
      std::cout << "All tests passed" << std::endl;
   }
   else
   {
      std::cout << "Failed tests: " << failedTests << std::endl;
   }
   
   return 0;
}