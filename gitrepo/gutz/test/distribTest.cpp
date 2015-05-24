// Test the distribution generator
// KCP -- March 10, 2007

#include "gaussDistrib.h"
#include <chiDistrib.h>
#include <chiSquareDistrib.h>
#include <poissonDistrib.h>
#include <rayleighDistrib.h>
#include <uniformDistrib.h>
#include <laplaceDistrib.h>
#include <exponentialDistrib.h>
#include <iostream>

int main(int argc, char **argv)
{

  GaussDistrib<double> *gd = new GaussDistrib<double>(0, 1, gutz::vec2d(-10.0, 10.0));   
  double test = gd->PDF(0);
std::cout << "test: " << test << std::endl;
   std::vector<double> distrib = gd->generateDistribution(10);
/*
  for(unsigned i = 0; i < distrib.size(); i++)
    std::cout << distrib[i] << " ";
  std::cout << std::endl;
*/
  ChiDistrib<double> *chid = new ChiDistrib<double>();
  ChiSquareDistrib<double> *chisqd = new ChiSquareDistrib<double>();
  ExponentialDistrib<double> *exd = new ExponentialDistrib<double>();
  LaplaceDistrib<double> *ld = new LaplaceDistrib<double>();
  PoissonDistrib<double> *pd = new PoissonDistrib<double>();
  RayleighDistrib<double> *rd = new RayleighDistrib<double>();
  UniformDistrib<double> *ud = new UniformDistrib<double>();

	std::cout << "chi mode: " << chid->PDF(1) << std::endl;
	std::cout << "chi squared mode: " << chisqd->PDF(1) << std::endl;
	std::cout << "exp mode: " << exd->PDF(1) << std::endl;
	std::cout << "Laplace mode: " << ld->PDF(1) << std::endl;
	std::cout << "Poisson mode: " << pd->PDF(1) << std::endl;
	std::cout << "Raleigh mode: " << rd->PDF(1) << std::endl;
	std::cout << "Uniform mode: " << ud->PDF(1) << std::endl;
	
	return 0;
}
   
