//                                               -*- C++ -*-
/**
 *  @brief The JunctionTreeDistribution distribution
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

#include <cmath>
#include "otagr/JunctionTreeDistribution.hxx"

#include <openturns/OSS.hxx>
#include <openturns/PersistentObjectFactory.hxx>

namespace OTAGR
{

CLASSNAMEINIT(JunctionTreeDistribution);

static const OT::Factory<JunctionTreeDistribution> Factory_JunctionTreeDistribution;

/* Default constructor */
JunctionTreeDistribution::JunctionTreeDistribution()
  : OT::ContinuousDistribution()
  , cliquesIndices_(1, OT::Indices(1))
  , separatorsIndices_(0)
  , cliquesDistributions_(1, OT::Uniform())
  , separatorsDistributions_(0)
{
  setName( "JunctionTreeDistribution" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
  JunctionTreeDistribution::JunctionTreeDistribution(const IndicesCollection & cliquesIndices,
						     const IndicesCollection & separatorsIndices,
						     const DistributionCollection & cliquesDistribution
						     const DistributionCollection & separatorsDistributions)
  : OT::ContinuousDistribution()
  , cliquesIndices_(cliquesIndices)
  , separatorsIndices_(separatorsIndices)
  , cliquesDistributions_(cliquesDistributions)
  , separatorsDistributions_(separatorsDistributions)
{
  setName( "JunctionTreeDistribution" );
  OT::UnsignedInteger dimension = 0;
  // Check the cliques indices wrt the cliques distributions
  const OT::UnsignedInteger cliquesNumber = cliquesIndices_.getSize();
  if (cliquesDistributions_.getSize() != cliquesNumbers) throw OT::InvalidArgumentException(HERE) << "Error: expected a collection cliques distributions of size=" << cliquesNumber << ", got size=" << cliquesDistributions_.getSize();
  // Check the distributions wrt the junction tree structure
  const OT::UnsignedInteger cliquesNumber = cliquesIndices_.getSize();
  if (cliquesDistributions_.getSize() != cliquesNumbers) throw OT::InvalidArgumentException(HERE) << "Error: expected a collection cliques distributions of size=" << cliquesNumber << ", got size=" << cliquesDistributions_.getSize();
  const OT::UnsignedInteger separatorsNumber = separatorsIndices_.getSize();
  if (separatorsDistributions_.getSize() != separatorsNumbers) throw OT::InvalidArgumentException(HERE) << "Error: expected a collection separators distributions of size=" << separatorsNumber << ", got size=" << separatorsDistributions_.getSize();
  // Check the cliques distributions wrt the cliques indices
  for (OT::UnsignedInteger i = 0; i < cliquesNumber; ++i)
    {
    if (cliquesDistributions_[i].getDimension() != cliquesIndices_[i].getSize()) throw OT::InvalidArgumentException(HERE) << "Error: the clique distribution " << i << " has a dimension=" << cliquesDistributions_[i].getDimension() << ", expected dimension=" << cliquesIndices_[i].getSize();
    dimension = std::max(dimension, *std::max_element(cliquesIndices_[i].begin(), cliquesIndices_[i].end()));
    }
  // Check the separators distributions wrt the separators indices
  for (OT::UnsignedInteger i = 0; i < cliquesNumber; ++i)
    if (cliquesDistributions_[i].getDimension() != cliquesIndices_[i].getSize()) throw OT::InvalidArgumentException(HERE) << "Error: the clique distribution " << i << " has a dimension=" << cliquesDistributions_[i].getDimension() << ", expected dimension=" << cliquesIndices_[i].getSize();
  
  setDimension( dimension );
  computeRange();
}

/* Comparison operator */
OT::Bool JunctionTreeDistribution::operator ==(const JunctionTreeDistribution & other) const
{
  if (this == &other) return true;
  return (cliquesIndices_ == other.cliquesIndices_) && (separatorsIndices_ == other.separatorsIndices_) && (cliquesDistributions_ == other.cliquesDistributions_) && (separatorsDistributions_ == other.separatorsDistributions_);
}

OT::Bool JunctionTreeDistribution::equals(const ContinuousDistribution & other) const
{
  const JunctionTreeDistribution* p_other = dynamic_cast<const JunctionTreeDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
OT::String JunctionTreeDistribution::__repr__() const
{
  OT::OSS oss(true);
  oss << "class=" << JunctionTreeDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " cliquesIndices=" << cliquesIndices_
      << " separatorsIndices=" << separatorsIndices_
      << " cliquesDistributions=" << cliquesDistributions_;
  return oss;
}

OT::String JunctionTreeDistribution::__str__(const OT::String & offset) const
{
  OT::OSS oss(false);
  oss << offset << getClassName() << "(cliques=" << cliquesIndices_ << ", separators=" << separatorsIndices_ << ", cliques distributions=" << cliquesDistributions_ << ", separators distributions=" << separatorsDistributions_ << ")";
  return oss;
}

/* Virtual constructor */
JunctionTreeDistribution * JunctionTreeDistribution::clone() const
{
  return new JunctionTreeDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void JunctionTreeDistribution::computeRange()
{
  OT::Point lowerBound(getDimension(), -OT::SpecFunc::MaxScalar);
  OT::Point upperBound(getDimension(),  OT::SpecFunc::MaxScalar);
  const UnsignedInteger cliquesNumber = cliquesDistributions_.getSize();
  for (OT::UnsignedInteger i = 0; i < cliquesNumber; ++i)
    {
      const OT::Point currentLowerBound(cliquesDistributions_[i].getRange().getLowerBound());
      const OT::Point currentUpperBound(cliquesDistributions_[i].getRange().getUpperBound());
      const OT::UnsignedInteger currentDimension = currentLowerBound.getDimension();
      for (OT::UnsignedInteger j = 0; j < currentDimension; ++j)
	{
	  const OT::UnsignedInteger currentIndex = cliquesIndices_[i][j];
	  lowerBound[currentIndex] = std::max(lowerBound[currentIndex], currentLowerBound[j]);
	  upperBound[currentIndex] = std::min(upperBound[currentIndex], currentUpperBound[j]);
	} // j
    } // i
  const UnsignedInteger separatorsNumber = separatorsDistributions_.getSize();
  for (OT::UnsignedInteger i = 0; i < separatorsNumber; ++i)
    {
      const OT::Point currentLowerBound(separatorsDistributions_[i].getRange().getLowerBound());
      const OT::Point currentUpperBound(separatorsDistributions_[i].getRange().getUpperBound());
      const OT::UnsignedInteger currentDimension = currentLowerBound.getDimension();
      for (OT::UnsignedInteger j = 0; j < currentDimension; ++j)
	{
	  const OT::UnsignedInteger currentIndex = separatorsIndices_[i][j];
	  lowerBound[currentIndex] = std::max(lowerBound[currentIndex], currentLowerBound[j]);
	  upperBound[currentIndex] = std::min(upperBound[currentIndex], currentUpperBound[j]);
	} // j
    } // i
  setRange(mixture_.getRange());
}


/* Get one realization of the distribution */
OT::Point JunctionTreeDistribution::getRealization() const
{
  return mixture_.getRealization();
}

/* Get a sample of the distribution */
OT::Sample JunctionTreeDistribution::getSample(const OT::UnsignedInteger size) const
{
  return mixture_.getSample(size);
}

/* Get the PDF of the distribution */
OT::Scalar JunctionTreeDistribution::computePDF(const OT::Point & point) const
{
  return mixture_.computePDF(point);
}


/* Get the CDF of the distribution */
OT::Scalar JunctionTreeDistribution::computeCDF(const OT::Point & point) const
{
  return mixture_.computeCDF(point);
}

OT::Scalar JunctionTreeDistribution::computeComplementaryCDF(const OT::Point & point) const
{
  return mixture_.computeComplementaryCDF(point);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
OT::Complex JunctionTreeDistribution::computeCharacteristicFunction(const OT::Scalar x) const
{
  return mixture_.computeCharacteristicFunction(x);
}

/* Get the quantile of the distribution */
OT::Point JunctionTreeDistribution::computeQuantile(const OT::Scalar prob,
    const OT::Bool tail) const
{
  return mixture_.computeQuantile(prob, tail);
}

/* Get the i-th marginal distribution */
OT::Distribution JunctionTreeDistribution::getMarginal(const OT::UnsignedInteger i) const
{
  const OT::UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw OT::InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (dimension == 1) return clone();
  if (kind_[i] == 0)
  {
    const OT::UnsignedInteger size = ticksCollection_[i].getSize();
    OT::SampleImplementation support(size, 1);
    support.setData(ticksCollection_[i]);
    OT::UserDefined marginal(support, mixture_.getMarginal(i)->getProbabilities());
    marginal.setDescription(OT::Description(1, getDescription()[i]));
    return marginal.clone();
  }
  return mixture_.getMarginal(i);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
JunctionTreeDistribution::Implementation JunctionTreeDistribution::getMarginal(const OT::Indices & indices) const
{
  const OT::UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw OT::InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  if (dimension == 1) return clone();
  return mixture_.getMarginal(indices);
} // getMarginal(Indices)

/* Check if the distribution is continuous */
OT::Bool JunctionTreeDistribution::isContinuous() const
{
  const OT::UnsignedInteger size = kind_.getSize();
  for (OT::UnsignedInteger i = 0; i < size; ++i)
    if (kind_[i] == 0) return false;
  return true;
}

/* Check if the distribution is discrete */
OT::Bool JunctionTreeDistribution::isDiscrete() const
{
  const OT::UnsignedInteger size = kind_.getSize();
  for (OT::UnsignedInteger i = 0; i < size; ++i)
    if (kind_[i] == 1) return false;
  return true;
}

/* Check if the distribution is integral */
OT::Bool JunctionTreeDistribution::isIntegral() const
{
  const OT::Scalar epsilon = OT::ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon");
  const OT::UnsignedInteger size = kind_.getSize();
  for (OT::UnsignedInteger i = 0; i < size; ++i)
    {
      if (kind_[i] == 1) return false;
      const OT::UnsignedInteger supportSize = ticksCollection_[i].getSize();
      for (OT::UnsignedInteger j = 0; j < supportSize; ++j)
	{
	  const OT::Scalar x = ticksCollection_[i][j];
	  if (std::abs(x - floor(x + 0.5)) > epsilon) return false;
	}
    } // i
  return true;
}


/* Compute the mean of the distribution */
void JunctionTreeDistribution::computeMean() const
{
  mean_ = mixture_.getMean();
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
OT::Point JunctionTreeDistribution::getStandardDeviation() const
{
  const OT::UnsignedInteger dimension = getDimension();
  OT::Point standardDeviation(dimension);
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
    standardDeviation[i] = getMarginal(i)->getStandardDeviation()[0];
  return standardDeviation;
}

/* Get the skewness of the distribution */
OT::Point JunctionTreeDistribution::getSkewness() const
{
  const OT::UnsignedInteger dimension = getDimension();
  OT::Point skewness(dimension);
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
    skewness[i] = getMarginal(i)->getSkewness()[0];
  return skewness;
}

/* Get the kurtosis of the distribution */
OT::Point JunctionTreeDistribution::getKurtosis() const
{
  const OT::UnsignedInteger dimension = getDimension();
  OT::Point kurtosis(dimension);
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
    kurtosis[i] = getMarginal(i)->getKurtosis()[0];
  return kurtosis;
}

/* Compute the covariance of the distribution */
void JunctionTreeDistribution::computeCovariance() const
{
  covariance_ = mixture_.getCovariance();
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
OT::Point JunctionTreeDistribution::getStandardMoment(const OT::UnsignedInteger n) const
{
  const OT::UnsignedInteger dimension = getDimension();
  OT::Point standardMoment(dimension);
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
    standardMoment[i] = getMarginal(i)->getStandardMoment(n)[0];
  return standardMoment;
}

/* Get the standard representative in the parametric family, associated with the standard moments */
JunctionTreeDistribution::Implementation JunctionTreeDistribution::getStandardRepresentative() const
{
  return clone();
}

/* Ticks collection accessor */
void JunctionTreeDistribution::setTicksCollection(const OT::Collection<OT::Point> & ticksCollection)
{
  *this = JunctionTreeDistribution(ticksCollection, kind_, probabilityTable_);
  computeRange();
}

OT::Collection<OT::Point> JunctionTreeDistribution::getTicksCollection() const
{
  return ticksCollection_;
}

/* Kind accessor */
void JunctionTreeDistribution::setKind(const OT::Collection<OT::UnsignedInteger> & kind)
{
  *this = JunctionTreeDistribution(ticksCollection_, kind, probabilityTable_);
  computeRange();
}

OT::Collection<OT::UnsignedInteger> JunctionTreeDistribution::getKind() const
{
  return kind_;
}

/* Probability table accessor */
void JunctionTreeDistribution::setProbabilityTable(const OT::Point & probabilityTable)
{
  *this = JunctionTreeDistribution(ticksCollection_, kind_, probabilityTable);
  computeRange();
}

OT::Point JunctionTreeDistribution::getProbabilityTable() const
{
  return probabilityTable_;
}

/* Conversion as a Mixture */
OT::Mixture JunctionTreeDistribution::asMixture() const
{
  return mixture_;
}

void JunctionTreeDistribution::save(OT::Advocate & adv) const
{
  OT::ContinuousDistribution::save(adv);
  adv.saveAttribute( "ticksCollection_", ticksCollection_ );
  adv.saveAttribute( "kind_", kind_ );
  adv.saveAttribute( "probabilityTable_", probabilityTable_ );
  adv.saveAttribute( "mixture_", mixture_ );
}

/* Method load() reloads the object from the StorageManager */
void JunctionTreeDistribution::load(OT::Advocate & adv)
{
  OT::ContinuousDistribution::load(adv);
  adv.loadAttribute( "ticksCollection_", ticksCollection_ );
  adv.loadAttribute( "kind_", kind_ );
  adv.loadAttribute( "probabilityTable_", probabilityTable_ );
  adv.loadAttribute( "mixture_", mixture_ );
  computeRange();
}

/* Description accessor */
void JunctionTreeDistribution::setDescription(const OT::Description & description)
{
  OT::ContinuousDistribution::setDescription(description);
  mixture_.setDescription(getDescription());
}

} /* namespace OTAGR */
