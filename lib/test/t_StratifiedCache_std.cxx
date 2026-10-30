#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

#include "otagrum/otagrum.hxx"

using namespace OT;
using namespace OT::Test;
using namespace OTAGRUM;

int main(int, char *[])
{
  TESTPREAMBLE;

  try
  {
    StratifiedCache cache;

    Point p1{1.0, 2.0, 3.0, 4.0, 5.0};
    Point p2{5.0, 4.0, 3.0, 2.0, 1.0};
    Point p3{0.1, 0.2, 0.3, 0.4, 0.5};
    Point p4{0.5, 0.5, 0.5, 0.5, 0.5};

    cache.set(1, "normal()", p1);
    cache.set(4, "uniform()", p2);
    cache.set(1, "Poisson(0.1)", p3);
    cache.set(1, "Geometric(0.4)", p4);

    assert_equal(cache.size(), 4);
    assert_equal(cache.maxLevel(), 4);
    assert_equal(cache.size(0), 0);
    assert_equal(cache.size(1), 3);
    assert_equal(cache.size(2), 0);
    assert_equal(cache.size(3), 0);
    assert_equal(cache.size(4), 1);

    assert_equal(cache.exists("uniform()"), true);
    assert_equal(cache.exists("missing()"), false);

    assert_almost_equal(cache.get("uniform()"), p2);
    assert_almost_equal(cache.get("normal()"), p1);

    cache.clearLevel(1);
    assert_equal(cache.size(1), 0);
    assert_equal(cache.exists("normal()"), false);
    assert_equal(cache.exists("uniform()"), true);

    cache.clear();
    assert_equal(cache.size(), 0);
    assert_equal(cache.exists("uniform()"), false);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
