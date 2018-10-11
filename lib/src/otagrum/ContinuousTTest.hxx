//                                               -*- C++ -*-
/**
 *  @brief ContinuousTTest
 *
 *  Copyright 2010-2018 Airbus-LIP6-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef OTAGRUM_CONITNUOUSTTEST_HXX
#define OTAGRUM_CONITNUOUSTTEST_HXX

#include <string>
#include <vector>

#include "otagrum/IndicesManip.hxx"
#include "otagrum/StratifiedCache.hxx"

namespace OTAGRUM {
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
  getPDFs_(const OT::UnsignedInteger Y, const OT::UnsignedInteger Z, const OT::Indices & X) const;

  mutable StratifiedCache cache_;
  OT::Sample data_;
  double alpha_;

public:
  explicit ContinuousTTest(const OT::Sample & data, const double alpha = 0.1);

  void setAlpha(const double alpha);
  double getAlpha(void) const;

  double getTTest(const OT::UnsignedInteger Y, const OT::UnsignedInteger Z, const OT::Indices & X) const;
  double getTTestWithoutCorrections(const OT::UnsignedInteger Y, const OT::UnsignedInteger Z,
                                    const OT::Indices & X) const;

  static std::tuple<double, double, bool> isIndepFromTest(const double t,
                                                          const double alpha);

  std::tuple<double, double, bool> isIndep(const OT::UnsignedInteger Y, const OT::UnsignedInteger Z,
                                           const OT::Indices & X) const;

  std::string __str__(const std::string &offset = "") const;

  void clearCache() const;
  void clearCacheLevel(const OT::UnsignedInteger level) const;
  OT::UnsignedInteger getDimension() const;
};
}; // namespace OTAGRUM

#endif // OTAGRUM_CONITNUOUSTTEST_HXX