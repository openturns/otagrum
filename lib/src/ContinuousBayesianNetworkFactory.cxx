//                                               -*- C++ -*-
/**
 *  @brief ContinuousBayesianNetworkFactory
 *
 *  Copyright 2010-2024 Airbus-LIP6-Phimeca
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

#include <openturns/OTprivate.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/UserDefined.hxx>
#include <openturns/UserDefinedFactory.hxx>
#include <openturns/HistogramFactory.hxx>
#include <openturns/IndependentCopula.hxx>
#include <openturns/BernsteinCopulaFactory.hxx>
#include <openturns/EmpiricalBernsteinCopula.hxx>

#include "otagrum/ContinuousPC.hxx"
#include "otagrum/ContinuousBayesianNetworkFactory.hxx"

using namespace OT;

namespace OTAGRUM
{

CLASSNAMEINIT(ContinuousBayesianNetworkFactory)

static const Factory<ContinuousBayesianNetworkFactory>
Factory_ContinuousBayesianNetworkFactory;

/* Default constructor */
ContinuousBayesianNetworkFactory::ContinuousBayesianNetworkFactory()
  : DistributionFactoryImplementation()
  , marginalsFactory_(HistogramFactory())
  , copulasFactory_(BernsteinCopulaFactory())
  , namedDAG_()
  , alpha_(ResourceMap::GetAsScalar("ContinuousBayesianNetworkFactory-DefaultAlpha"))
  , maximumConditioningSetSize_(ResourceMap::GetAsUnsignedInteger("ContinuousBayesianNetworkFactory-DefaultMaximumConditioningSetSize"))
  , workInCopulaSpace_(ResourceMap::GetAsBool("ContinuousBayesianNetworkFactory-WorkInCopulaSpace"))
{
  setName("ContinuousBayesianNetworkFactory");
}

/* Parameter constructor */
ContinuousBayesianNetworkFactory::ContinuousBayesianNetworkFactory(const DistributionFactory & marginalsFactory,
    const DistributionFactory & copulasFactory,
    const NamedDAG & namedDAG,
    const Scalar alpha,
    const UnsignedInteger maximumConditioningSetSize,
    const Bool workInCopulaSpace)
  : DistributionFactoryImplementation()
  , marginalsFactory_(marginalsFactory)
  , copulasFactory_(copulasFactory)
  , namedDAG_(namedDAG)
  , alpha_(alpha)
  , maximumConditioningSetSize_(maximumConditioningSetSize)
  , workInCopulaSpace_(workInCopulaSpace)
{
  setName("ContinuousBayesianNetworkFactory");
}

/* Virtual constructor */
ContinuousBayesianNetworkFactory *
ContinuousBayesianNetworkFactory::clone() const
{
  return new ContinuousBayesianNetworkFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution
ContinuousBayesianNetworkFactory::build(const Sample &sample) const
{
  return buildAsContinuousBayesianNetwork(sample);
}

Distribution ContinuousBayesianNetworkFactory::build() const
{
  return buildAsContinuousBayesianNetwork().clone();
}

ContinuousBayesianNetwork
ContinuousBayesianNetworkFactory::buildAsContinuousBayesianNetwork(
  const Sample &sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE)
        << "Error: cannot build a ContinuousBayesianNetwork distribution "
        "from an empty "
        "sample";
  // Check if the named DAG has to be learnt
  NamedDAG localDAG;
  if (namedDAG_.getSize() == 0)
  {
    ContinuousPC learner(sample, maximumConditioningSetSize_, alpha_);
    localDAG = learner.learnDAG();
  }
  else localDAG = namedDAG_;
  // Now, learn the local distributions
  // First check if the user provided a BernsteinCopulaFactory and if we have
  // to use the BetaCopula
  const Bool useBetaCopula = (copulasFactory_.getImplementation()->getClassName() == BernsteinCopulaFactory::GetClassName()) && ResourceMap::GetAsBool("ContinuousBayesianNetworkFactory-UseBetaCopula");
  const Indices order(localDAG.getTopologicalOrder());
  Collection< Distribution > marginals(order.getSize());
  Collection< Distribution > copulas(order.getSize());
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalIndex = order[i];
    Indices indices(localDAG.getParents(globalIndex));
    LOGINFO(OSS() << "Learn node=" << globalIndex << ", with parents=" << indices);
    const UnsignedInteger dimension =  1 + indices.getSize();
    // Build the current marginal distribution
    if (workInCopulaSpace_) marginals[globalIndex] = Uniform(0.0, 1.0);
    else
    {
      const Sample marginalSample(sample.getMarginal(globalIndex));
      UserDefined discreteMarginal(UserDefinedFactory().buildAsUserDefined(marginalSample));
      // If the support is large enough use a continuous factory
      if (discreteMarginal.getSupport().getSize() > ResourceMap::GetAsUnsignedInteger("ContinuousBayesianNetworkFactory-MaximumDiscreteSupport"))
      {
        LOGINFO(OSS() << "  Learn marginal=" << globalIndex << " as a continuous distribution");
        marginals[globalIndex] = marginalsFactory_.build(marginalSample);
      }
      else
      {
        LOGINFO(OSS() << "  Learn marginal=" << globalIndex << " as a discrete distribution");
        marginals[globalIndex] = discreteMarginal;
      }
    }
    // Build the local copulas
    if (dimension == 1) copulas[globalIndex] = IndependentCopula(1);
    else
    {
      indices.add(globalIndex);
      const Sample localSample(sample.getMarginal(indices).rank() / size);
      if (useBetaCopula)
      {
        LOGINFO(OSS() << "  Learn copula=" << indices << " using a beta copula");
        copulas[globalIndex] = EmpiricalBernsteinCopula(localSample, localSample.getSize());
      }
      else
      {
        LOGINFO(OSS() << "  Learn copula=" << indices << " using the given factory");
        copulas[globalIndex] = copulasFactory_.build(localSample);
      }
    } // d > 1
  } // i (nodes)
  return ContinuousBayesianNetwork(localDAG, marginals, copulas);
}

ContinuousBayesianNetwork
ContinuousBayesianNetworkFactory::buildAsContinuousBayesianNetwork() const
{
  return ContinuousBayesianNetwork();
}

} // namespace OTAGRUM
