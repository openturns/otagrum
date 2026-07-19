#include <iostream>
#include <tuple>
#include <utility>

#include "otagrum/otagrum.hxx"
#include "otagrum/Greater.hxx"

using namespace OTAGRUM;

void printCheck(const std::string &label, bool actual, bool expected)
{
  std::cout << label << " : " << ((actual == expected) ? "OK" : "fail")
            << std::endl;
}

void testRankedTriple()
{
  std::cout << std::endl << "== RankedTriple ==" << std::endl;

  OT::Indices u;
  u.add(7);
  u.add(8);
  RankedTriple triple(1, 2, 3, u);

  printCheck("getX", triple.getX() == 1, true);
  printCheck("getY", triple.getY() == 2, true);
  printCheck("getZ", triple.getZ() == 3, true);
  printCheck("getU", triple.getU() == u, true);

  // Copies share the same underlying tuple (cheap gum::Heap-friendly copy).
  RankedTriple copy = triple;
  printCheck("copy sees the same X", copy.getX() == triple.getX(), true);
}

void testGreaterPairOn2nd()
{
  std::cout << std::endl << "== GreaterPairOn2nd ==" << std::endl;
  GreaterPairOn2nd greater;

  std::pair<RankedTriple, double> high{RankedTriple(0, 1, 2, OT::Indices()), 0.9};
  std::pair<RankedTriple, double> low{RankedTriple(3, 4, 5, OT::Indices()), 0.1};

  printCheck("high > low", greater(high, low), true);
  printCheck("low > high", greater(low, high), false);
  printCheck("high > high", greater(high, high), false);
}

void testGreaterTupleOnLast()
{
  std::cout << std::endl << "== GreaterTupleOnLast ==" << std::endl;
  GreaterTupleOnLast greater;

  typedef std::tuple<OT::UnsignedInteger, OT::UnsignedInteger, OT::UnsignedInteger>
  Triplet;
  typedef std::tuple<Triplet *, double, double, double> ScoredTriplet;

  Triplet k1{0, 1, 2};
  Triplet k2{3, 4, 5};

  // Same sign of I, differing max(pXZ, pYZ): higher max wins.
  {
    ScoredTriplet e1{&k1, 0.5, 0.9, 0.1};  // max = 0.9
    ScoredTriplet e2{&k2, 0.5, 0.2, 0.3};  // max = 0.3
    printCheck("differing max, e1 > e2", greater(e1, e2), true);
    printCheck("differing max, e2 > e1", greater(e2, e1), false);
  }

  // Opposite signs of I: lower I wins, regardless of max(pXZ, pYZ).
  {
    ScoredTriplet e3{&k1, 0.5, 0.5, 0.5};
    ScoredTriplet e4{&k2, -0.5, 0.5, 0.5};
    printCheck("opposite signs, e3 > e4", greater(e3, e4), false);
    printCheck("opposite signs, e4 > e3", greater(e4, e3), true);
  }

  // Same (non-negative) sign, equal max(pXZ, pYZ): higher |I| wins.
  {
    ScoredTriplet e5{&k1, 0.3, 0.5, 0.5};
    ScoredTriplet e6{&k2, 0.9, 0.5, 0.5};
    printCheck("equal max, positive I, e5 > e6", greater(e5, e6), false);
    printCheck("equal max, positive I, e6 > e5", greater(e6, e5), true);
  }

  // Same (negative) sign, equal max(pXZ, pYZ): higher |I| wins.
  {
    ScoredTriplet e7{&k1, -0.3, 0.5, 0.5};
    ScoredTriplet e8{&k2, -0.9, 0.5, 0.5};
    printCheck("equal max, negative I, e7 > e8", greater(e7, e8), false);
    printCheck("equal max, negative I, e8 > e7", greater(e8, e7), true);
  }
}

int main(int /*argc*/, char ** /*argv*/)
{
  testRankedTriple();
  testGreaterPairOn2nd();
  testGreaterTupleOnLast();
  return EXIT_SUCCESS;
}
