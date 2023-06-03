#include <iostream>
#include <openturns/Indices.hxx>

#include "otagrum/otagrum.hxx"

using namespace OTAGRUM;

void iterCombinaison(const OT::Indices &supset, int size)
{
  std::cout << std::endl
            << "== " << size << "-Combinations from " << supset.__str__()
            << std::endl;
  IndicesCombinationIterator iter(supset, size);
  for (iter.setFirst(); !iter.isLast(); iter.next())
  {
    std::cout << iter.current() << std::endl;
  }
}

void testIndicesManip()
{
  OT::Indices indices(0);
  std::cout << indices << std::endl;
  auto ind = indices;
  for (int i = 0; i < 15; ++i)
  {
    ind = ind + (15 - i);
    std::cout << ind << std::endl;
  }

  ind = ind - 5;
  std::cout << ind << std::endl;
  std::cout << indices << std::endl;
  ind = ind - 15;
  std::cout << ind << std::endl;
  std::cout << indices << std::endl;
  /*
  std::cout << "\nlen :";
  if (ind.__len__() == 14) {
  std::cout << " OK";
  } else {
  std::cout << " fail";
  }

  std::cout << "\n4 in ind : ";
  if (isIn(ind, 4))
  std::cout << "OK";
  else
  std::cout << "fail";

  std::cout << "\n13 in ind : ";
  if (isIn(ind, 13))
  std::cout << "OK";
  else
  std::cout << "fail";

  std::cout << "\n5 not in ind : ";
  if (isIn(ind, 5))
  std::cout << "fail";
  else
  std::cout << "OK";

  std::cout << "\n20 not in ind : ";
  if (isIn(ind, 20))
  std::cout << "fail";
  else
  std::cout << "OK";

  std::cout << "\nNothing in indices : ";
  bool fail = false;
  for (OTAGRUM::Indice i = 0; i < 15; i++)
  if (isIn(indices, i)) {
  std::cout << "fail";
  fail = true;
  break;
  }
  if (!fail)
  std::cout << "OK";
  std::cout << "\n\n";

  OT::Indices ind1, ind2;
  ind1 = ind1 + 1 + 2 + 3 + 4;
  std::cout << ind1 << std::endl;
  ind2 = ind1 + 5;
  std::cout << ind2 << std::endl;
  ind2 = ind1;
  ind2 = ind2 + 5;
  std::cout << ind2 << std::endl;

  OT::Indices supset(0);
  supset = supset + 100 + 300;
  iterCombinaison(supset, 0);
  iterCombinaison(supset, 1);
  iterCombinaison(supset, 2);
  supset = supset + 400 + 60 + 9000;
  iterCombinaison(supset, 0);
  iterCombinaison(supset, 1);
  iterCombinaison(supset, 2);
  iterCombinaison(supset, 3);
  iterCombinaison(supset, 4);
  iterCombinaison(supset, 5);*/
}

int main(int /*argc*/, char ** /*argv*/)
{
  testIndicesManip();
  return EXIT_SUCCESS;
}
