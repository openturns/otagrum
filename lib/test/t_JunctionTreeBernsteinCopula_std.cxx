#include <iostream>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>

#include <openturns/Exception.hxx>
#include <openturns/Indices.hxx>
#include <openturns/Normal.hxx>
#include <openturns/Graph.hxx>

#include "otagrum/JunctionTree.hxx"
#include "otagrum/JunctionTreeBernsteinCopula.hxx"
#include "otagrum/otagrum.hxx"

void testOK() {
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
  for (const auto &elt : bn.nodes()) {
    names.push_back(bn.variable(elt).name());
  }
  auto jt = OTAGRUM::JunctionTree(*jtagr, names);
  OT::Sample copulaSample = OT::Normal(jt.getSize()).getSample(1000);
  OTAGRUM::JunctionTreeBernsteinCopula copula(jt, copulaSample, 5, false);
  std::cout << "copula=" << copula << std::endl;
  OT::Sample sample = copula.getSample(10);
  std::cout << "sample=" << sample << std::endl;
  OT::Sample pdf = copula.computePDF(sample);
  std::cout << "pdf=" << pdf << std::endl;
  //OT::Graph graph = copula.drawMarginal2DPDF(0, 1, OT::Point(2, 0.0), OT::Point(2, 1.0), OT::Indices(2, 31));
  //graph.draw("test_marginal_pdf.png");
}

int main(int argc, char **argv) {
  testOK();
}
