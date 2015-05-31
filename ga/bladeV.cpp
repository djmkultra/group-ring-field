/// blade V (fivve)
///   testing refactored symbols and geometri objects

#include <iostream>
#include "symath.h"
#include "geomobj.h"



typedef Symath::Sym S;
using namespace std;

int main( int argc, char **argv )
{
  GOf A(0.0, 1.0, 2.0, 3.0);
  GOsym As(S("0"), S("a1"), S("a2"), S("a3"));

  GOsym Ws = GOsym::conformal(S("w1"),S("w2"),S("w3"));
  std::cout << " Ws =          " << Ws << std::endl;
  std::cout << " Ws.Ws = " << inner(Ws,Ws) << std::endl << std::endl;
  std::cout << " Ws.Ws = " << simplify(inner(Ws,Ws)) << std::endl << std::endl;

  /// Conformal zero vector
  GOsym N0 = GOsym::conformal(S("0"),S("0"),S("0"));
  std::cout << " N0 = " << N0 << " = " << GOsym::n0() << std::endl;
  std::cout << " N0N0 = " << N0*N0 << "  .  " << inner(N0,N0) << std::endl;
  std::cout << " N0N0 = " << GOsym::product<GOsym::GEOMETRIC>(N0,N0) << std::endl;

  /// Scalar element test
  GOsym E0 = GOsym(S("1"),e0);
  std::cout << E0 << " * " << E0 << " = " << E0 * E0 << std::endl;
  std::cout << " 1 * A = " << E0 * As << std::endl; 
  GOsym E1 = GOsym(S("1"),e1);
  std::cout << E1 << " * " << E1 << " = " << E1 * E1 << std::endl;

  /// Conformal basis element inspection
  std::cout << " n0 " << GOf::n0() << "   ni " << GOf::ni() << std::endl;
  std::cout << " n0ni " << GOf::n0() * GOf::ni() << std::endl;
  std::cout << " nin0 " << GOf::ni() * GOf::n0() << std::endl;
  std::cout << " n0.ni " << GOf::product<GOf::INNER>(GOf::n0(), GOf::ni()) << std::endl;
  std::cout << " n0n0 " << GOf::n0() * GOf::n0() << std::endl;
  std::cout << " nini " << GOf::ni() * GOf::ni() << std::endl;

  /// Conformal vectors test
  GOf W = GOf::conformal(1, 2, 3);
  GOf Y = GOf::conformal(1, .5, 3);
  std::cout << " W homogenious = " << W << "  W.W = " << inner(W,W) << "  W*W " << W*W << std::endl;
  GOf w = GOf::extract(W);
  std::cout << " w native = " << w << "   w.w =  " << inner(w,w) << std::endl;
  std::cout << " Y " << Y << "  Y.Y = " << inner(Y,Y) << std::endl;
  std::cout << " W.Y = " << inner(W,Y) << "  = -1/2(|w-y|)^2 " << std::endl; 
  std::cout << " ni.W = -coeff(n0) = -1 : " << inner(GOf::ni(),W) << std::endl;
  //GOsym Ws = GOsym::conformal(S("w1"),S("w2"),S("w3"));
  std::cout << " Ws =          " << Ws << std::endl;
  std::cout << " Ws.distribute " << simplify(Ws) << std::endl;
  std::cout << " Ws.Ws = " << inner(Ws,Ws) << std::endl;
  std::cout << " ws = " << GOsym::extract(Ws) << std::endl;
  GOsym Em = GOsym(S("1"),eminus);
  GOf Eem = GOf(1.0, eminus);
  GOf Eep = GOf(1.0, eplus);
  std::cout << " " << Eem << " * " << Eem << " = " << Eem * Eem << std::endl;
  std::cout << " " << Eep << " * " << Eep << " = " << Eep * Eep << std::endl;
  std::cout << " e-e- = " << Em << " * " << Em << " = " << Em * GOsym(S("1"),eminus) << std::endl;
  std::cout << " e-^(-1) " << Em.inverse() << std::endl;
  GOf EemA = Eem * A;
  std::cout << " e-A " << EemA << "  e-A(e-A)^-1 = " << EemA * EemA.inverse() << std::endl;
  std::cout << " Ni = " << GOsym::ni() << std::endl;
  std::cout << " N0^Ws = " << wedge(N0,Ws) << std::endl;
  std::cout << " N0.Ws = " << inner(N0,Ws) << std::endl;
  GOsym Ni = GOsym::ni();
  std::cout << " Ni.Ws = " << inner(Ni,Ws) << std::endl << std::endl;

  /// No invert for conformal objects. NANNANAN
  GOf aaa = GOf::conformal(2,2,2);
  std::cout << " conformal geometry doesn't invert " << std::endl;
  std::cout << " aaa " << aaa << " inv " << aaa.inverse() << "  *  " << aaa * aaa.inverse() << std::endl;
  std::cout << " aaa.rev " << aaa.reverse() << std::endl << std::endl;

  W = GOf::conformal(1,0,0);

  /// Manualy build the parallel plane translation versor, this saved the day!
  GOf Pa = GOf(1,e2) + GOf::ni() * 1;
  GOf Pb = GOf(1,e2) + GOf::ni() * .5;
  std::cout << " pa =" << Pa << " pb=" << Pb << "  sammich " << Pa * Pb * W * Pb * Pa << std::endl;
  std::cout << " pa pb " << Pa * Pb << "      sammich " << Pa * (Pb * W) * Pb * Pa << std::endl;
  std::cout << " pb pa " << Pb * Pa << "      sammich " << Pa * (Pb * W * Pb) * Pa << std::endl;
  std::cout << " W pb " << W * Pb << "      sammich " << (Pa * Pb) * W * (Pb * Pa) << std::endl;
  GOf PaPb = Pa * Pb;
  GOf PbPa = Pb * Pa;
  std::cout << " PaPb " << PaPb << "  sammmmich " << PaPb * W * PbPa << std::endl << std::endl;

  /// Test the translation versor
  GOsym Tr = GOsym::translateVersor(As);
  std::cout << " Tr(A)      = " << Tr << std::endl;
  std::cout << " Tr(A)^(-1) = " << Tr.inverse() << std::endl;
  std::cout << " Tr(A)Tr(A)^(-1) = " << Tr*Tr.inverse() << std::endl << std::endl;
  GOf Ef1(1,e1,1,e2,1,e3);
  GOf T = GOf::translateVersor(Ef1);
  std::cout << " W          " << W << std::endl;
  std::cout << " T(" << Ef1 << ") = T(A)  " << T << std::endl;
  std::cout << " T(A)WT(A)~ = " << T * W * T.inverse() << std::endl;
  std::cout << " T(A)WT(A)~ = " << T * W * T.reverse() << std::endl;
  std::cout << " T(A)wT(A)~ = " << T * GOf::extract(W) * T.inverse() << "  no work with native " << GOf::extract(W) << std::endl;
  std::cout << " T(A)~WT(A) = " << T.inverse() * W * T << std::endl;
  std::cout << "    = " << GOf::extract(T * W * T.inverse()) << "  native extract " << std::endl;
  std::cout << " expecting " << GOf::extract(W) + Ef1 << std::endl << std::endl;
  std::cout << " T(A)^(-1) = " << T.inverse() << std::endl;
  std::cout << " T(A)^(-1) = " << GOf::invTranslateVersor(Ef1) << std::endl;
  std::cout << " T(A).T(A) = " << inner(T,T) << "      T * T = " << T * T << "   " << T.inverse() * T.inverse() <<std::endl;
  std::cout << " T(A)T(A)^(-1) = " << T * (T.inverse()) << std::endl;
  std::cout << " T(A)^(-1)T(A) = " << (T.inverse()) * T << std::endl;
  std::cout << " T(A)T(A)^(-1) = " << T * GOf::invTranslateVersor(Ef1) << std::endl;

  /// Basic vector test
  std::cout << std::endl << std::endl;
  std::cout << " A  = " << A << std::endl;
  std::cout << " A^-1 = " << A.inverse() << std::endl;
  std::cout << " AA^-1 " << A * A.inverse() << std::endl;
  std::cout << " AA = " << A * A << std::endl;
  std::cout << " A^(-1) = " << inverse(A) << std::endl;
  std::cout << " As = " << As << std::endl;
  std::cout << " As^(-1) = " << inverse(As) << std::endl;
   
  GOf B(0.0, 2.5, 2.5, 0.5);
  GOsym Bs(S("0"), S("b1"), S("b2"), S("b3"));
  std::cout << " B  = " << B << std::endl;
  std::cout << " Bs = " << Bs << std::endl;
   
  /// Wedges and duals
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
  std::cout << " Cs = " << Cs << std::endl << std::endl;

  /// Right cotraction 
  std::cout << "Cs^BsLAs = right(Cs^Bs, As) = " << std::endl << right(wedge(Cs,Bs),As) << std::endl << std::endl; 
  std::cout << "Cs^BsLBs = right(Cs^Bs, Bs) = " << std::endl << right(wedge(Cs,Bs),Bs) << std::endl << std::endl; 

  GOf D(0.0, -.5, -1.5, 1.0);
  GOsym Ds(S("0"), S("d1"), S("d2"), S("d3"));
  std::cout << " D  = " << D << std::endl;
  std::cout << " Ds = " << Ds << std::endl;
   
  /// More wedge and products and +- and reversal
  GOf CwD = wedge(C,D);
  GOsym CwDs = wedge(Cs,Ds);
  std::cout << " C ^ D = " << CwD << "   mag  " << inner(CwD,CwD) << std::endl;
  std::cout << " C * D = " << Cs * Ds << std::endl;
  std::cout << " C * D = " << C * D << "    mag  " << inner(C*D,C*D) << std::endl;
  std::cout << " C ^ D = " << CwDs << std::endl;
  std::cout << " (C^C)~ rev = " << CwDs.reverse() << std::endl;
  std::cout << " (C*D)~ rev = " << (Cs*Ds).reverse() << std::endl;
 
  /// Meet
  std::cout << " A^B V C^D = "  << meet( AwB, CwD, 3 ) << std::endl;
  std::cout << " A^B V C^D = "  << simplify( meet( AwBs, CwDs, 3 ) ) << std::endl;
  std::cout << " C^D V A^B = "  << meet( CwD, AwB, 3 ) << std::endl;
  std::cout << " C^D V A^B = "  << simplify( meet( CwDs, AwBs, 3 ) ) << std::endl;
  std::cout << std::endl;
  
  /// More maths
  std::cout << " C - C = " << Cs - Cs << std::endl;
  std::cout << " C + C = " << Cs + Cs << std::endl;
  std::cout << " C * D - C . D - C ^ D = " << Cs * Ds - inner(Cs, Ds) - wedge(Cs, Ds) << std::endl;
  std::cout << std::endl << std::endl;
  return 0;
}
