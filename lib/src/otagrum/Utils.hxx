//                                               -*- C++ -*-
/**
 *  @brief Utils
 *
 *  Copyright 2010-2020 Airbus-LIP6-Phimeca
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

#ifndef OTAGRUM_UTILS_HXX
#define OTAGRUM_UTILS_HXX

#include <openturns/Distribution.hxx>
#include <openturns/DistributionImplementation.hxx>
#include <openturns/PersistentObject.hxx>
#include <openturns/Point.hxx>
#include <openturns/StorageManager.hxx>
#include <openturns/UserDefined.hxx>

#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/discretizedVariable.h>

#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM
{

class OTAGRUM_API Utils
{
public:

  static gum::Potential<double> Discretize(const OT::DistributionImplementation &distribution,
      const gum::DiscretizedVariable<double> &v);

  /* Helper function to discretize a continuous distribution */
  static gum::Potential<double> Discretize(const OT::Distribution &distribution,
      const gum::DiscretizedVariable<double> &v,
      bool isTruncated = true);

  static OT::Distribution FromMarginal(const gum::Potential<double> &pot);

  static OT::Distribution FromPotential(const gum::Potential<double> &pot);

  static OT::Indices FromNodeSet(const gum::NodeSet &clique);

  // Underflow of exponential is common practice in numerical routines,
  // so handle it here.
  static inline float FastPow2(float p)
  {
    float offset = (p < 0) ? 1.0f : 0.0f;
    float clipp = (p < -126) ? -126.0f : p;
    int w = clipp;
    float z = clipp - w + offset;
    union
    {
      uint32_t i;
      float f;
    } v = { static_cast<uint32_t> ( (1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) ) };

    return v.f;
  }

  static inline float FastExp(float p)
  {
    return FastPow2 (1.442695040f * p);
  }

  static inline float FasterPow2(float p)
  {
    float clipp = (p < -126) ? -126.0f : p;
    union
    {
      uint32_t i;
      float f;
    } v = { static_cast<uint32_t> ( (1 << 23) * (clipp + 126.94269504f) ) };
    return v.f;
  }

  static inline float FasterExp(float p)
  {
    return FasterPow2 (1.442695040f * p);
  }

private:
  Utils();
};

} /* namespace OTAGRUM */

#endif /* OTAGRUM_UTILS_HXX */
