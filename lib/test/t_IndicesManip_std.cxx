#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

#include "otagrum/otagrum.hxx"

using namespace OT;
using namespace OT::Test;
using namespace OTAGRUM;

static UnsignedInteger countCombinations(const Indices &supset, UnsignedInteger size)
{
  IndicesCombinationIterator iter(supset, size);
  UnsignedInteger count = 0;
  for (iter.setFirst(); !iter.isLast(); iter.next())
    ++count;
  return count;
}

int main(int, char *[])
{
  TESTPREAMBLE;

  try
  {
    Indices empty;
    assert_equal((int) empty.getSize(), 0);

    Indices ind;
    for (UnsignedInteger i = 0; i < 15; ++i)
      ind = ind + (15 - i);
    assert_equal((int) ind.getSize(), 15);
    assert_equal(ind, Indices{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1});

    // operator+ does not duplicate existing values
    assert_equal((int) (ind + 15).getSize(), 15);

    // operator+ with Indices
    Indices ind1{1, 2, 3, 4};
    assert_equal(ind1 + 5, Indices{1, 2, 3, 4, 5});

    // operator- removes the value, missing value is a no-op
    Indices removed = ind - 5;
    assert_equal(removed, Indices{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 4, 3, 2, 1});
    assert_equal(empty, Indices());
    assert_equal((ind - 15), Indices{14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1});
    assert_equal((int) (ind - 999).getSize(), 15);
    // Combination iterator counts: C(2,0)=1, C(2,1)=2, C(2,2)=1
    Indices supset{100, 300};
    assert_equal((int) countCombinations(supset, 0), 1);
    assert_equal((int) countCombinations(supset, 1), 2);
    assert_equal((int) countCombinations(supset, 2), 1);

    // Combination iterator values for C(5,2)=10
    Indices supset5{100, 300, 400, 60, 9000};
    assert_equal((int) countCombinations(supset5, 0), 1);
    assert_equal((int) countCombinations(supset5, 1), 5);
    assert_equal((int) countCombinations(supset5, 2), 10);
    assert_equal((int) countCombinations(supset5, 3), 10);
    assert_equal((int) countCombinations(supset5, 4), 5);
    assert_equal((int) countCombinations(supset5, 5), 1);

    IndicesCombinationIterator iter(supset, 1);
    iter.setFirst();
    assert_equal(iter.current(), Indices{100});
    iter.next();
    assert_equal(iter.current(), Indices{300});
    iter.next();
    assert_equal(iter.isLast(), true);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
