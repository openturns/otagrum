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

#ifndef OTAGRUM_JUNCTIONTREEDISTRIBUTION_HXX
#define OTAGRUM_JUNCTIONTREEDISTRIBUTION_HXX

#include <openturns/ContinuousDistribution.hxx>
#include <openturns/Mixture.hxx>

#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM {

/**
 * @class JunctionTreeDistribution
 *
 * The JunctionTreeDistribution distribution.
 */
class OTAGRUM_API JunctionTreeDistribution
  : public OT::ContinuousDistribution
{
  CLASSNAME;
public:

  typedef OT::Pointer<OT::ContinuousDistribution>   Implementation;

  /** Default constructor */
  JunctionTreeDistribution();

  /** Parameters constructor */
  JunctionTreeDistribution(const OT::Collection<OT::Point> & ticksCollection,
			    const OT::Collection<OT::UnsignedInteger> & kind,
			    const OT::Point & probabilityTable);

  /** Comparison operator */
  using OT::ContinuousDistribution::operator ==;
  OT::Bool operator ==(const JunctionTreeDistribution & other) const;
protected:
  OT::Bool equals(const OT::ContinuousDistribution & other) const;
public:

  /** String converter */
  OT::String __repr__() const;
  OT::String __str__(const OT::String & offset = "") const;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual JunctionTreeDistribution * clone() const;

  /** Get one realization of the distribution */
  OT::Point getRealization() const;
  OT::Sample getSample(const OT::UnsignedInteger size) const;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using OT::ContinuousDistribution::computePDF;
  OT::Scalar computePDF(const OT::Point & point) const;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
  using OT::ContinuousDistribution::computeCDF;
  OT::Scalar computeCDF(const OT::Point & point) const;
  using OT::ContinuousDistribution::computeComplementaryCDF;
  OT::Scalar computeComplementaryCDF(const OT::Point & point) const;

  /** Get the quantile of the distribution */
  using OT::ContinuousDistribution::computeQuantile;
  OT::Point computeQuantile(const OT::Scalar prob,
                        const OT::Bool tail = false) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  OT::Complex computeCharacteristicFunction(const OT::Scalar x) const;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const OT::UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const OT::Indices & indices) const;

  /** Check if the distribution is continuous */
  OT::Bool isContinuous() const;

  /** Check if the distribution is discrete */
  OT::Bool isDiscrete() const;

  /** Check if the distribution is integral */
  OT::Bool isIntegral() const;

  /** Get the standard deviation of the distribution */
  OT::Point getStandardDeviation() const;

  /** Get the skewness of the distribution */
  OT::Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  OT::Point getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  OT::Point getStandardMoment(const OT::UnsignedInteger n) const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  OT::Distribution getStandardRepresentative() const;

  /* Interface specific to JunctionTreeDistribution */

  /** Ticks collection accessor */
  void setTicksCollection(const OT::Collection<OT::Point> & ticksCollection);
  OT::Collection<OT::Point> getTicksCollection() const;

  /** Kind accessor */
  void setKind(const OT::Collection<OT::UnsignedInteger> & kind);
  OT::Collection<OT::UnsignedInteger> getKind() const;

  /** Probability table accessor */
  void setProbabilityTable(const OT::Point & probabilityTable);
  OT::Point getProbabilityTable() const;

  /** Conversion as a Mixture */
  OT::Mixture asMixture() const;
  
  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

  /** Description accessor */
  void setDescription(const OT::Description & description);

protected:

private:

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** The main parameter set of the distribution */
  /** Indices of the cliques */
  OT::PersistentCollection<OT::Indices> cliquesIndices_;
  
  /** Indices of the separators */
  OT::PersistentCollection<OT::Indices> separatorsIndices_;

  /** Distributions of the cliques */
  OT::PersistentCollection<OT::Distribution> cliquesDistributions_;

  /** Distributions of the separators */
  OT::PersistentCollection<OT::Distribution> separatorsDistributions_;
  
}; /* class JunctionTreeDistribution */

} /* namespace OTAGRUM */

#endif /* OTAGRUM_JUNCTIONTREEDISTRIBUTION_HXX */
