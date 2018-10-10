//
// Created by phw on 28/11/17.
//

#ifndef OTAGR_CONITNUOUSTTEST_H
#define OTAGR_CONITNUOUSTTEST_H
#include <string>
#include <vector>

#include "otagr/IndicesManip.hxx"
#include "otagr/StratifiedCache.hxx"

namespace OTAGR {
class ContinuousTTest {

private:
  /// from z-score, return p-value P(Z>z)
  static double z2p_(const double z);

  /// from z-score, return p-value P(Z \not\in [-z,z])
  static double z2p_interval_(const double z);

  /// computes K from the sample properties (size, dimension, ...)
  static OT::UnsignedInteger getK_(const OT::Sample & sample);

  /// computes the key from Indices and k
  static std::string getKey_(const OT::Indices & l, const OT::UnsignedInteger k);

  /// get the pdf of Bernstein Copula on Indice l in data
  /// if k=0 : use getK_ to find the right value
  OT::Point getPDF_(const OT::Indices & l, const OT::UnsignedInteger k = 0) const;

  /// get the pdfs of Berstein Copulae fX,fYX,fZX,FUZX
  /// allows to call getPDF_ with the same k for all copulae
  std::tuple<OT::Point, OT::Point, OT::Point, OT::Point, OT::UnsignedInteger>
  getPDFs_(const Indice Y, const Indice Z, const OT::Indices & X) const;

  mutable StratifiedCache cache_;
  OT::Sample data_;
  double alpha_;

public:
  explicit ContinuousTTest(const OT::Sample & data, const double alpha = 0.1);

  void setAlpha(const double alpha);
  double getAlpha(void) const;

  double getTTest(const Indice Y, const Indice Z, const OT::Indices & X) const;
  double getTTestWithoutCorrections(const Indice Y, const Indice Z,
                                    const OT::Indices & X) const;

  static std::tuple<double, double, bool> isIndepFromTest(const double t,
                                                          const double alpha);

  std::tuple<double, double, bool> isIndep(const Indice Y, const Indice Z,
                                           const OT::Indices & X) const;

  std::string __str__(const std::string &offset = "") const;

  void clearCache() const;
  void clearCacheLevel(const OT::UnsignedInteger level) const;
  OT::UnsignedInteger getDimension() const;
};
}; // namespace OTAGR

#endif // OTAGR_CONITNUOUSTTEST_H
