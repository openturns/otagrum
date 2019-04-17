#include "otagrum/NamedDAG.hxx"
#include <iostream>

void testConstructor()
{
  const auto proto = "A->B->C->D;E->A->C<-E";
  const auto bn = gum::BayesNet<double>::fastPrototype(proto);

  std::cout << "      proto : " << proto << std::endl;
  std::cout << "         BN : " << bn << std::endl << std::endl;

  OTAGRUM::NamedDAG ndag(bn);

  std::cout << "       size : " << ndag.getSize() << std::endl;
  std::cout << "       desc : " << ndag.getDescription() << std::endl
            << std::endl;

  std::cout << "      nodes : " << ndag.getNodes() << std::endl;
  std::cout << " parents(0) : " << ndag.getParents(0) << std::endl;
  std::cout << "children(0) : " << ndag.getChildren(0) << std::endl;

  std::cout << "topologicalOrder : " << ndag.getTopologicalOrder() << std::endl;

  std::cout << std::endl << ndag.__str__("+++ +++ |") << std::endl;

  std::cout << std::endl << ndag.toDot();
  OT::Study study("tmp.xml");
  study.add("ndag", ndag);
  study.save();
  OT::Study study2("tmp.xml");
  study2.load();
  NamedDag dag;
  study2.fill("ndag", dag);
}

int main(void)
{
  testConstructor();

  return 0;
}
