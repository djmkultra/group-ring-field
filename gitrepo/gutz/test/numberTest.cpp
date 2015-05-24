/// Number Theory Tests


#include "../mathGutz/numbers.h"
#include <iostream>



int main(int argc, char **argv)
{
  typedef std::vector<int> intvec;
  typedef intvec::iterator intvecIter;

  const int maxnum = 1000;

  intvec pv = primes( maxnum );
  
  std::cout << " Primes up to: " << maxnum << std::endl;
  for(intvecIter ivi = pv.begin(), END = pv.end(); ivi != END; ++ivi)
    {
      std::cout << (*ivi) << "    ";
    }
  std::cout << std::endl << " ** " << pv.size() << " ** " << std::endl;

  std::cout << " Coprimes up to: " << maxnum << std::endl;
  int count = 0;
  for( int i=2; i< maxnum; ++i )
    {
      for( int j=i+1; j<maxnum; ++j )
	{
	  if( coprime(i,j) )
	    {
	      std::cout << "( " << i << " & " << j << " )";
	      ++count;
	    }
	}
    }
  std::cout << std::endl << " ** " << count << " ** = " << count/double(maxnum*maxnum) << " density " << std::endl;


  std::cout << " 20 choose 2 " << choose( 20, 2 ) << "   " 
	    << binomial( 20, 2 ) <<  std::endl;
  std::cout << " 20 choose 1 " << choose( 20, 1 ) << "   " 
	    << binomial( 20, 1 ) <<  std::endl;
}
