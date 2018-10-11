#include <iostream>

#include <openturns/ClaytonCopulaFactory.hxx>
#include <openturns/ComposedCopula.hxx>
#include <openturns/FrankCopulaFactory.hxx>
#include <openturns/NormalCopulaFactory.hxx>

#include "otagr/ContinuousTTest.hxx"

OT::Sample getNormalSample(OT::UnsignedInteger size) {
  OT::UnsignedInteger dim = 10;

  OT::CorrelationMatrix R(dim);
  for (OT::UnsignedInteger i = 0; i < dim; i++)
    for (OT::UnsignedInteger j = 0; j < i; j++)
      R(i, j) = (i + j + 1.0) / (4.0 * dim);

  OT::Point mean(dim, 2.0);
  OT::Point sigma(dim, 3.0);
  OT::Normal distribution(mean, sigma, R);

  return distribution.getSample(size);
}

OT::Sample getSpecificInstanceSeePythonTest(OT::UnsignedInteger size) {
  auto R = OT::CorrelationMatrix(3);
  R(0, 1) = 0.9;
  R(0, 2) = 0.25;

  OT::Collection<OT::Copula> copulas;
  copulas.add(OT::NormalCopula(R));
  copulas.add(OT::FrankCopula(3.0));
  copulas.add(OT::ClaytonCopula(2.0));

  auto copula = OT::ComposedCopula(copulas);
  return copula.getSample(size);
}

void testNormalSample() {
  double t1;
  double p1;
  bool ok1;
  double t2;
  double p2;
  bool ok2;
  OT::Indices X;

  OT::Sample sample = getNormalSample(1300);
  OTAGR::ContinuousTTest test(sample);
  std::tie(t1, p1, ok1) = test.isIndep(0, 1, X);
  std::cout << test.__str__("");
  std::tie(t2, p2, ok2) = test.isIndep(8, 9, X);
  std::cout << test.__str__("");
  std::cout << "ttest value: " << t1 << "    pvalue:" << p1
            << "    test:" << (ok1 ? " OK " : " fail") << "\n";
  std::cout << "ttest value: " << t2 << "    pvalue:" << p2
            << "    test:" << (ok2 ? " fail " : " OK ") << "\n";
}

namespace OTAGR {
void testIndepsSeePythonTest() {
  double t;
  double p;
  bool ok;
  OT::Indices X;

  auto data = getSpecificInstanceSeePythonTest(3000);
  OTAGR::ContinuousTTest test(data);
  test.setAlpha(0.1);

  //  0 et 1 not independent
  std::tie(t, p, ok) = test.isIndep(0, 1, X);
  std::cout << "ttest value: " << t << "     pvalue:" << p
            << "   test:" << (ok ? " fail " : " OK") << "\n";

  // O et 4 independent
  std::tie(t, p, ok) = test.isIndep(0, 4, X);
  std::cout << "ttest value: " << t << "     pvalue:" << p
            << "   test:" << (ok ? " OK " : " fail ") << "\n";

  // O et 4 independent if 2,3
  std::tie(t, p, ok) = test.isIndep(0, 4, X + 2 + 3);
  std::cout << "ttest value: " << t << "     pvalue:" << p
            << "   test:" << (ok ? " OK " : " fail ") << "\n";

  // O et 2 independent if 1
  std::tie(t, p, ok) = test.isIndep(0, 2, X + 1);
  std::cout << "ttest value: " << t << "     pvalue:" << p
            << "   test:" << (ok ? " OK " : " fail ") << "\n";
}
}

int main(int argc, char **argv) {
  testNormalSample();
  OTAGR::testIndepsSeePythonTest();
  return EXIT_SUCCESS;
}
