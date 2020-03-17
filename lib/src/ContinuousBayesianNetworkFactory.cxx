//                                               -*- C++ -*-
/**
 *  @brief ContinuousBayesianNetworkFactory
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <openturns/OTprivate.hxx>
#include <openturns/PersistentObjectFactory.hxx>

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
  , factories_(1, BernsteinCopulaFactory())
  , namedDAG_()
  , alpha_(ResourceMap::GetAsScalar("ContinuousBayesianNetworkFactory-DefaultAlpha"))
  , maximumConditioningSetSize_(ResourceMap::GetAsUnsignedInteger("ContinuousBayesianNetworkFactory-DefaultMaximumConditioningSetSize"))
  , workInCopulaSpace_(ResourceMap::GetAsBool("ContinuousBayesianNetworkFactory-WorkInCopulaSpace"))
{
  setName("ContinuousBayesianNetworkFactory");
}

/* Parameter constructor */
ContinuousBayesianNetworkFactory::ContinuousBayesianNetworkFactory(const Collection< DistributionFactory > & factories,
    const NamedDAG & namedDAG,
    const Scalar alpha,
    const UnsignedInteger maximumConditioningSetSize,
    const Bool workInCopulaSpace)
  : DistributionFactoryImplementation()
  , factories_(factories)
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
  const Indices order(localDAG.getTopologicalOrder());
  Collection< Distribution > localDistributions(order.getSize());
  const Scalar learningRatio = ResourceMap::GetAsScalar("ContinuousBayesianNetworkFactory-LearningRatio");
  if (!((learningRatio >= 0.0) && (learningRatio <= 1.0)))
    throw InvalidArgumentException(HERE) << "Error: expected a learning ratio in (0, 1), here learning ratio=" << learningRatio << ". Check \"ContinuousBayesianNetworkFactory-LearningRatio\" in ResourceMap.";
  UnsignedInteger learningSize = static_cast<UnsignedInteger>(size * learningRatio);
  if ((learningSize == 0) || (learningSize == size))
    throw InvalidArgumentException(HERE) << "Error: expected a learning size between 1 and size-1, here learning size=" << learningSize << ". Check \"ContinuousBayesianNetworkFactory-LearningRatio\" in ResourceMap.";
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalIndex = order[i];
    Indices indices(localDAG.getParents(globalIndex));
    LOGINFO(OSS() << "Learn node=" << globalIndex << ", with parents=" << indices);
    const UnsignedInteger dimension =  1 + indices.getSize();
    if (dimension == 1 && workInCopulaSpace_) localDistributions.add(Uniform(0.0, 1.0));
    else
    {
      indices.add(globalIndex);
      Sample localSample(sample.getMarginal(indices));
      // Now, check if we have to perform a model selection
      if (factories_.getSize() == 1)
        localDistributions[globalIndex] = factories_[0].build(localSample);
      else
      {
        // Select the best model using a cross-validation based on
        // log-likelihood
        const Sample validationSample(localSample.split(learningSize));
        const UnsignedInteger factoriesNumber = factories_.getSize();
        Scalar bestScore = -SpecFunc::MaxScalar;
        Distribution bestCandidate;
        for (UnsignedInteger j = 0; j < factoriesNumber; ++j)
        {
          Distribution candidate(factories_[j].build(localSample));
          // Enforce the candidate to be a copula
          if (workInCopulaSpace_ && !candidate.isCopula())
            candidate = candidate.getCopula();
          const Scalar score = candidate.computeLogPDF(validationSample).computeMean()[0];
          LOGINFO(OSS() << "Candidate " << j << "=" << candidate << ", score=" << score);
          if (score > bestScore)
          {
            bestScore = score;
            bestCandidate = candidate;
            LOGINFO(OSS() << "Best candidate so far=" << bestCandidate.getDimension() << ", best score so far=" << bestScore);
          }
        } // j (factories)
        LOGINFO(OSS() << "Best candidate=" << bestCandidate.getDimension() << ", best score=" << bestScore);
        localDistributions[globalIndex] = bestCandidate;
      } // factories_.getSize() > 1
    } // d > 1 or !workInCopulaSpace
  } // i (nodes)
  return ContinuousBayesianNetwork(localDAG, localDistributions);
}

ContinuousBayesianNetwork
ContinuousBayesianNetworkFactory::buildAsContinuousBayesianNetwork() const
{
  return ContinuousBayesianNetwork();
}

} // namespace OTAGRUM
