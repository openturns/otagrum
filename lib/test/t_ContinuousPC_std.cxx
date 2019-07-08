#include <iostream>

#include <openturns/ClaytonCopulaFactory.hxx>
#include <openturns/ComposedCopula.hxx>
#include <openturns/FrankCopulaFactory.hxx>
#include <openturns/NormalCopulaFactory.hxx>
#include <openturns/RandomGenerator.hxx>

#include "otagrum/ContinuousPC.hxx"

OT::Description DescrFromStringVect(const std::vector<std::string> &v) {
  OT::Description res;
  for (auto s : v) {
    res.add(s);
  }
  return res;
}
/*
0--1 : p-value=0
2--3 : p-value=0.750422
1--4 : p-value=0.933081
2--4 : p-value=6.18139e-35
3--4 : p-value=3.61185e-182
*/
OT::Sample getTrucBizarre(OT::UnsignedInteger size) {
  OT::Collection<OT::Copula> copulas;

  OT::UnsignedInteger dim = 3;

  OT::CorrelationMatrix R(dim);
  for (OT::UnsignedInteger i = 0; i < dim; i++)
    for (OT::UnsignedInteger j = 0; j < i; j++)
      R(i, j) = (i + j + 1) / (2.0 * dim);

  auto copula1 = OT::FrankCopula(4.0);
  copula1.setDescription(DescrFromStringVect({"A1", "A2"}));
  auto copula2 = OT::ClaytonCopula(3.0);
  copula2.setDescription(DescrFromStringVect({"B1", "B2"}));

  copulas.add(copula1);
  // copulas.add(copula2);
  copulas.add(OT::NormalCopula(R));

  auto copula = OT::ComposedCopula(copulas);
  return copula.getSample(size);
}

void tests(void) {
  try {
    OT::RandomGenerator::SetSeed(0);
    OT::Sample sample = getTrucBizarre(7000);
    std::cout << "Sample size : " << sample.getSize() << std::endl
              << "Sample dimension :" << sample.getDimension() << std::endl
              << "Sample "<<sample<<std::endl;
    {
      OTAGRUM::ContinuousPC learner(sample, 3, 0.9);
      std::cout << "go" << std::endl;
      auto skel = learner.inferSkeleton();
      std::cout << learner.skeletonToDot(skel) << std::endl;

      /*auto cpdag = learner.inferPDAG(skel);
      std::cout << cpdag.toDot() << std::endl;
      for (const auto &e : learner.getRemoved()) {
        std::cout << e << " : p-value=" << learner.getPValue(e.first(),
      e.second())
                  << " sepset=" << learner.getSepset(e.first(), e.second())
                  << std::endl;
      } */
      for (const auto &e : skel.edges()) {
        std::cout << e
                  << " : p-value=" << learner.getPValue(e.first(), e.second())
                  << std::endl;
      }
    }
    {
      std::cout << "\n\n";
      OTAGRUM::ContinuousPC learner(sample, 5, 0.8);
      std::cout << "go" << std::endl;
      auto skel = learner.inferSkeleton();
      std::cout << "\n****\n"
                << "skeletton"
                << "\n****\n"
                << skel.toDot() << std::endl;
      auto cpdag = learner.inferPDAG(skel);
      std::cout << "\n****\n"
                << "CPDAG"
                << "\n****\n"
                << cpdag.toDot() << std::endl;
      auto moral = learner.deriveMoralGraph(cpdag);
      std::cout << "\n****\n"
                << "Moral graph"
                << "\n****\n"
                << moral.toDot() << std::endl;
      auto jt = learner.deriveJunctionTree(cpdag);
      std::cout << "\n****\n"
                << "Junction tree"
                << "\n****\n"
                << jt.toDot() << std::endl;

      auto njt = learner.learnJunctionTree();
      std::cout << "\n****\n"
                << "Named Junction tree"
                << "\n****\n"
                << njt.__str__() << std::endl;
      auto ndag = learner.learnDAG();
      std::cout << "\n****\n"
                << "Named DAG"
                << "\n****\n"
                << ndag.__str__() << std::endl;
      std::cout << ndag.toDot() << std::endl;
    }
  } catch (OT::Exception &e) {
    std::cout << e.__repr__() << std::endl;
  } catch (gum::Exception &e) {
    GUM_SHOWERROR(e);
  }
};

int main(void) {
  OT::Log::Show(OT::Log::ALL);
  tests();
  
  return EXIT_SUCCESS;
}
