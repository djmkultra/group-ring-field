/// symbolic math sanity 

#include "symath.h"

int main( int argc, char** argv )
{
  
   Symath::Symbol  a("a");
   Symath::Symbol  b("b");
   
   std::cout << " a = " << a << "   b = " << b << std::endl;
   std::cout << " a*b = " << a*b << std::endl;
   
   Symath::Symbol c("c");
   Symath::Symbol d("d");
   
   std::cout << " a+b*c*c = " << a+b*c*c << std::endl;
   
   (a+b*c*c).printTree(std::cout);
   
   
   std::cout << " -a+a = " << -a+a << std::endl;
   
   (-a+a).printTree(std::cout);
   

   std::cout << " a*(b-b)*c*c = " << a*(b-b)*c*c << std::endl;
   
   (a*(b-b)*c*c).printTree(std::cout);
   
   std::cout << " -(a*a+b) + (a*a+b) = " << -(a*a+b) + (a*a+b) << std::endl;
   std::cout << " -(a*a+(b + a)*a+b) = " << -(a*a+(b + a)*a+b) << std::endl;
   (-(a*a+(b + a)*a+b)).printTree(std::cout);
   
   Symath::Symbol sum("sum");
   sum = a;
   std::cout << " sum = " << sum << std::endl;
   sum += b;
   std::cout << " sum = " << sum << std::endl;
   sum += c;
   std::cout << " sum = " << sum << std::endl;
   
   
   return 0;
}
