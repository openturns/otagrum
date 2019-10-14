#include <openturns/OT.hxx>

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
  for (const auto &nod : ndag.getNodes())
  {
    std::cout << " parents(" << nod << ") : " << ndag.getParents(nod) << std::endl;
    std::cout << "children(" << nod << ") : " << ndag.getChildren(nod) << std::endl;
  }

  std::cout << "topologicalOrder : " << ndag.getTopologicalOrder() << std::endl;

  std::cout << "topologicalOrder : " << ndag.getTopologicalOrder() << std::endl;

  std::cout << std::endl << ndag.__str__("+++ +++ |") << std::endl;

  std::cout << std::endl << ndag.toDot();

  OT::Log::Show(OT::Log::ALL);
  OT::Study study("tmp.xml");
  study.add("ndag", ndag);
  study.save();

  std::cout << "\n------------------\n";
  OT::Study study2("tmp.xml");
  study2.load();
  std::cout << "Labels=" << study2.printLabels() << std::endl;
  std::cout << "\n------------------\n";

  OTAGRUM::NamedDAG dag;
  std::cout << "Before fill, " << dag.getClassName() << ", " << dag.getName() << std::endl;

  study2.fillObject("ndag", dag);

  std::cout << "        size : " << dag.getSize() << std::endl;
  std::cout << "       desc : " << dag.getDescription() << std::endl
            << std::endl;

  std::cout << "      nodes : " << dag.getNodes() << std::endl;
  std::cout << " parents(0) : " << dag.getParents(0) << std::endl;
  std::cout << "children(0) : " << dag.getChildren(0) << std::endl;

  std::cout << "topologicalOrder : " << dag.getTopologicalOrder() << std::endl;

  std::cout << std::endl << dag.__str__("+++ +++ |") << std::endl;

  std::cout << std::endl << dag.toDot();

}

int main(void)
{
  testConstructor();

  return 0;
}
