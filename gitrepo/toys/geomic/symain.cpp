/// symbolic math sanity 

#include "symath.h"

typedef Symath::Symbol S;
using namespace Symath;
using namespace std;

int main( int argc, char** argv )
{
  
   S  a("a");
   S  b("b");
   
   cout << " a = " << a << "   b = " << b << endl;
   cout << " a*b = " << a*b << endl;
   
   S c("c");
   S d("d");
   
   cout << " a+b*c*c = " << a+b*c*c << endl;

   cout << endl;

   S e = a*(b+c+d) + a*(b*(c+d)) + a*((b+c)*(a+c));
   cout << " e = " << e << endl;
   cout << " distribute e = " << e.distribute() << endl;
   cout << " normalized e = " << e.normalForm() << endl;

   cout << endl;

   S g = (a + (b - c*c) - c * (b - c) + (-b) + (c * b - a)) + S::one();
   cout << " g = " << g << std::endl;
   cout << " distribute  " << g.distribute() << std::endl;
   cout << " cancel      " << g.cancelAdditions() << endl << endl;
   cout << " normalized g" << g.normalForm() << endl << endl;

   cout << " negs " << endl;
   cout << " -( -S(ONE) * a * (-a - b) + -(-(a + b)) ) = " << -( -S(ONE) * a * (-a - b) + -(-(a + b)) ) << endl;
   cout << "   = a * (-a - b) - (a + b) ^^ " << endl;
   cout << " -(-(a+b)) = " << -(-(a+b)) << endl;
   cout << " -S(ONE) " << -S(ONE) << "   " << -S(ONE) * a << "  ... " << a * (-a - b) << endl;
   cout << "  -S(ONE) * a * (-a - b) = " << -S(ONE) * a * (-a - b) << endl;

   S h = (a+b-c+ d *(a-b+c) * c * a - b);
   cout << " add and muls -h = " << -(h) << endl;
   cout << " add and muls  h = " << h  << endl;
   cout << " cancel & norm     " << h.cancelAdditions().normalForm() << endl;
   cout << " cancel            " << h.cancelAdditions() << endl;
   (h.cancelAdditions().normalForm()).printTree(cout) << endl;

   cout << " tree for a+b*c*c: " << endl;
   (a+b*c*c).printTree(cout) << endl;
   
   cout << " (cos(c-a)-sin(a+b*a)*d + sin(c*c + d*a)) = " << (cos(c-a)-sin(a+b*a)*d + sin(c*c + d*a)) << endl;
   (cos(c-a)-sin(a+b*a)*d + sin(c*c + d*a)).printTree(cout) << endl;

   cout << " -a+a = " << -a+a << endl;
   
   cout << " a*(b-b)*c*c = " << a*(b-b)*c*c << endl;
      
   cout << " -(a*a+b) + (a*a+b) = " << -(a*a+b) + (a*a+b) << endl;
   cout << " -(a*a+(b + a)*a+b) = " << -(a*a+(b + a)*a+b) << endl << endl;
   (-(a*a+(b + a)*a+b)).printTree(cout) << endl;
   
   S sum("sum");
   sum = a;
   cout << " sum = " << sum << endl;
   sum += b;
   cout << " sum = " << sum << endl;
   sum += c;
   cout << " sum = " << sum << endl << endl;
   
   cout << " sin:  " << sin(-(a-b)) * c + d << endl;

   return 0;
}
