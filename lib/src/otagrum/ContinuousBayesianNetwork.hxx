//                                               -*- C++ -*-
/**
 *  @brief The ContinuousBayesianNetwork distribution
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
#ifndef OTAGRUM_CONTINUOUSBAYESIANNETWORK_HXX
#define OTAGRUM_CONTINUOUSBAYESIANNETWORK_HXX

#include <openturns/ContinuousDistribution.hxx>
#include <openturns/Distribution.hxx>

#include "otagrum/NamedDAG.hxx"
#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM
{

/**
 * @class ContinuousBayesianNetwork
 *
 * The ContinuousBayesianNetwork distribution.
 */
class OTAGRUM_API ContinuousBayesianNetwork
  : public OT::ContinuousDistribution
{
  CLASSNAME;
public:

  typedef OT::Collection< OT::Distribution >           DistributionCollection;
  typedef OT::PersistentCollection< OT::Distribution > DistributionPersistentCollection;

  /** Default constructor */
  ContinuousBayesianNetwork();

  /** Parameters constructor */
  ContinuousBayesianNetwork(const NamedDAG & dag, const DistributionCollection & jointDistributions);

public:
  /** Comparison operator */
  using OT::ContinuousDistribution::operator ==;
  OT::Bool operator ==(const ContinuousBayesianNetwork & other) const;
protected:
  OT::Bool equals(const OT::DistributionImplementation & other) const;
public:

  /** String converter */
  OT::String __repr__() const;
  OT::String __str__(const OT::String & offset = "") const;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual ContinuousBayesianNetwork * clone() const;

  /** Get one realization of the distribution */
  OT::Point getRealization() const;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using OT::ContinuousDistribution::computePDF;
  OT::Scalar computePDF(const OT::Point & point) const;

  /** DAG and DistributionCollection accessor */
  void setDAGAndDistributionCollection(const NamedDAG & dag,
                                       const DistributionCollection & jointDistributions);

  NamedDAG getNamedDAG() const;
  DistributionCollection getDistributionCollection() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:

private:
  /** Compute the range */
  void computeRange();

  /** The main parameter set of the distribution */

  /** Underlying junction tree */
  NamedDAG dag_;

  /** Collection of joint distributions */
  DistributionPersistentCollection jointDistributions_;

}; /* class ContinuousBayesianNetwork */

} /* namespace OTAGRUM */

#endif /* OTAGRUM_CONTINUOUSBAYESIANNETWORK_HXX */
