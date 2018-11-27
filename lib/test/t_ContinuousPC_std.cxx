#include <iostream>

#include <openturns/ClaytonCopulaFactory.hxx>
#include <openturns/ComposedCopula.hxx>
#include <openturns/FrankCopulaFactory.hxx>
#include <openturns/NormalCopulaFactory.hxx>

#include "otagrum/ContinuousPC.hxx"

OT::Sample getTrucBizarre(OT::UnsignedInteger size)
{
  OT::Collection<OT::Copula> copulas;

  OT::UnsignedInteger dim = 3;

  OT::CorrelationMatrix R(dim);
  for (OT::UnsignedInteger i = 0; i < dim; i++)
    for (OT::UnsignedInteger j = 0; j < i; j++)
      R(i, j) = (i + j + 1) / (2.0 * dim);

  copulas.add(OT::FrankCopula(4.0));
  copulas.add(OT::ClaytonCopula(3.0));
  //copulas.add(OT::NormalCopula(R));

  auto copula = OT::ComposedCopula(copulas);
  return copula.getSample(size);
}

int main(void)
{
  try
  {
    OT::Sample sample = getTrucBizarre(10000);
    std::cout << "Sample size : " << sample.getSize() << std::endl
              << "Sample dimension :" << sample.getDimension() << std::endl;
    {
      OTAGRUM::ContinuousPC learner(sample, 3, 0.9);
      learner.setOptimalPolicy(true);
      std::cout << "go" << std::endl;
      auto skel = learner.getSkeleton();
      std::cout << learner.skeletonToDot(skel) << std::endl;

      /*auto cpdag = learner.getPDAG(skel);
      std::cout << cpdag.toDot() << std::endl;
      for (const auto &e : learner.getRemoved()) {
        std::cout << e << " : p-value=" << learner.getPValue(e.first(),
      e.second())
                  << " sepset=" << learner.getSepset(e.first(), e.second())
                  << std::endl;
      } */
      for (const auto &e : skel.edges())
      {
        std::cout << e
                  << " : p-value=" << learner.getPValue(e.first(), e.second())
                  << std::endl;
      }
    }
    {
      OTAGRUM::ContinuousPC learner(sample, 5, 0.8);
      learner.setOptimalPolicy(true);
      std::cout << "go" << std::endl;
      auto skel = learner.getSkeleton();
      std::cout << skel.toDot() << std::endl;
      auto cpdag = learner.getPDAG(skel);
      std::cout << cpdag.toDot() << std::endl;
      auto moral = learner.getMoralGraph(cpdag);
      std::cout << moral.toDot() << std::endl;
    }
  }
  catch (OT::Exception &e)
  {
    std::cout << e.__repr__() << std::endl;
  }
  catch (gum::Exception &e)
  {
    GUM_SHOWERROR(e);
  }
  std::cout << "Finito " << std::endl;

  return EXIT_SUCCESS;
}
