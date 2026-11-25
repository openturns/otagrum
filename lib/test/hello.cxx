#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
int main() {
  auto bn = gum::BayesNet<float>();
  auto a = bn.add(gum::LabelizedVariable("A", "", 2));
  auto b = bn.add(gum::LabelizedVariable("B", "", 2));
  bn.addArc(a, b);
  bn.cpt(a).fillWith({0.3f, 0.7f});
  bn.cpt(b).fillWith({0.9f, 0.1f, 0.2f, 0.8f});
  gum::LazyPropagation<float> inf(&bn);
  inf.makeInference();
  std::cout << inf.posterior(b) << std::endl;
}
