//                                               -*- C++ -*-
/**
 *  @brief OTAgrum_ResourceMap.cxx
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "otagrum/OTAgrumResourceMap.hxx"
#include <openturns/ResourceMap.hxx>
#include <mutex>

using namespace OT;

namespace OTAGRUM
{

OTAgrumResourceMap_init::OTAgrumResourceMap_init()
{
  static std::once_flag flag;
  std::call_once(flag, [&]()
  {
    // JunctionTreeBernsteinCopulaFactory
    ResourceMap::AddAsScalar("JunctionTreeBernsteinCopulaFactory-DefaultAlpha", 0.1);
    ResourceMap::AddAsUnsignedInteger("JunctionTreeBernsteinCopulaFactory-DefaultBinNumber", 5);
    ResourceMap::AddAsUnsignedInteger("JunctionTreeBernsteinCopulaFactory-DefaultMaximumConditioningSetSize", 5);
    // ContinuousBayesianNetworkFactory
    ResourceMap::AddAsBool("ContinuousBayesianNetworkFactory-WorkInCopulaSpace", true);
    ResourceMap::AddAsBool("ContinuousBayesianNetworkFactory-UseBetaCopula", true);
    ResourceMap::AddAsScalar("ContinuousBayesianNetworkFactory-DefaultAlpha", 0.1);
    ResourceMap::AddAsUnsignedInteger("ContinuousBayesianNetworkFactory-DefaultMaximumConditioningSetSize", 5);
    ResourceMap::AddAsUnsignedInteger("ContinuousBayesianNetworkFactory-MaximumDiscreteSupport", 10);
  });
}

} // namespace OTAGRUM
