#include "otagrum/ContinuousBayesianNetwork.hxx"
#include <iostream>

void testConstructor()
{
  const auto proto = "A->B->C->D;E->A->C<-E";
  const auto bn = gum::BayesNet<double>::fastPrototype(proto);

  std::cout << "      proto : " << proto << std::endl;
  std::cout << "         BN : " << bn << std::endl << std::endl;

  OTAGRUM::ContinuousBayesianNetwork cbn(bn);

  std::cout << "       size : " << cbn.getSize() << std::endl;
  std::cout << "       desc : " << cbn.getDescription() << std::endl
            << std::endl;

  std::cout << "      nodes : " << cbn.getNodes() << std::endl;
  std::cout << " parents(0) : " << cbn.getParents(0) << std::endl;
  std::cout << "children(0) : " << cbn.getChildren(0) << std::endl;
}

int main(void)
{
  testConstructor();

  return 0;
}
