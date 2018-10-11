#include <iostream>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/multidim/potential.h>
#include <agrum/variables/discretizedVariable.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/rangeVariable.h>

#include <openturns/Exception.hxx>
#include <openturns/Point.hxx>
#include <openturns/Uniform.hxx>

#include "otagrum/Utils.hxx"

using namespace OTAGRUM;

void test_basics() {
  gum::DiscretizedVariable<double> v("v", "v",
                                     std::vector<double>{0, 4, 10, 15, 30, 40});
  gum::DiscretizedVariable<double> w("w", "w",
                                     std::vector<double>{-1, 4, 10, 30, 40});

  std::cout << "\n** From OT::Distribution to gum::Potential:\n";

  auto unifDistrib = OT::Uniform(0.0, 40.0);
  gum::Potential<double> pv = Utils::Discretize(unifDistrib, v);
  gum::Potential<double> pw = Utils::Discretize(unifDistrib, w);
  std::cout << pv << std::endl;
  std::cout << pw << std::endl;

  std::cout << "\nCatching InvalidArgumentException for bad support :";
  try {
    std::cout << Utils::Discretize(OT::Uniform(1.0, 100.0), w) << std::endl;
    std::cout << "Failed !\n";
  } catch (OT::InvalidArgumentException) {
    std::cout << "OK\n";
  }

  std::cout << "\n** Back to OT::Distribution :\n";
  std::cout << Utils::FromMarginal(pv) << std::endl;
  std::cout << Utils::FromMarginal(pw) << std::endl;

  std::cout << "\n** From RangeVariable\n";
  gum::RangeVariable x("x", "x", 3, 10);
  gum::Potential<double> px;
  px.add(x);
  px.fillWith({1, 2, 3, 4, 5, 6, 7, 8}).normalize();
  std::cout << px << std::endl;
  std::cout << Utils::FromMarginal(px) << std::endl;
}

void test_fromMarginal() {
  {
    std::cout << "\n** From LabelizedVariable\n";
    gum::LabelizedVariable y("y", "y", 0);
    y.addLabel("True").addLabel("Maybe").addLabel("False");
    gum::Potential<double> py;
    py.add(y);
    py.fillWith({2, 8, 4}).normalize();
    std::cout << py << std::endl;
    std::cout << Utils::FromMarginal(py) << std::endl;
  }
  {
    std::cout << "\n** From LabelizedVariable but numerical\n";
    gum::LabelizedVariable y("y", "y", 0);
    y.addLabel("1").addLabel("1.5").addLabel("3.15");
    gum::Potential<double> py;
    py.add(y);
    py.fillWith({2, 8, 4}).normalize();
    std::cout << py << std::endl;
    std::cout << Utils::FromMarginal(py) << std::endl;
  }
  {
    std::cout << "\n** From LabelizedVariable but partially numerical\n";
    gum::LabelizedVariable y("y", "y", 0);
    y.addLabel("1").addLabel("foo").addLabel("3.15");
    gum::Potential<double> py;
    py.add(y);
    py.fillWith({2, 8, 4}).normalize();
    std::cout << py << std::endl;
    std::cout << Utils::FromMarginal(py) << std::endl;
  }
  {
    std::cout << "\n** From Discretized\n";
    gum::DiscretizedVariable<double> y("y", "y", {0, 0.5, 1.5, 2, 2.3, 10});
    gum::Potential<double> py;
    py.add(y);
    py.fillWith({2, 8, 4}).normalize();
    std::cout << py << std::endl;
    std::cout << Utils::FromMarginal(py) << std::endl;
  }
}

void test_fromPotential() {
  gum::LabelizedVariable y("y", "y", 0);
  y.addLabel("True").addLabel("Maybe").addLabel("False");

  gum::DiscretizedVariable<double> z("z", "z", {0, 0.5, 1.5});

  gum::Potential<double> p;
  p.add(y);
  p.add(z);
  p.fillWith({1, 2, 3, 4, 5, 6}).normalize();

  std::cout << p << std::endl;

  auto distribution = Utils::FromPotential(p);

  std::cout << "Marginal 0 " << distribution.getMarginal(0) << std::endl;
  std::cout << "MarginalPotential 0 " << p.margSumIn({&y}) << std::endl;

  std::cout << "Marginal 1 " << distribution.getMarginal(1) << std::endl;
  std::cout << "MarginalPotential 1 " << p.margSumIn({&z}) << std::endl;
}

void test_fromInference() {
  gum::BayesNet<double> bn;
  bn.add(gum::DiscretizedVariable<double>("A", "A", {1, 1.5, 2, 2.5, 3, 4}));
  bn.add(gum::LabelizedVariable("B", "B", {"chaud", "tiede", "froid"}));
  bn.add(gum::RangeVariable("C", "C", 1, 4));
  bn.addArc("A", "C");
  bn.addArc("C", "B");
  bn.generateCPTs();

  gum::LazyPropagation<double> ie(&bn);
  ie.addJointTarget({0, 2});
  ie.addTarget(1);
  ie.makeInference();

  std::cout << ie.jointPosterior({0, 2})<<std::endl;
  std::cout << Utils::FromPotential(ie.jointPosterior({0, 2}))<<std::endl;

  std::cout << ie.posterior(0)<<std::endl;
  std::cout << Utils::FromPotential(ie.posterior(0))<<std::endl;

  std::cout << ie.posterior(1)<<std::endl;
  std::cout << Utils::FromPotential(ie.posterior(1))<<std::endl;

  std::cout << ie.posterior(2)<<std::endl;
  std::cout << Utils::FromPotential(ie.posterior(2))<<std::endl;

}

int main(int argc, char **argv) {
  // test_basics();
  // test_fromMarginal();
  // test_fromPotential();
  test_fromInference();
  return 0;
}
