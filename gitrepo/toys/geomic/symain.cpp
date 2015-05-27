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
   
   cout << " negs " << endl;
   cout << " -( -S(ONE) * a * (-a - b) + -(-(a + b)) ) = " << -( -S(ONE) * a * (-a - b) + -(-(a + b)) ) << endl;
   cout << "   = a * (-a - b) - (a + b) ^^ " << endl;
   cout << " -(-(a+b)) = " << -(-(a+b)) << endl;
   cout << " -S(ONE) " << -S(ONE) << "   " << -S(ONE) * a << "  ... " << a * (-a - b) << endl;
   cout << "  -S(ONE) * a * (-a - b) = " << -S(ONE) * a * (-a - b) << endl;


   (a+b*c*c).printTree(cout) << endl;
   
   
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
