#include <iostream>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>

#include <openturns/OT.hxx>

#include "otagrum/NamedJunctionTree.hxx"
#include "otagrum/JunctionTreeBernsteinCopula.hxx"
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
  OT::RandomGenerator::SetSeed(0);
  std::cout<<"Generator "<<OT::RandomGenerator::GetState()<<std::endl;
  OT::Sample copulaSample = OT::Normal(OT::Point(dim), C).getSample(1000);
  std::cout<<copulaSample<<std::endl;
  std::cout<<"Generator "<<OT::RandomGenerator::GetState()<<std::endl;
  std::cout<<"=========================="<<std::endl;
  std::cout<<"=========================="<<std::endl;


  {
    OT::RandomGenerator::SetSeed(0);
    std::cout<<"Generator "<<OT::RandomGenerator::GetState()<<std::endl;
    OTAGRUM::JunctionTreeBernsteinCopula copula(jt, copulaSample, 5, false);
    std::cout << "copula=" << copula << std::endl;
    OT::Sample sample = copula.getSample(1);
    std::cout << "sample=" << sample << std::endl;
    OT::Sample pdf = copula.computePDF(sample);
    std::cout << "pdf=" << pdf << std::endl;
    double entropyMC = -copula.computeLogPDF(copula.getSample(1000)).computeMean()[0];
    std::cout << "entropy (MC)=" << entropyMC << std::endl;
    std::cout<<"Generator "<<OT::RandomGenerator::GetState()<<std::endl;
  }
  {
    OT::RandomGenerator::SetSeed(0);
    std::cout<<"Generator "<<OT::RandomGenerator::GetState()<<std::endl;
    OTAGRUM::JunctionTreeBernsteinCopula copula(jt, jt.getOrderMaxFirst(), copulaSample, 5, false);
    std::cout << "copula=" << copula << std::endl;
    OT::Sample sample = copula.getSample(10);
    std::cout << "sample=" << sample << std::endl;
    OT::Sample pdf = copula.computePDF(sample);
    std::cout << "pdf=" << pdf << std::endl;
    double entropyMC = -copula.computeLogPDF(copula.getSample(1000)).computeMean()[0];
    std::cout << "entropy (MC)=" << entropyMC << std::endl;
    std::cout<<"Generator "<<OT::RandomGenerator::GetState()<<std::endl;
    if (false)
    {
      OT::Indices indices(2);
      for (unsigned int i = 0; i < dim; ++i)
      {
        indices[0] = i;
        for (unsigned int j = 0; j < i; ++j)
        {
          indices[1] = j;
          OT::Graph graph = copula.getMarginal(indices).drawPDF();
          graph.draw(OT::String(OT::OSS() << "marginal_" << i << "_" << j << "_pdf.png"), 600, 620, 1);
        } // j
      } // i
    }
  }
}

int main(int argc, char **argv)
{
  OT::RandomGenerator::SetSeed(0);
  OT::ResourceMap::SetAsUnsignedInteger("parallel-threads",1);
  OT::ResourceMap::SetAsBool("Distribution-Parallel",false);
  testOK();
}
