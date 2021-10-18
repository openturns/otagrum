#include <iostream>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>

#include <openturns/OT.hxx>

#include "otagrum/JunctionTreeBernsteinCopula.hxx"
#include "otagrum/NamedJunctionTree.hxx"
#include "otagrum/otagrum.hxx"

void testOK()
{
  auto bn =
    gum::BayesNet<double>::fastPrototype("0->1->2->3;1->4->3;4->5;6->5;7->5");
  /*
   *   0
   *    \
   *     1
   *    / \
   *   2   4 6 7
   *    \ / \|/
   *     3   5
   */

  gum::LazyPropagation<double> ie(&bn);

  auto jtagr = ie.junctionTree();
  std::vector<std::string> names;
  for (const auto &elt : bn.nodes())
  {
    names.push_back(bn.variable(elt).name());
  }
  auto jt = OTAGRUM::NamedJunctionTree(*jtagr, names);
  unsigned int dim = jt.getSize();
  OT::CovarianceMatrix C(dim);
  for (unsigned int i = 0; i < dim; ++i)
  {
    for (unsigned int j = 0; j < i; ++j)
      C(i, j) = 1.0;
    C(i, i) = 2.0;
  }
  double entropyMC1, entropyMC2;
  OT::RandomGenerator::SetSeed(0);
  OT::Sample copulaSample = OT::Normal(OT::Point(dim), C).getSample(1000);
  {
    OT::RandomGenerator::SetSeed(33);
    OTAGRUM::JunctionTreeBernsteinCopula copula(jt, copulaSample, 5, false);
    OT::Sample sample = copula.getSample(1000);
    OT::Sample pdf = copula.computePDF(sample);
    entropyMC1 = -copula.computeLogPDF(copula.getSample(1000)).computeMean()[0];
  }
  {
    OT::RandomGenerator::SetSeed(33);
    OTAGRUM::JunctionTreeBernsteinCopula copula(jt, jt.getOrderMaxFirst(),
        copulaSample, 5, false);
    OT::Sample sample = copula.getSample(1000);
    OT::Sample pdf = copula.computePDF(sample);
    entropyMC2 = -copula.computeLogPDF(copula.getSample(1000)).computeMean()[0];
  }
  std::cout << "Same entropy : ";
  if (fabs(entropyMC1 - entropyMC2) < 1e-5)
    std::cout << "OK" << std::endl;
  else
    std::cout << "not OK" << std::endl;
}

int main(int /*argc*/, char ** /*argv*/)
{
  //OT::ResourceMap::SetAsUnsignedInteger("TBB-ThreadsNumber", 1);
  OT::ResourceMap::SetAsBool("Distribution-Parallel", false);
  testOK();
}
