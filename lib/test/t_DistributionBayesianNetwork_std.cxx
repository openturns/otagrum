#include <iostream>

#include <openturns/OT.hxx>
#include <openturns/OTtestcode.hxx>

#include "otagrum/otagrum.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  try
  {
    RandomGenerator::SetSeed(0);
    ResourceMap::SetAsBool("Distribution-Parallel", false);
    const auto bn = gum::BayesNet<double>::fastPrototype("A->C->B;");
    OTAGRUM::NamedDAG ndag(bn);
    OTAGRUM::DistributionBayesianNetwork::DistributionCollection joints(ndag.getSize());
    for (UnsignedInteger i = 0; i < ndag.getSize(); ++i)
    {
      const UnsignedInteger dim = 1 + ndag.getParents(i).getSize();
      CorrelationMatrix R(dim);
      for (UnsignedInteger j = 0; j < dim; ++j)
        for (UnsignedInteger k = 0; k < j; ++k)
          R(j, k) = 0.8;
      joints[i] = Normal(Point(dim, 0.0), Point(dim, 1.0), R);
    }
    Distribution dbn(OTAGRUM::DistributionBayesianNetwork(ndag, joints));
    std::cout << "dbn=" << dbn << std::endl;
    std::cout << "pdf=" << dbn.computePDF(Point(dbn.getDimension(), 0.5)) << std::endl;
    std::cout << "realization=" << dbn.getRealization() << std::endl;
    std::cout << "parameterDimension=" << dbn.getParameterDimension() << std::endl;

    Log::Show(Log::TRACE);
    DistributionValidation validator(dbn);
    validator.skipDDF();
    validator.skipGradient();
    validator.skipComplementaryCDF();
    validator.skipConditional();
    validator.skipTransformation();
    validator.skipEntropy();
    validator.skipCharacteristicFunction();
    validator.skipGeneratingFunction();
    validator.skipMinimumVolumeInterval();
    validator.skipMinimumVolumeLevelSet();
    validator.skipConfidenceInterval();
    validator.skipCorrelation();
    validator.skipDependenceMeasures();
    validator.run();
    std::cout << "validation ok" << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
