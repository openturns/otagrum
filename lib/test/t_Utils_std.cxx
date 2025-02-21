#include <iostream>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/base/variables/rangeVariable.h>

#include <openturns/Uniform.hxx>

#include "otagrum/otagrum.hxx"

using namespace OTAGRUM;

void test_basics()
{
  gum::DiscretizedVariable<double> v("v", "v",
                                     std::vector<double> {0, 4, 10, 15, 30, 40});
  gum::DiscretizedVariable<double> w("w", "w",
                                     std::vector<double> {-1, 4, 10, 30, 40});

  std::cout << "\n** From OT::Distribution to gum::Tensor:\n";

  auto unifDistrib = OT::Uniform(0.0, 40.0);
  gum::Tensor<double> pv = Utils::Discretize(unifDistrib, v);
  gum::Tensor<double> pw = Utils::Discretize(unifDistrib, w);
  std::cout << pv << std::endl;
  std::cout << pw << std::endl;

  std::cout << "\nCatching InvalidArgumentException for bad support :";
  try
  {
    std::cout << Utils::Discretize(OT::Uniform(1.0, 100.0), w) << std::endl;
    std::cout << "Failed !\n";
  }
  catch (OT::InvalidArgumentException &)
  {
    std::cout << "OK\n";
  }

  std::cout << "\n** Back to OT::Distribution :\n";
  std::cout << Utils::FromMarginal(pv) << std::endl;
  std::cout << Utils::FromMarginal(pw) << std::endl;

  std::cout << "\n** From RangeVariable\n";
  gum::RangeVariable x("x", "x", 3, 10);
  gum::Tensor<double> px;
  px.add(x);
  px.fillWith({1, 2, 3, 4, 5, 6, 7, 8}).normalize();
  std::cout << px << std::endl;
  std::cout << Utils::FromMarginal(px) << std::endl;
}

void test_fromMarginal()
{
  {
    std::cout << "\n** From LabelizedVariable\n";
    gum::LabelizedVariable y("y", "y", 0);
    y.addLabel("True").addLabel("Maybe").addLabel("False");
    gum::Tensor<double> py;
    py.add(y);
    py.fillWith({2, 8, 4}).normalize();
    std::cout << py << std::endl;
    std::cout << Utils::FromMarginal(py) << std::endl;
  }
  {
    std::cout << "\n** From LabelizedVariable but numerical\n";
    gum::LabelizedVariable y("y", "y", 0);
    y.addLabel("1").addLabel("1.5").addLabel("3.15");
    gum::Tensor<double> py;
    py.add(y);
    py.fillWith({2, 8, 4}).normalize();
    std::cout << py << std::endl;
    std::cout << Utils::FromMarginal(py) << std::endl;
  }
  {
    std::cout << "\n** From LabelizedVariable but partially numerical\n";
    gum::LabelizedVariable y("y", "y", 0);
    y.addLabel("1").addLabel("foo").addLabel("3.15");
    gum::Tensor<double> py;
    py.add(y);
    py.fillWith({2, 8, 4}).normalize();
    std::cout << py << std::endl;
    std::cout << Utils::FromMarginal(py) << std::endl;
  }
  {
    std::cout << "\n** From Discretized\n";
    gum::DiscretizedVariable<double> y("y", "y", {0, 0.5, 1.5, 2, 2.3, 10});
    gum::Tensor<double> py;
    py.add(y);
    py.fillWith({2, 8, 4}).normalize();
    std::cout << py << std::endl;
    std::cout << Utils::FromMarginal(py) << std::endl;
  }
}

void test_fromTensor()
{
  gum::LabelizedVariable y("y", "y", 0);
  y.addLabel("True").addLabel("Maybe").addLabel("False");

  gum::DiscretizedVariable<double> z("z", "z", {0, 0.5, 1.5});

  gum::Tensor<double> p;
  p.add(y);
  p.add(z);
  p.fillWith({1, 2, 3, 4, 5, 6}).normalize();

  std::cout << p << std::endl;

  auto distribution = Utils::FromTensor(p);

  std::cout << "Marginal 0 " << distribution.getMarginal(0) << std::endl;
  std::cout << "MarginalTensor 0 " << p.sumIn({&y}) << std::endl;

  std::cout << "Marginal 1 " << distribution.getMarginal(1) << std::endl;
  std::cout << "MarginalTensor 1 " << p.sumIn({&z}) << std::endl;
}

void test_fromInference()
{
  gum::initRandom(10);
  gum::BayesNet<double> bn;
  bn.add(gum::DiscretizedVariable<double>("A", "A", {1, 1.5, 2, 2.5, 3, 4}));
  bn.add(gum::LabelizedVariable("B", "B", {"chaud", "tiede", "froid"}));
  bn.add(gum::RangeVariable("C", "C", 1, 4));
  bn.addArc("A", "C");
  bn.addArc("C", "B");
  bn.cpt("A").fillWith({1, 2, 3, 4, 5}).normalize();
  bn.cpt("C").fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}).normalizeAsCPT();
  bn.cpt("B").fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}).normalizeAsCPT();

  gum::LazyPropagation<double> ie(&bn);
  ie.addJointTarget({0, 2});
  ie.addTarget(1);
  ie.makeInference();

  std::cout << ie.jointPosterior({0, 2}).putFirst(&bn.variable("A")) << std::endl;
  std::cout << Utils::FromTensor(ie.jointPosterior({0, 2}).putFirst(&bn.variable("A"))) << std::endl;

  std::cout << ie.posterior(0) << std::endl;
  std::cout << Utils::FromTensor(ie.posterior(0)) << std::endl;

  std::cout << ie.posterior(1) << std::endl;
  std::cout << Utils::FromTensor(ie.posterior(1)) << std::endl;

  std::cout << ie.posterior(2) << std::endl;
  std::cout << Utils::FromTensor(ie.posterior(2)) << std::endl;

}

int main(int /*argc*/, char ** /*argv*/)
{
  //test_basics();
  //test_fromMarginal();
  //test_fromTensor();
  test_fromInference();
  return 0;
}
