//                                               -*- C++ -*-
/**
 *  @brief ContinuousTTest
 *
 *  Copyright 2010-2025 Airbus-LIP6-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef OTAGRUM_CONTINUOUSTTEST_HXX
#define OTAGRUM_CONTINUOUSTTEST_HXX

#include <string>
#include <vector>

#include "otagrum/IndicesManip.hxx"
#include "otagrum/StratifiedCache.hxx"

#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM
{

class OTAGRUM_API ContinuousTTest : public OT::Object
{
public:
  explicit ContinuousTTest(const OT::Sample & data,
                           const OT::Scalar alpha = 0.1);

  void setAlpha(const double alpha);
  double getAlpha(void) const;

  double getTTest(const OT::UnsignedInteger Y,
                  const OT::UnsignedInteger Z,
                  const OT::Indices & X) const;

  double getTTestWithoutCorrections(const OT::UnsignedInteger Y,
                                    const OT::UnsignedInteger Z,
                                    const OT::Indices & X) const;

  static std::tuple<double, double, bool> isIndepFromTest(const double t,
      const double alpha);

  std::tuple<double, double, bool> isIndep(const OT::UnsignedInteger Y,
      const OT::UnsignedInteger Z,
      const OT::Indices & X) const;

  std::string __str__(const std::string &offset = "") const;

  void clearCache() const;
  void clearCacheLevel(const OT::UnsignedInteger level) const;
  OT::UnsignedInteger getDimension() const;

  OT::Description getDataDescription() const;

  /// computes K from the sample properties (size, dimension, ...)
  static OT::UnsignedInteger GetK(const OT::UnsignedInteger size,
                                  const OT::UnsignedInteger dimension);

private:
  /// computes the key from Indices and k
  static std::string GetKey(const OT::Indices & l,
                            const OT::UnsignedInteger k);

  /// get the log-pdf of Bernstein Copula on Indice l in data
  /// if k=0 : use getK_ to find the right value
  OT::Point getLogPDF(const OT::Indices & l,
                      const OT::UnsignedInteger k) const;

  /// get the log-pdfs of Berstein Copulae fX,fYX,fZX,FUZX
  /// allows one to call getLogPDF_ with the same k for all copulae
  std::tuple<OT::Point, OT::Point, OT::Point, OT::Point, OT::UnsignedInteger>
  getLogPDFs(const OT::UnsignedInteger Y,
             const OT::UnsignedInteger Z,
             const OT::Indices & X) const;

  mutable StratifiedCache cache_;
  OT::Sample data_;
  bool verbose_;
  double alpha_;  //Confidence threshold

};

} // namespace OTAGRUM

#endif // OTAGRUM_CONTINUOUSTTEST_HXX
