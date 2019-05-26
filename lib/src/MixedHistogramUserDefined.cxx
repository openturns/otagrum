//                                               -*- C++ -*-
/**
 *  @brief The MixedHistogramUserDefined distribution
 *
 *  Copyright 2010-2019 Airbus-LIP6-Phimeca
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

#include <cmath>
#include "otagrum/MixedHistogramUserDefined.hxx"

#include <openturns/ComposedDistribution.hxx>
#include <openturns/Dirac.hxx>
#include <openturns/Histogram.hxx>
#include <openturns/Uniform.hxx>
#include <openturns/UserDefined.hxx>
#include <openturns/OSS.hxx>
#include <openturns/Tuples.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/ResourceMap.hxx>

namespace OTAGRUM
{

CLASSNAMEINIT(MixedHistogramUserDefined);

static const OT::Factory<MixedHistogramUserDefined> Factory_MixedHistogramUserDefined;

/* Default constructor */
MixedHistogramUserDefined::MixedHistogramUserDefined()
  : OT::DistributionImplementation()
  , ticksCollection_(1, OT::Point(1))
  , kind_(1, 1)
  , probabilityTable_(OT::Point(1, 1.0))
{
  *this = MixedHistogramUserDefined(ticksCollection_, kind_, probabilityTable_);
  setName( "MixedHistogramUserDefined" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
MixedHistogramUserDefined::MixedHistogramUserDefined(const PointCollection & ticksCollection,
    const OT::Indices & kind,
    const OT::Point & probabilityTable)
  : OT::DistributionImplementation()
  , ticksCollection_(ticksCollection)
  , kind_(kind)
  , probabilityTable_(probabilityTable)
{
  setName( "MixedHistogramUserDefined" );
  const OT::UnsignedInteger dimension = kind.getSize();
  // Check the ticks
  if (ticksCollection.getSize() != dimension) throw OT::InvalidArgumentException(HERE) << "Error: expected a collection of ticks of size=" << dimension << ", got size=" << ticksCollection.getSize();
  // Check the probability table
  // kind[i] == 0 -> the ith marginal is discrete
  // kind[i] == 1 -> the ith marginal is continuous
  OT::UnsignedInteger totalSize = 1;
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (kind[i] > 1)
      throw OT::InvalidArgumentException(HERE) << "Kind must be in [[0, 1]]";
    if (!(ticksCollection[i].getSize() >= 1))
      throw OT::InvalidArgumentException(HERE) << "Empty ticks";
    if (ticksCollection[i].getSize() == kind[i])
      throw OT::InvalidArgumentException(HERE) << "Need at least 2 ticks for continuous variable";
    totalSize *= ticksCollection[i].getSize() - kind[i];
  }
  if (probabilityTable.getSize() != totalSize) throw OT::InvalidArgumentException(HERE) << "Error: expected a probability table of size=" << totalSize << ", got size=" << probabilityTable.getSize();
  // Special case: dimension 1
  if (dimension == 1)
  {
    const OT::Point ticks(ticksCollection[0]);
    // Discrete
    if (kind[0] == 0)
    {
      const OT::UnsignedInteger size = ticks.getSize();
      OT::SampleImplementation support(size, 1);
      support.setData(ticks);
      mixture_ = OT::Mixture(OT::Mixture::DistributionCollection(1, OT::UserDefined(support, probabilityTable)));
    }
    // Continuous
    else
      mixture_ = OT::Mixture(OT::Mixture::DistributionCollection(1, OT::Histogram(ticks, probabilityTable)));
  } // dimension == 1
  else
  {
    OT::Indices discretization(dimension);
    for (OT::UnsignedInteger i = 0; i < dimension; ++i)
      // Here, kind[i] == 0 <-> kind[i] is false <-> i is discrete
      discretization[i] = ticksCollection[i].getSize() - kind[i];
    OT::Bool allDiscrete = true;
    for (OT::UnsignedInteger i = 0; i < dimension; ++i)
      allDiscrete = allDiscrete && (kind[i] == 0);
    OT::IndicesCollection allIndices(OT::Tuples(discretization).generate());
    // Multivariate discrete
    if (allDiscrete)
    {
      OT::Sample support(totalSize, dimension);
      for (OT::UnsignedInteger i = 0; i < totalSize; ++i)
      {
        for (OT::UnsignedInteger j = 0; j < dimension; ++j)
          support(i, j) = ticksCollection[j][allIndices(i, j)];
      }
      mixture_ = OT::Mixture(OT::Mixture::DistributionCollection(1, OT::UserDefined(support, probabilityTable)));
    } // allDiscrete
    else
    {
      OT::Mixture::DistributionCollection atoms(totalSize);
      for (OT::UnsignedInteger i = 0; i < totalSize; ++i)
      {
        OT::Mixture::DistributionCollection subAtoms(dimension);
        for (OT::UnsignedInteger j = 0; j < dimension; ++j)
        {
          const OT::UnsignedInteger k = allIndices(i, j);
          const OT::Point ticks = ticksCollection[j];
          // Discrete
          if (kind[j] == 0)
            subAtoms[j] = OT::Dirac(OT::Point(1, ticks[k]));
          // Continuous
          else
            subAtoms[j] = OT::Uniform(ticks[k], ticks[k + 1]);
        } // j
        atoms[i] = OT::ComposedDistribution(subAtoms);
      } // i
      mixture_ = OT::Mixture(atoms, probabilityTable);
    } // At least one continuous
  } // dimension > 1
  setDimension( dimension );
  mixture_.setDescription(getDescription());
  computeRange();
}

/* Comparison operator */
OT::Bool MixedHistogramUserDefined::operator ==(const MixedHistogramUserDefined & other) const
{
  if (this == &other) return true;
  return (ticksCollection_ == other.ticksCollection_) && (kind_ == other.kind_) && (probabilityTable_ == other.probabilityTable_);
}

OT::Bool MixedHistogramUserDefined::equals(const DistributionImplementation & other) const
{
  const MixedHistogramUserDefined* p_other = dynamic_cast<const MixedHistogramUserDefined*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
OT::String MixedHistogramUserDefined::__repr__() const
{
  OT::OSS oss(true);
  oss << "class=" << MixedHistogramUserDefined::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " ticksCollection=" << ticksCollection_
      << " kind=" << kind_
      << " probabilityTable=" << probabilityTable_;
  return oss;
}

OT::String MixedHistogramUserDefined::__str__(const OT::String & offset) const
{
  OT::OSS oss(false);
  oss << offset << getClassName() << "(mixture = " << mixture_ << ")";
  return oss;
}

/* Virtual constructor */
MixedHistogramUserDefined * MixedHistogramUserDefined::clone() const
{
  return new MixedHistogramUserDefined(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void MixedHistogramUserDefined::computeRange()
{
  setRange(mixture_.getRange());
}


/* Get one realization of the distribution */
OT::Point MixedHistogramUserDefined::getRealization() const
{
  return mixture_.getRealization();
}

/* Get a sample of the distribution */
OT::Sample MixedHistogramUserDefined::getSample(const OT::UnsignedInteger size) const
{
  return mixture_.getSample(size);
}

/* Get the PDF of the distribution */
OT::Scalar MixedHistogramUserDefined::computePDF(const OT::Point & point) const
{
  return mixture_.computePDF(point);
}


/* Get the CDF of the distribution */
OT::Scalar MixedHistogramUserDefined::computeCDF(const OT::Point & point) const
{
  return mixture_.computeCDF(point);
}

OT::Scalar MixedHistogramUserDefined::computeComplementaryCDF(const OT::Point & point) const
{
  return mixture_.computeComplementaryCDF(point);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
OT::Complex MixedHistogramUserDefined::computeCharacteristicFunction(const OT::Scalar x) const
{
  return mixture_.computeCharacteristicFunction(x);
}

/* Get the quantile of the distribution */
OT::Point MixedHistogramUserDefined::computeQuantile(const OT::Scalar prob,
    const OT::Bool tail) const
{
  return mixture_.computeQuantile(prob, tail);
}

/* Get the i-th marginal distribution */
OT::Distribution MixedHistogramUserDefined::getMarginal(const OT::UnsignedInteger i) const
{
  const OT::UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw OT::InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (dimension == 1) return clone();
  if (kind_[i] == 0)
  {
    const OT::UnsignedInteger size = ticksCollection_[i].getSize();
    OT::SampleImplementation support(size, 1);
    support.setData(ticksCollection_[i]);
    const OT::Distribution marginalMixture(mixture_.getMarginal(i));
    OT::UserDefined marginal(marginalMixture.getSupport(), marginalMixture.getProbabilities());
    marginal.setDescription(OT::Description(1, getDescription()[i]));
    return marginal.clone();
  }
  return mixture_.getMarginal(i);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
OT::Distribution MixedHistogramUserDefined::getMarginal(const OT::Indices & indices) const
{
  const OT::UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw OT::InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  if (dimension == 1) return clone();
  return mixture_.getMarginal(indices);
} // getMarginal(Indices)

/* Check if the distribution is continuous */
OT::Bool MixedHistogramUserDefined::isContinuous() const
{
  const OT::UnsignedInteger size = kind_.getSize();
  for (OT::UnsignedInteger i = 0; i < size; ++i)
    if (kind_[i] == 0) return false;
  return true;
}

/* Check if the distribution is discrete */
OT::Bool MixedHistogramUserDefined::isDiscrete() const
{
  const OT::UnsignedInteger size = kind_.getSize();
  for (OT::UnsignedInteger i = 0; i < size; ++i)
    if (kind_[i] == 1) return false;
  return true;
}

/* Check if the distribution is integral */
OT::Bool MixedHistogramUserDefined::isIntegral() const
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
void MixedHistogramUserDefined::computeMean() const
{
  mean_ = mixture_.getMean();
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
OT::Point MixedHistogramUserDefined::getStandardDeviation() const
{
  const OT::UnsignedInteger dimension = getDimension();
  OT::Point standardDeviation(dimension);
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
    standardDeviation[i] = getMarginal(i).getStandardDeviation()[0];
  return standardDeviation;
}

/* Get the skewness of the distribution */
OT::Point MixedHistogramUserDefined::getSkewness() const
{
  const OT::UnsignedInteger dimension = getDimension();
  OT::Point skewness(dimension);
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
    skewness[i] = getMarginal(i).getSkewness()[0];
  return skewness;
}

/* Get the kurtosis of the distribution */
OT::Point MixedHistogramUserDefined::getKurtosis() const
{
  const OT::UnsignedInteger dimension = getDimension();
  OT::Point kurtosis(dimension);
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
    kurtosis[i] = getMarginal(i).getKurtosis()[0];
  return kurtosis;
}

/* Compute the covariance of the distribution */
void MixedHistogramUserDefined::computeCovariance() const
{
  covariance_ = mixture_.getCovariance();
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
OT::Point MixedHistogramUserDefined::getStandardMoment(const OT::UnsignedInteger n) const
{
  const OT::UnsignedInteger dimension = getDimension();
  OT::Point standardMoment(dimension);
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
    standardMoment[i] = getMarginal(i).getStandardMoment(n)[0];
  return standardMoment;
}

/* Get the standard representative in the parametric family, associated with the standard moments */
OT::Distribution MixedHistogramUserDefined::getStandardRepresentative() const
{
  return clone();
}

/* Ticks collection accessor */
void MixedHistogramUserDefined::setTicksCollection(const OT::Collection<OT::Point> & ticksCollection)
{
  *this = MixedHistogramUserDefined(ticksCollection, kind_, probabilityTable_);
  computeRange();
}

OT::Collection<OT::Point> MixedHistogramUserDefined::getTicksCollection() const
{
  return ticksCollection_;
}

/* Kind accessor */
void MixedHistogramUserDefined::setKind(const OT::Indices & kind)
{
  *this = MixedHistogramUserDefined(ticksCollection_, kind, probabilityTable_);
  computeRange();
}

OT::Indices MixedHistogramUserDefined::getKind() const
{
  return kind_;
}

/* Probability table accessor */
void MixedHistogramUserDefined::setProbabilityTable(const OT::Point & probabilityTable)
{
  *this = MixedHistogramUserDefined(ticksCollection_, kind_, probabilityTable);
  computeRange();
}

OT::Point MixedHistogramUserDefined::getProbabilityTable() const
{
  return probabilityTable_;
}

/* Conversion as a Mixture */
OT::Mixture MixedHistogramUserDefined::asMixture() const
{
  return mixture_;
}

void MixedHistogramUserDefined::save(OT::Advocate & adv) const
{
  OT::DistributionImplementation::save(adv);
  adv.saveAttribute( "ticksCollection_", ticksCollection_ );
  adv.saveAttribute( "kind_", kind_ );
  adv.saveAttribute( "probabilityTable_", probabilityTable_ );
  adv.saveAttribute( "mixture_", mixture_ );
}

/* Method load() reloads the object from the StorageManager */
void MixedHistogramUserDefined::load(OT::Advocate & adv)
{
  OT::DistributionImplementation::load(adv);
  adv.loadAttribute( "ticksCollection_", ticksCollection_ );
  adv.loadAttribute( "kind_", kind_ );
  adv.loadAttribute( "probabilityTable_", probabilityTable_ );
  adv.loadAttribute( "mixture_", mixture_ );
  computeRange();
}

/* Description accessor */
void MixedHistogramUserDefined::setDescription(const OT::Description & description)
{
  OT::DistributionImplementation::setDescription(description);
  mixture_.setDescription(getDescription());
}

} /* namespace OTAGRUM */
