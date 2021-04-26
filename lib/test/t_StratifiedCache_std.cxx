#include <iostream>
#include <openturns/Geometric.hxx>
#include <openturns/Normal.hxx>
#include <openturns/Poisson.hxx>
#include <openturns/Uniform.hxx>

#include "otagrum/StratifiedCache.hxx"

using namespace OTAGRUM;

int main(int /*argc*/, char ** /*argv*/)
{
  StratifiedCache cache;

  OT::UnsignedInteger size = 5;
  cache.set(1, "normal()", OT::Normal().getSample(size).asPoint());
  cache.set(4, "uniform()", OT::Uniform().getSample(size).asPoint());
  cache.set(1, "Poisson(0.1)", OT::Poisson(0.1).getSample(size).asPoint());
  cache.set(1, "Geometric(0.4)", OT::Geometric(0.4).getSample(size).asPoint());

  std::cout << "size : " << cache.size() << std::endl;
  std::cout << "max level : " << cache.maxLevel() << std::endl;
  std::cout << std::endl;

  for (int i = 0; i <= cache.maxLevel(); i++)
  {
    std::cout << "size level[" << i << "] :" << cache.size(i) << std::endl;
  }
  std::cout << std::endl;
  std::cout << cache.__str__() << std::endl;

  std::cout << cache.get("uniform()") << std::endl;
}
