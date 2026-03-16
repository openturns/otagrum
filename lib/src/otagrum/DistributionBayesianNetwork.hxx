//                                               -*- C++ -*-
/**
 *  @brief The DistributionBayesianNetwork distribution
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
#ifndef OTAGRUM_DISTRIBUTIONBAYESIANNETWORK_HXX
#define OTAGRUM_DISTRIBUTIONBAYESIANNETWORK_HXX

#include <openturns/ContinuousDistribution.hxx>
#include <openturns/Distribution.hxx>

#include "otagrum/NamedDAG.hxx"
#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM
{

/**
 * @class DistributionBayesianNetwork
 *
 * The DistributionBayesianNetwork distribution.
 */
class OTAGRUM_API DistributionBayesianNetwork
  : public OT::ContinuousDistribution
{
  CLASSNAME
public:

  typedef OT::Collection< OT::Distribution >           DistributionCollection;
  typedef OT::PersistentCollection< OT::Distribution > DistributionPersistentCollection;

  /** Default constructor */
  DistributionBayesianNetwork();

  /** Parameters constructor */
  DistributionBayesianNetwork(const NamedDAG & dag,
                              const DistributionCollection & joints);

public:
  /** Comparison operator */
  using OT::ContinuousDistribution::operator ==;
  OT::Bool operator ==(const DistributionBayesianNetwork & other) const;
protected:
  OT::Bool equals(const OT::DistributionImplementation & other) const override;
public:

  /** String converter */
  OT::String __repr__() const override;
  OT::String __str__(const OT::String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  DistributionBayesianNetwork * clone() const override;

  /** Get one realization of the distribution */
  OT::Point getRealization() const override;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using OT::ContinuousDistribution::computePDF;
  OT::Scalar computePDF(const OT::Point & point) const override;

  /** Get the log-PDF of the distribution */
  using OT::ContinuousDistribution::computeLogPDF;
  OT::Scalar computeLogPDF(const OT::Point & point) const override;

  /** DAG, marginals and copulas accessor */
  void setDAGAndJoints(const NamedDAG & dag,
                       const DistributionCollection & joints);

  gum::DAG getDAG() const;
  OT::Indices getParents(const OT::UnsignedInteger nodeId) const;
  /** One joint per node */
  DistributionCollection getJoints() const;
  OT::Distribution getJointAtNode(const OT::UnsignedInteger i) const;

  using OT::ContinuousDistribution::getMarginal;
  OT::Distribution getMarginal(const OT::UnsignedInteger i) const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:

private:
  /** Compute the range */
  void computeRange() override;

  /** The main parameter set of the distribution */

  /** Underlying junction tree */
  NamedDAG dag_;

  /** Collection of distributions, one per node */
  DistributionPersistentCollection joints_;

}; /* class DistributionBayesianNetwork */

} /* namespace OTAGRUM */

#endif /* OTAGRUM_DISTRIBUTIONBAYESIANNETWORK_HXX */
