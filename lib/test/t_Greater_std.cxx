#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

#include "otagrum/otagrum.hxx"
#include "otagrum/Greater.hxx"

using namespace OT;
using namespace OT::Test;
using namespace OTAGRUM;

int main(int, char *[])
{
  TESTPREAMBLE;

  try
  {
    // RankedTriple
    Indices u{7, 8};
    RankedTriple triple(1, 2, 3, u);
    assert_equal((int) triple.getX(), 1);
    assert_equal((int) triple.getY(), 2);
    assert_equal((int) triple.getZ(), 3);
    assert_equal(triple.getU(), u);
    RankedTriple copy = triple;
    assert_equal((int) copy.getX(), (int) triple.getX());

    // GreaterPairOn2nd
    GreaterPairOn2nd greaterPair;
    std::pair<RankedTriple, double> high{RankedTriple(0, 1, 2, Indices()), 0.9};
    std::pair<RankedTriple, double> low{RankedTriple(3, 4, 5, Indices()), 0.1};
    assert_equal(greaterPair(high, low), true);
    assert_equal(greaterPair(low, high), false);
    assert_equal(greaterPair(high, high), false);

    // GreaterTupleOnLast
    GreaterTupleOnLast greaterTuple;
    using Triplet = std::tuple<UnsignedInteger, UnsignedInteger, UnsignedInteger>;
    using ScoredTriplet = std::tuple<Triplet *, double, double, double>;
    Triplet k1{0, 1, 2};
    Triplet k2{3, 4, 5};

    // Same sign of I, differing max(pXZ, pYZ): higher max wins
    {
      ScoredTriplet e1{&k1, 0.5, 0.9, 0.1};
      ScoredTriplet e2{&k2, 0.5, 0.2, 0.3};
      assert_equal(greaterTuple(e1, e2), true);
      assert_equal(greaterTuple(e2, e1), false);
    }
    // Opposite signs of I: lower I wins
    {
      ScoredTriplet e3{&k1, 0.5, 0.5, 0.5};
      ScoredTriplet e4{&k2, -0.5, 0.5, 0.5};
      assert_equal(greaterTuple(e3, e4), false);
      assert_equal(greaterTuple(e4, e3), true);
    }
    // Same positive sign, equal max: higher |I| wins
    {
      ScoredTriplet e5{&k1, 0.3, 0.5, 0.5};
      ScoredTriplet e6{&k2, 0.9, 0.5, 0.5};
      assert_equal(greaterTuple(e5, e6), false);
      assert_equal(greaterTuple(e6, e5), true);
    }
    // Same negative sign, equal max: higher |I| wins
    {
      ScoredTriplet e7{&k1, -0.3, 0.5, 0.5};
      ScoredTriplet e8{&k2, -0.9, 0.5, 0.5};
      assert_equal(greaterTuple(e7, e8), false);
      assert_equal(greaterTuple(e8, e7), true);
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
