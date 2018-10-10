//                                               -*- C++ -*-
/**
 *  @brief The MixedHistogramUserDefined distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTAGR_MIXEDHISTOGRAMUSERDEFINED_HXX
#define OTAGR_MIXEDHISTOGRAMUSERDEFINED_HXX

#include <openturns/DistributionImplementation.hxx>
#include <openturns/Mixture.hxx>

#include "otagr/otagrprivate.hxx"

namespace OTAGR {

/**
 * @class MixedHistogramUserDefined
 *
 * The MixedHistogramUserDefined distribution.
 */
class OTAGR_API MixedHistogramUserDefined
  : public OT::DistributionImplementation
{
  CLASSNAME;
public:

  typedef OT::Pointer<OT::DistributionImplementation>   Implementation;

  /** Default constructor */
  MixedHistogramUserDefined();

  /** Parameters constructor */
  MixedHistogramUserDefined(const OT::Collection<OT::Point> & ticksCollection,
			    const OT::Collection<OT::UnsignedInteger> & kind,
			    const OT::Point & probabilityTable);

  /** Comparison operator */
  using OT::DistributionImplementation::operator ==;
  OT::Bool operator ==(const MixedHistogramUserDefined & other) const;
protected:
  OT::Bool equals(const OT::DistributionImplementation & other) const;
public:

  /** String converter */
  OT::String __repr__() const;
  OT::String __str__(const OT::String & offset = "") const;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual MixedHistogramUserDefined * clone() const;

  /** Get one realization of the distribution */
  OT::Point getRealization() const;
  OT::Sample getSample(const OT::UnsignedInteger size) const;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using OT::DistributionImplementation::computePDF;
  OT::Scalar computePDF(const OT::Point & point) const;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
  using OT::DistributionImplementation::computeCDF;
  OT::Scalar computeCDF(const OT::Point & point) const;
  using OT::DistributionImplementation::computeComplementaryCDF;
  OT::Scalar computeComplementaryCDF(const OT::Point & point) const;

  /** Get the quantile of the distribution */
  using OT::DistributionImplementation::computeQuantile;
  OT::Point computeQuantile(const OT::Scalar prob,
                        const OT::Bool tail = false) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  OT::Complex computeCharacteristicFunction(const OT::Scalar x) const;

  /** Get the i-th marginal distribution */
  OT::Distribution getMarginal(const OT::UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  OT::Distribution getMarginal(const OT::Indices & indices) const;

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

  /* Interface specific to MixedHistogramUserDefined */

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
  /** Ticks per dimension */
  OT::PersistentCollection<OT::Point> ticksCollection_;

  /** Nature of each marginal (continuous/discrete) */
  OT::PersistentCollection<OT::UnsignedInteger> kind_;

  /** Probability table as a flat vector (no d-dim array available) */
  OT::Point probabilityTable_;

  /** Underlying mixture, waiting for a better implementation. */
  OT::Mixture mixture_;
}; /* class MixedHistogramUserDefined */

} /* namespace OTAGR */

#endif /* OTAGR_MIXEDHISTOGRAMUSERDEFINED_HXX */
