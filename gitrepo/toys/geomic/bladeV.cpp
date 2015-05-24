/// blade V (fivve)
///   testing refactored symbols and geometri objects

#include <iostream>
#include "symath.h"
#include "geomobj.h"



typedef Symath::Symbol S;
using namespace std;

int main( int argc, char **argv )
{
   GOf A(0.0, 1.0, 2.0, 3.0);
   GOsym As(S("0"), S("a1"), S("a2"), S("a3"));
   std::cout << " A  = " << A << std::endl;
   std::cout << " A^(-1) = " << inverse(A) << std::endl;
   std::cout << " As = " << As << std::endl;
   std::cout << " As^(-1) = " << inverse(As) << std::endl;
   
   GOf B(0.0, 2.5, 2.5, 0.5);
   GOsym Bs(S("0"), S("b1"), S("b2"), S("b3"));
   std::cout << " B  = " << B << std::endl;
   std::cout << " Bs = " << Bs << std::endl;

   
   GOf AwB = wedge(A, B);
   GOsym AwBs = wedge(As, Bs);
   std::cout << " A ^ B = " << AwB << std::endl;
   std::cout << " A ^ B = " << AwBs << std::endl;
   std::cout << " B ^ A = " << wedge(B,A) << std::endl;
   std::cout << " B ^ A = " << wedge(Bs,As) << std::endl;
   std::cout << " dual(A^B) " << dual( AwB, 3 ) << std::endl;
   std::cout << " dual(A^B) " << simplify( dual( AwBs, 3 ) ) << std::endl;
   std::cout << " dual(B^A) " << dual( wedge(B,A), 3 ) << std::endl;
   std::cout << " dual(B^A) " << simplify( dual( wedge(Bs,As), 3 ) ) << std::endl;
   
   GOf C(0.0, -1.0, .5, -1.0);
   GOsym Cs(S("0"), S("c1"), S("c2"), S("c3"));
   std::cout << " C  = " << C << std::endl;
   std::cout << " Cs = " << Cs << std::endl;
   
   GOf D(0.0, -.5, -1.5, 1.0);
   GOsym Ds(S("0"), S("d1"), S("d2"), S("d3"));
   std::cout << " D  = " << D << std::endl;
   std::cout << " Ds = " << Ds << std::endl;
   
   GOf CwD = wedge(C,D);
   GOsym CwDs = wedge(Cs,Ds);
   std::cout << " C ^ D = " << CwD << std::endl;
   std::cout << " C * D = " << Cs * Ds << std::endl;
   std::cout << " C ^ D = " << CwDs << std::endl;
   
   std::cout << " A^B V C^D = "  << meet( AwB, CwD, 3 ) << std::endl;
   std::cout << " A^B V C^D = "  << simplify( meet( AwBs, CwDs, 3 ) ) << std::endl;
   std::cout << " C^D V A^B = "  << meet( CwD, AwB, 3 ) << std::endl;
   std::cout << " C^D V A^B = "  << simplify( meet( CwDs, AwBs, 3 ) ) << std::endl;
   std::cout << std::endl;
   std::cout << " C - C = " << Cs - Cs << std::endl;
   std::cout << " C + C = " << Cs + Cs << std::endl;
   std::cout << " C * D - C . D - C ^ D = " << Cs * Ds - inner(Cs, Ds) - wedge(Cs, Ds) << std::endl;
   
  return 0;
}
