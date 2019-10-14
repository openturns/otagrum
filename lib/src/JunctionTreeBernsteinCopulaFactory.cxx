//                                               -*- C++ -*-
/**
 *  @brief JunctionTreeBernsteinCopulaFactory
 *
 *  Copyright 2010-2019 Airbus-LIP6-Phimeca
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

#include <openturns/OTprivate.hxx>
#include <openturns/PersistentObjectFactory.hxx>

#include "otagrum/OTAgrumResourceMap.hxx"
#include "otagrum/ContinuousPC.hxx"
#include "otagrum/JunctionTreeBernsteinCopulaFactory.hxx"

using namespace OT;

namespace OTAGRUM
{

CLASSNAMEINIT(JunctionTreeBernsteinCopulaFactory)

static const Factory<JunctionTreeBernsteinCopulaFactory>
Factory_JunctionTreeBernsteinCopulaFactory;

/* Default constructor */
JunctionTreeBernsteinCopulaFactory::JunctionTreeBernsteinCopulaFactory()
  : DistributionFactoryImplementation()
{
  nbBins_ = ResourceMap::GetAsUnsignedInteger("JunctionTreeBernsteinCopulaFactory-DefaultBinNumber");
  alpha_ = ResourceMap::GetAsScalar("JunctionTreeBernsteinCopulaFactory-DefaultAlpha");
  maximumConditioningSetSize_ = ResourceMap::GetAsUnsignedInteger("JunctionTreeBernsteinCopulaFactory-DefaultMaximumConditioningSetSize");
}

/* Parameter constructor */
JunctionTreeBernsteinCopulaFactory::JunctionTreeBernsteinCopulaFactory(
  const UnsignedInteger nbBins, const Scalar alpha, const UnsignedInteger maximumConditioningSetSize)
  : DistributionFactoryImplementation(), nbBins_(nbBins), alpha_(alpha),
    maximumConditioningSetSize_(maximumConditioningSetSize)
{
  setName("JunctionTreeBernsteinCopulaFactory");
}

/* Virtual constructor */
JunctionTreeBernsteinCopulaFactory *
JunctionTreeBernsteinCopulaFactory::clone() const
{
  return new JunctionTreeBernsteinCopulaFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution
JunctionTreeBernsteinCopulaFactory::build(const Sample &sample) const
{
  return buildAsJunctionTreeBernsteinCopula(sample);
}

Distribution JunctionTreeBernsteinCopulaFactory::build() const
{
  return buildAsJunctionTreeBernsteinCopula().clone();
}

JunctionTreeBernsteinCopula
JunctionTreeBernsteinCopulaFactory::buildAsJunctionTreeBernsteinCopula(
  const Sample &sample) const
{

  if (sample.getSize() == 0)
    throw InvalidArgumentException(HERE)
        << "Error: cannot build a JunctionTreeBernsteinCopula distribution "
        "from an empty "
        "sample";
  OTAGRUM::ContinuousPC learner(sample, maximumConditioningSetSize_, alpha_);
  return JunctionTreeBernsteinCopula(learner.learnJunctionTree(), sample,
                                     nbBins_);
}

JunctionTreeBernsteinCopula
JunctionTreeBernsteinCopulaFactory::buildAsJunctionTreeBernsteinCopula() const
{
  return JunctionTreeBernsteinCopula();
}

} // namespace OTAGRUM
