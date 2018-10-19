#include <iostream>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>

#include <openturns/Exception.hxx>
#include <openturns/Indices.hxx>

#include "otagrum/JunctionTree.hxx"
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

  std::cout << "** CLIQUES **" << std::endl;
  for (auto cliq : jt.getNodes()) {
    std::cout << jtagr->clique(cliq)
              << "  as OT::Indices = " << jt.getClique(cliq) << std::endl;
  }

  std::cout << std::endl << "** SEPARATORS **" << std::endl;
  for (auto edge : jt.getEdges()) {
    std::cout << jtagr->separator(edge)
              << "  as OT::Indices = " << jt.getSeparator(edge) << std::endl;
  }
}

void testNotOKForSize() {
  auto bn =
      gum::BayesNet<double>::fastPrototype("0->1->2->3;1->4->3->5;6->5;7->5");
  gum::LazyPropagation<double> ie(&bn);
  auto jtagr = ie.junctionTree();

  std::vector<std::string> names;
  for (const auto &elt : bn.nodes()) {
    if (elt != 3) {
      names.push_back(bn.variable(elt).name());
    }
  }

  try {
    auto jt = OTAGRUM::JunctionTree(*jtagr, names);
    std::cout << "testForSize : ERROR\n";
  } catch (OT::InvalidArgumentException &) {
    std::cout << "testForSize : OK\n";
  }
}

void testNotOKForHole() {
  auto bn =
      gum::BayesNet<double>::fastPrototype("0->1->2->3;1->4->3->5;6->5;7->5");
  bn.erase("3"); // creating a hole
  gum::LazyPropagation<double> ie(&bn);
  auto jtagr = ie.junctionTree();

  std::vector<std::string> names;
  for (const auto &elt : bn.nodes()) {
    names.push_back(bn.variable(elt).name());
  }
  try {
    auto jt = OTAGRUM::JunctionTree(*jtagr, names);
    std::cout << " testForHole : ERROR\n";
  } catch (OT::InvalidArgumentException &) {
    std::cout << "testForHole : OK\n";
  }
}

void testGetMarginal() {
  auto bn = gum::BayesNet<double>::fastPrototype(
      "a->b->c->d->h;b->e->d;e->f->h;e->g->h;e->h");
  /*
   *   a
   *    \
   *     b
   *    / \
   *   c   e
   *    \ /|\
   *     d g f       and e-h
   *      \|/
   *       h
   */
  gum::LazyPropagation<double> ie(&bn);
  auto jtagr = ie.junctionTree();
  std::vector<std::string> names;
  for (const auto &elt : bn.nodes()) {
    names.push_back(bn.variable(elt).name());
  }

  auto jt = OTAGRUM::JunctionTree(*jtagr, names);
  std::cout << jt.__str__() << std::endl;

  std::cout << "getMarginal on h,a,b\n";
  OT::Indices indices;
  indices.add(4);
  indices.add(0);
  indices.add(1);

  auto jt2 = jt.getMarginal(indices);
  std::cout << jt2.__str__() << std::endl;
}

int main(int argc, char **argv) {
  testOK();
  std::cout << "\n";

  testNotOKForSize();
  std::cout << "\n";

  testNotOKForHole();
  std::cout << "\n";

  testGetMarginal();
  return 0;
}
