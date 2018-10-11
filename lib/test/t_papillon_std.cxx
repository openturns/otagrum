#include <iostream>

#include <openturns/Indices.hxx>
#include <openturns/Sample.hxx>

#include <openturns/ClaytonCopulaFactory.hxx>
#include <openturns/ComposedCopula.hxx>
#include <openturns/FrankCopulaFactory.hxx>
#include <openturns/NormalCopulaFactory.hxx>

#include "otagr/ContinuousPC.hxx"

OT::Sample getSpecificInstance(OT::UnsignedInteger size) {
  OT::Collection<OT::Copula> copulas;

  OT::CorrelationMatrix R(3);
  R(0, 1) = 0.8;
  R(0, 2) = 0.5;

  // copulas.add(OT::FrankCopula(3.0));
  copulas.add(OT::NormalCopula(R));
  // copulas.add(OT::ClaytonCopula(2.0));

  auto copula = OT::ComposedCopula(copulas);
  return copula.getSample(size);
}

void learn(std::string filename, int level = 5, double alpha = 0.05,
           bool optimalPolicy = true, bool verbose = true) {

  // std::string filename =
  //    ;
  auto sample1 = OT::Sample::ImportFromCSVFile(filename);
  std::cout << "sample size : " << sample1.getSize() << std::endl;
  std::cout << "sample dim : " << sample1.getDimension() << std::endl;

  OTAGR::ContinuousPC learner(sample1, 5, 0.1);
  learner.setVerbosity(true);
  learner.setOptimalPolicy(true);
  auto g = learner.getSkeleton();
  std::cout << g << std::endl;
}

int main(int argc, char **argv) {
//   learn("minipapillon.csv");
  learn("BlockCopula_increasing_blocks_dimension_20_size_10000.csv");
  return EXIT_SUCCESS;
}
