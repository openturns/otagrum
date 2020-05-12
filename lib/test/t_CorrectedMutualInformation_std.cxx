#include <iostream>

#include <openturns/ClaytonCopulaFactory.hxx>
#include <openturns/ComposedCopula.hxx>
#include <openturns/FrankCopulaFactory.hxx>
#include <openturns/NormalCopulaFactory.hxx>
#include <openturns/RandomGenerator.hxx>

#include "otagrum/CorrectedMutualInformation.hxx"

using namespace OTAGRUM;

OT::Sample getNormalSample(OT::UnsignedInteger size)
{
  OT::UnsignedInteger dim = 10;

  OT::CorrelationMatrix R(dim);
  for (OT::UnsignedInteger i = 0; i < dim; i++)
    for (OT::UnsignedInteger j = 0; j < i; j++)
      R(i, j) = (i + j + 1.0) / (4.0 * dim);

  OT::Point mean(dim, 2.0);
  OT::Point sigma(dim, 3.0);
  OT::Normal distribution(mean, sigma, R);

  return distribution.getSample(size);
}

OT::Description DescrFromStringVect(const std::vector<std::string>& v)
{
  OT::Description res;
  for(auto s : v)
  {
    res.add(s);
  }
  return res;
}

OT::Sample getSpecificInstanceSeePythonTest(OT::UnsignedInteger size)
{
  auto R = OT::CorrelationMatrix(3);
  R(0, 1) = 0.9;
  R(0, 2) = 0.25;
  auto copula1 = OT::NormalCopula(R);
  copula1.setDescription(DescrFromStringVect({"A1", "A2", "A3"}));

  auto copula2 = OT::FrankCopula(3.0);
  copula2.setDescription(DescrFromStringVect({"B1", "B2"}));

  auto copula3 = OT::ClaytonCopula(2.0);
  copula2.setDescription(DescrFromStringVect({"C1", "C2"}));

  OT::Collection<OT::Distribution> copulas;
  copulas.add(copula1);
  copulas.add(copula2);
  copulas.add(copula3);

  auto copula = OT::ComposedCopula(copulas);

  auto sample = copula.getSample(size);
  return sample;
}

void testNormalSample()
{
  OT::RandomGenerator::SetSeed(0);
  OT::Sample sample = getNormalSample(1300);
  std::cout << "Sample size: " << sample.getSize() << std::endl
            << "Sample dimension: " << sample.getDimension() << std::endl
            << "Sample " << std::endl << sample << std::endl;

  CorrectedMutualInformation info(sample);

  double I2_1 = info.compute2PtCorrectedInformation(0, 1);
  double I3_1 = info.compute3PtCorrectedInformation(0, 1, 4);

  double I2_2 = info.compute2PtCorrectedInformation(8, 9);
  double I3_2 = info.compute3PtCorrectedInformation(8, 9, 3);

  std::cout << "Two-point corrected information: " << I2_1 << std::endl
            << "Three-point corrected information: " << I3_1 << std::endl;
  std::cout << "Two-point corrected information: : " << I2_2 << std::endl
            << "Three-point corrected information: " << I3_2 << std::endl;
}

void testIndepsSeePythonTest()
{
  auto data = getSpecificInstanceSeePythonTest(6000);

  CorrectedMutualInformation info(data);
  info.setAlpha(0.01);

  //  0 et 1 not independent
  double I2 = info.compute2PtCorrectedInformation(0, 1);
  std::cout << "Two-point corrected information: " << I2 
            << "\tTest: " << ((I2<0) ? "fail " : "OK ") << std::endl;

  // O et 4 independent
  I2 = info.compute2PtCorrectedInformation(0, 4);
  std::cout << "Two-point corrected information: " << I2
            << "\tTest: " << ((I2<0) ? "OK " : "fail ") << std::endl;

  //OT::Indices U;
  // O et 4 independent if 2,3
  //I2 = info.compute2PtCorrectedInformation(0, 4, U + 2 + 3);
  //std::cout << "Two-point corrected information: " << I2
            //<< "Test: " << ((I2<0) ? "OK " : "fail ") << std::endl;

  // O et 2 independent if 1.
  //I2 = info.compute2PtCorrectedInformation(1, 2, U + 0);
  //std::cout << "Two-point corrected information: " << I2
            //<< "Test: " << ((I2<0) ? "OK " : "fail ") << std::endl;
}

int main(int /*argc*/, char ** /*argv*/)
{
  testNormalSample();
  testIndepsSeePythonTest();
  return EXIT_SUCCESS;
}
