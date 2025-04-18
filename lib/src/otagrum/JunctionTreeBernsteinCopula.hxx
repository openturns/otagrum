//                                               -*- C++ -*-
/**
 *  @brief The JunctionTreeBernsteinCopula distribution
 *
 *  Copyright 2010-2025 Airbus-LIP6-Phimeca
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
#ifndef OTAGRUM_JUNCTIONTREEBERNSTEINCOPULA_HXX
#define OTAGRUM_JUNCTIONTREEBERNSTEINCOPULA_HXX

#include <openturns/Distribution.hxx>
#include <openturns/Sample.hxx>
#include <openturns/DistributionImplementation.hxx>

#include "NamedJunctionTree.hxx"
#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM
{

/**
 * @class JunctionTreeBernsteinCopula
 *
 * The JunctionTreeBernsteinCopula distribution.
 */
class OTAGRUM_API JunctionTreeBernsteinCopula
  : public OT::DistributionImplementation
{
  CLASSNAME
public:

  typedef OT::Collection< OT::Indices > IndicesCollection;
  typedef OT::PersistentCollection< OT::Indices > IndicesPersistentCollection;

  /** Default constructor */
  JunctionTreeBernsteinCopula();

  /** Parameters constructor */
  JunctionTreeBernsteinCopula(const NamedJunctionTree & junctionTree,
                              const OT::Sample & copulaSample,
                              const OT::UnsignedInteger binNumber,
                              const OT::Bool isCopulaSample = false);

  JunctionTreeBernsteinCopula(const NamedJunctionTree & junctionTree,
                              const OT::Indices & cliquesOrder,
                              const OT::Sample & copulaSample,
                              const OT::UnsignedInteger binNumber,
                              const OT::Bool isCopulaSample = false);

public:
  /** Comparison operator */
  using OT::DistributionImplementation::operator ==;
  OT::Bool operator ==(const JunctionTreeBernsteinCopula & other) const;
protected:
  OT::Bool equals(const OT::DistributionImplementation & other) const override;
public:

  /** String converter */
  OT::String __repr__() const override;
  OT::String __str__(const OT::String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  JunctionTreeBernsteinCopula * clone() const override;

  /** Get one realization of the distribution */
  OT::Point getRealization() const override;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using OT::DistributionImplementation::computePDF;
  OT::Scalar computePDF(const OT::Point & point) const override;

  /** Copula sample accessor */
  void setCopulaSample(const OT::Sample & copulaSample,
                       const OT::Bool isEmpiricalCopulaSample = false);
  OT::Sample getCopulaSample() const;

  /** Set the order according to which the cliques are traversed */
  void setCliquesOrder(const OT::Indices & cliquesOrder);

  /** Bin number accessor */
private:
  void setBinNumber(const OT::UnsignedInteger binNumber);

public:
  OT::UnsignedInteger getBinNumber() const;

  /** Get the i-th marginal distribution */
  OT::Distribution getMarginal(const OT::UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  OT::Distribution getMarginal(const OT::Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:

private:
  /** Compute the derivative attributes from the main attributes */
  void update();

  /** Compute the range */
  void computeRange() override;

  /** The main parameter set of the distribution */

  /** Underlying junction tree */
  NamedJunctionTree junctionTree_;

  /** Cliques as collections of Indices */
  IndicesPersistentCollection cliquesCollection_;

  /** Separators as collections of Indices */
  IndicesPersistentCollection separatorsCollection_;

  /** The underlying sample */
  OT::Sample copulaSample_;

  /** The common Bernstein class size */
  OT::UnsignedInteger binNumber_;

  /** Normalization factors */
  OT::SampleImplementation logBetaMarginalFactors_;
  OT::SampleImplementation logFactors_;

}; /* class JunctionTreeBernsteinCopula */

} /* namespace OTAGRUM */

#endif /* OTAGRUM_JUNCTIONTREEBERNSTEINCOPULA_HXX */
