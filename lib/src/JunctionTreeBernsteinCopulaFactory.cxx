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

#include <otagrum/ContinuousPC.hxx>
#include <otagrum/JunctionTreeBernsteinCopulaFactory.hxx>

namespace OTAGRUM
{

CLASSNAMEINIT(JunctionTreeBernsteinCopulaFactory)

static const OT::Factory<JunctionTreeBernsteinCopulaFactory>
Factory_JunctionTreeBernsteinCopulaFactory;

/* Default constructor */
JunctionTreeBernsteinCopulaFactory::JunctionTreeBernsteinCopulaFactory()
  : DistributionFactoryImplementation()
  , nbBins_(5)
  , alpha_(0.1)
  , maximumConditioningSetSize_(5)
{
  setName("JunctionTreeBernsteinCopulaFactory");
}

/* Parameter constructor */
JunctionTreeBernsteinCopulaFactory::JunctionTreeBernsteinCopulaFactory(const int nbBins,
								       const double alpha,
								       const int maximumConditioningSetSize)
  : DistributionFactoryImplementation()
  , nbBins_(nbBins)
  , alpha_(alpha)
  , maximumConditioningSetSize_(maximumConditioningSetSize)
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

OT::Distribution
JunctionTreeBernsteinCopulaFactory::build(const OT::Sample &sample) const
{
  return buildAsJunctionTreeBernsteinCopula(sample);
}

OT::Distribution JunctionTreeBernsteinCopulaFactory::build() const
{
  return buildAsJunctionTreeBernsteinCopula().clone();
}

JunctionTreeBernsteinCopula
JunctionTreeBernsteinCopulaFactory::buildAsJunctionTreeBernsteinCopula(
  const OT::Sample &sample) const
{

  if (sample.getSize() == 0)
    throw OT::InvalidArgumentException(HERE)
        << "Error: cannot build a JunctionTreeBernsteinCopula distribution from an empty "
        "sample";
  OTAGRUM::ContinuousPC learner(sample, maximumConditioningSetSize_, alpha_);
  learner.setOptimalPolicy(true);
  return JunctionTreeBernsteinCopula(learner.learnJunctionTree(), sample, nbBins_);
}

JunctionTreeBernsteinCopula
JunctionTreeBernsteinCopulaFactory::buildAsJunctionTreeBernsteinCopula() const
{
  return JunctionTreeBernsteinCopula();
}

} // namespace OTAGRUM
