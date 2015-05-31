/// symbolic math sanity 

#include "symath.h"

typedef Symath::Sym S;
using namespace Symath;
using namespace std;

int main( int argc, char** argv )
{
  S one = S::one();
  S zero = S::zero();
  std::cout << " one " << one << " = " << S(one) << " = " << S(1) << " = " 
	    << S("1") << " T " << one.isOne() << std::endl;
  std::cout << " zero " << zero << " = " << S(zero) << " = " << S(0) << " = " 
	    << S("0") << " T " << zero.isZero() << std::endl;
  
  S w1("w1");
  S w2("w2");
  S w3("w3");
  S Wsa = (one+-one/S(2)*(w1*w1+w2*w2+w3*w3)/S(2)); 
  S Wsb = (one+(w1*w1+w2*w2+w3*w3)/S(2));
  S Wsab = Wsa * Wsb;
  
  std::cout << " Wsa      " << Wsa << std::endl;
  std::cout << " Wsa dist " << Wsa.distribute() << std::endl;
  std::cout << " Wsa norm " << Wsa.normalForm() << std::endl;
  std::cout << " Wsa cncl " << Wsa.cancelAdditions() << std::endl << std::endl;
  std::cout << " Wsb      " << Wsb << std::endl;
  std::cout << " Wsb dist " << Wsb.distribute() << std::endl;
  std::cout << " Wsb norm " << Wsb.normalForm() << std::endl;
  std::cout << " Wsb cncl " << Wsb.cancelAdditions() << std::endl << std::endl;
  std::cout << " Wsab      " << Wsab << std::endl;
  std::cout << " Wsab dist " << Wsab.distribute() << std::endl;
  std::cout << " Wsab norm " << Wsab.normalForm() << std::endl;
  std::cout << " Wsab cncl " << Wsab.cancelAdditions() << std::endl << std::endl;
  std::cout << " Wsab - Wsab " << (Wsab - Wsab).cancelAdditions() << std::endl;

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
   cout << " -( -S(1) * a * (-a - b) + -(-(a + b)) ) = " << -( -S(1) * a * (-a - b) + -(-(a + b)) ) << endl;
   cout << "   = a * (-a - b) - (a + b) ^^ " << endl;
   cout << " -(-(a+b)) = " << -(-(a+b)) << endl;
   cout << " -S(1) " << -S(1) << "   " << -S(1) * a << "  ... " << a * (-a - b) << endl;
   cout << "  -S(1) * a * (-a - b) = " << -S(1) * a * (-a - b) << endl;

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
