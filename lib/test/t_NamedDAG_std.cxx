#include <openturns/OT.hxx>

#include "otagrum/NamedDAG.hxx"
#include <iostream>

void testConstructor()
{
  std::cout << "\n\n** Checking constructor " << std::endl;

  const auto proto = "A->C->B->D;E->A->B<-E";
  const auto bn = gum::BayesNet<double>::fastPrototype(proto);

  std::cout << "      proto : " << proto << std::endl;
  std::cout << "         BN : " << bn << std::endl << std::endl;

  OTAGRUM::NamedDAG ndag(bn);
  ndag.setName("ndagFromTest");

  std::cout << "       size : " << ndag.getSize() << std::endl;
  std::cout << "       desc : " << ndag.getDescription() << std::endl
            << std::endl;

  for (gum::NodeId nod = 0; nod < ndag.getSize(); nod++)
  {
    std::cout << " parents(" << nod << ") : " << ndag.getParents(nod)
              << std::endl;
    std::cout << "children(" << nod << ") : " << ndag.getChildren(nod)
              << std::endl;
  }

  std::cout << "topologicalOrder : " << ndag.getTopologicalOrder() << std::endl;

  std::cout << std::endl << ndag.__str__("__str__ | ") << std::endl;

  std::cout << std::endl << "todo : " << ndag.toDot();
}

void testSerialisation()
{
  std::cout << "\n\n** Checking serialization " << std::endl;
  const auto proto = "A->C->B->D;E->A->B<-E";
  const auto bn = gum::BayesNet<double>::fastPrototype(proto);
  OTAGRUM::NamedDAG ndag(bn);
  ndag.setName("ndagFromTest");

  std::cout << "Before saving, " << ndag.getClassName() << ", "
            << ndag.getName() << std::endl;
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
  std::cout << "Before fill, " << dag.getClassName() << ", " << dag.getName()
            << std::endl;

  study2.fillObject("ndag", dag);
  std::cout << "After fill, " << dag.getClassName() << ", " << dag.getName()
            << std::endl;

  std::cout << "        size : " << dag.getSize() << std::endl;
  std::cout << "       desc : " << dag.getDescription() << std::endl
            << std::endl;

  for (gum::NodeId nod = 0; nod < ndag.getSize(); nod++)
  {
    std::cout << " parents(" << nod << ") : " << ndag.getParents(nod)
              << std::endl;
    std::cout << "children(" << nod << ") : " << ndag.getChildren(nod)
              << std::endl;
  }

  std::cout << "topologicalOrder : " << dag.getTopologicalOrder() << std::endl;

  std::cout << std::endl << dag.__str__("__str__ |") << std::endl;

  std::cout << std::endl << dag.toDot();
}
void testConstructorWithErrors()
{
  std::cout << "\n\n** Checking raised exceptions " << std::endl;
  auto bn = gum::BayesNet<double>::fastPrototype("A->B->C<-D->E<-A");
  std::cout << "  - Before removing a node : " << bn << std::endl;
  bn.erase("C");
  std::cout << "  - After removing node C : " << bn << std::endl;

  std::cout << "=> Trying to build a dag with non contiguous ids :";
  try
  {
    OTAGRUM::NamedDAG d(bn);
    std::cout << " ERROR " << std::endl;
  }
  catch (OT::InvalidArgumentException)
  {
    std::cout << " OK " << std::endl;
  }
}

int main(void)
{
  testConstructor();
  testSerialisation();
  testConstructorWithErrors();

  return 0;
}
