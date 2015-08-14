#include <iostream>
// aGrUM includes
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/discretizedVariable.h>
#include <agrum/BN/inference/lazyPropagation.h>
// OT includes
#include <NumericalPoint.hxx>
#include <Collection.hxx>
#include <Uniform.hxx>
#include <Distribution.hxx>
// OTBayesNet
#include "BayesNetAgrum.hxx"

using namespace gum;
using namespace OT;
using namespace OT::Base::Type;
using namespace OT::Base::Stat;
using namespace OT::Uncertainty::Distribution;
using namespace OT::Uncertainty::Model;

typedef OT::Base::Type::Collection<Distribution>             DistributionCollection;

int main(int argc, char **argv)
{
  // Create an empty BN
  BayesNet<float> myBN;
  // Create variables
  LabelizedVariable light("Light", "Light", 0);
  LabelizedVariable moisture("Moisture", "Moisture", 0);
  DiscretizedVariable<float> height("Height", "Height");
  // Create labels and ticks
  light.addLabel("Dim");
  light.addLabel("Bright");
  int lightSize(light.domainSize());
  moisture.addLabel("Dry");
  moisture.addLabel("Wet");
  int moistureSize(moisture.domainSize());
  NumericalPoint data;
  for (UnsignedInteger i = 0; i <= 10; ++i)
    data.add(10.0 * i);
  int heightSize(data.getSize() - 1);
  for (int i = 0; i < heightSize + 1; ++i)
    height.addTick(data[i]);
  // Add variables to the net
  gum::Id indexLight(myBN.addVariable(light));
  gum::Id indexMoisture(myBN.addVariable(moisture));
  gum::Id indexHeight(myBN.addVariable(height));
  // Create arcs
  myBN.insertArc(indexLight, indexMoisture);
  myBN.insertArc(indexLight, indexHeight);
  myBN.insertArc(indexMoisture, indexHeight);
  // Create conditional probability tables
  {
    const Potential<float>& probaLight(myBN.cpt(indexLight));
    const int n = lightSize;
    const float t[] = {0.5, 0.5};
    const std::vector<float> v(t, t + n);
    probaLight.fillWith(v);
  }
  {
    const Potential<float>& probaMoisture(myBN.cpt(indexMoisture));
    const int n = lightSize * moistureSize;
    const float t[] = {0.2, 0.8,
                       0.6, 0.4};
    const std::vector<float> v(t, t + n);
    probaMoisture.fillWith(v);
  }
  {
    const Potential<float>& probaHeight(myBN.cpt(indexHeight));
    Uniform heightWhenDimAndDry(0.0, 40.0);
    Uniform heightWhenBrightAndDry(0.0, 20.0);
    Uniform heightWhenDimAndWet(10.0, 60.0);
    Uniform heightWhenBrightAndWet(40.0, 100.0);
    BayesNetAgrum::DistributionCollection collection;
    collection.add(heightWhenDimAndDry);
    collection.add(heightWhenBrightAndDry);
    collection.add(heightWhenDimAndWet);
    collection.add(heightWhenBrightAndWet);
    probaHeight.fillWith(BayesNetAgrum::Discretize(collection, data));
  }
  BayesNetAgrum otbn(myBN);
  std::cout << "otbn=" << otbn << std::endl;
  otbn.exportToBIFFile("test.bif");
  std::cout << "Evidence: Light=Bright" << std::endl;
  otbn.setEvidence("Light", "Bright");
  Distribution heightDistribution(otbn.getMarginal("Height"));
  std::cout << "heightDistribution=" << heightDistribution << std::endl;
  std::cout << "proba(height < 30.0)=" << heightDistribution.computeCDF(30.0) << std::endl;
  std::cout << "Evidence: Light=Dim" << std::endl;
  otbn.setEvidence("Light", "Dim");
  heightDistribution = otbn.getMarginal("Height");
  std::cout << "heightDistribution=" << heightDistribution << std::endl;
  std::cout << "proba(height < 30.0)=" << heightDistribution.computeCDF(30.0) << std::endl;
  return 0;
}
