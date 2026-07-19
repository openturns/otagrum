#include <cmath>
#include <iostream>

#include <openturns/OT.hxx>
#include "otagrum/otagrum.hxx"

using namespace OTAGRUM;

void testBuildFromSample()
{
  const OT::UnsignedInteger dim = 4;
  OT::CorrelationMatrix R(dim);
  for (OT::UnsignedInteger i = 0; i < dim; ++i)
    for (OT::UnsignedInteger j = 0; j < i; ++j)
      R(i, j) = 0.3;

  OT::RandomGenerator::SetSeed(0);
  OT::Sample sample(OT::Normal(OT::Point(dim), R).getSample(500));

  JunctionTreeBernsteinCopulaFactory factory(5, 0.1, 3);
  JunctionTreeBernsteinCopula copula = factory.buildAsJunctionTreeBernsteinCopula(sample);

  std::cout << "learnt copula dimension : "
            << ((copula.getDimension() == dim) ? "OK" : "fail") << std::endl;

  OT::RandomGenerator::SetSeed(33);
  const double entropy =
    -copula.computeLogPDF(copula.getSample(500)).computeMean()[0];
  std::cout << "entropy is finite : "
            << (std::isfinite(entropy) ? "OK" : "fail") << std::endl;

  // build() (the OT::DistributionFactoryImplementation entry point) should
  // agree with buildAsJunctionTreeBernsteinCopula() on the dimension.
  OT::Distribution generic = factory.build(sample);
  std::cout << "build() dimension matches : "
            << ((generic.getDimension() == dim) ? "OK" : "fail") << std::endl;
}

void testEmptySampleRaises()
{
  JunctionTreeBernsteinCopulaFactory factory;
  std::cout << std::endl << "Empty sample should raise : ";
  try
  {
    factory.buildAsJunctionTreeBernsteinCopula(OT::Sample());
    std::cout << "fail (no exception)" << std::endl;
  }
  catch (const OT::InvalidArgumentException &)
  {
    std::cout << "OK" << std::endl;
  }
}

void testDefaultBuild()
{
  JunctionTreeBernsteinCopulaFactory factory;
  JunctionTreeBernsteinCopula empty = factory.buildAsJunctionTreeBernsteinCopula();
  std::cout << std::endl << "Default build() dimension : "
            << ((empty.getDimension() == 1) ? "OK" : "fail") << std::endl;
}

int main(int /*argc*/, char ** /*argv*/)
{
  testBuildFromSample();
  testEmptySampleRaises();
  testDefaultBuild();
  return EXIT_SUCCESS;
}
