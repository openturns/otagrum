#include <iostream>

#include <openturns/ClaytonCopulaFactory.hxx>
#include <openturns/ComposedCopula.hxx>
#include <openturns/FrankCopulaFactory.hxx>
#include <openturns/NormalCopulaFactory.hxx>
#include <openturns/RandomGenerator.hxx>

#include "otagrum/TabuList.hxx"

OT::Description DescrFromStringVect(const std::vector<std::string> &v)
{
  OT::Description res;
  for (auto s : v)
  {
    res.add(s);
  }
  return res;
}

OT::Sample getTrucBizarre(OT::UnsignedInteger size)
{
  OT::Collection<OT::Distribution> copulas;

  OT::UnsignedInteger dim = 3;

  OT::CorrelationMatrix R(dim);
  for (OT::UnsignedInteger i = 0; i < dim; i++)
    for (OT::UnsignedInteger j = 0; j < i; j++)
      R(i, j) = 0.64;

  auto copula1 = OT::FrankCopula(4.0);
  copula1.setDescription(DescrFromStringVect({"A1", "A2"}));
  auto copula2 = OT::ClaytonCopula(3.0);
  copula2.setDescription(DescrFromStringVect({"B1", "B2"}));

  copulas.add(copula1);
  copulas.add(copula2);
  copulas.add(OT::NormalCopula(R));

  auto copula = OT::ComposedCopula(copulas);
  return copula.getSample(size);
}

void tests(void)
{
  try
  {
    OT::RandomGenerator::SetSeed(0);
    OT::Sample sample = getTrucBizarre(4000);
    std::cout << "Sample size: " << sample.getSize() << std::endl
              << "Sample dimension:" << sample.getDimension() << std::endl
              << "Sample " << std::endl << sample << std::endl;
    {
      std::cout << "\n\n";
      OTAGRUM::TabuList learner(sample);
      std::cout << "go" << std::endl;
      auto ndag = learner.learnDAG();
      std::cout << "\n****\n"
                << "Named DAG"
                << "\n****\n"
                << ndag.__str__() << std::endl;
      std::cout << ndag.toDot() << std::endl;
    }
  }
  catch (const OT::Exception & e)
  {
    std::cout << e.__repr__() << std::endl;
  }
  catch (const gum::Exception & e)
  {
    GUM_SHOWERROR(e);
  }
};

std::string dirname()
{
  std::string res = __FILE__;
  const size_t last_slash_idx = res.find_last_of("\\/");
  if (std::string::npos != last_slash_idx)
  {
    res.erase(last_slash_idx + 1);
  }
  return res;
}

int main(void)
{
//   OT::Log::Show(OT::Log::ALL);
  tests();

  return EXIT_SUCCESS;
}
