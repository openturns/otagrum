//                                               -*- C++ -*-
/**
 *  @brief The ContinuousBayesianNetwork distribution
 *
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

#include <cmath>
#include "otagrum/ContinuousBayesianNetwork.hxx"

#include <openturns/OSS.hxx>
#include <openturns/Exception.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/SpecFunc.hxx>
#include <openturns/DistFunc.hxx>
#include <openturns/Uniform.hxx>
#include <openturns/PersistentObjectFactory.hxx>


namespace OTAGRUM
{

CLASSNAMEINIT(ContinuousBayesianNetwork);

static const OT::Factory<ContinuousBayesianNetwork> Factory_ContinuousBayesianNetwork;

/* Default constructor */
ContinuousBayesianNetwork::ContinuousBayesianNetwork()
  : OT::ContinuousDistribution()
  , dag_()
  , jointDistributions_(0)
{
  setName( "ContinuousBayesianNetwork" );
  setDAGAndDistributionCollection(dag_, jointDistributions_);
}

/* Parameters constructor */
ContinuousBayesianNetwork::ContinuousBayesianNetwork(const NamedDAG & dag,
    const DistributionCollection & jointDistributions)
  : OT::ContinuousDistribution()
  , dag_(dag)
  , jointDistributions_(0)
{
  setName( "ContinuousBayesianNetwork" );
  setDAGAndDistributionCollection(dag, jointDistributions);
}

/* Comparison operator */
OT::Bool ContinuousBayesianNetwork::operator ==(const ContinuousBayesianNetwork & other) const
{
  if (this == &other) return true;
  return (dag_ == other.dag_) && (jointDistributions_ == other.jointDistributions_);
}

OT::Bool ContinuousBayesianNetwork::equals(const DistributionImplementation & other) const
{
  const ContinuousBayesianNetwork* p_other = dynamic_cast<const ContinuousBayesianNetwork*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
OT::String ContinuousBayesianNetwork::__repr__() const
{
  OT::OSS oss(true);
  oss << "class=" << ContinuousBayesianNetwork::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " dag=" << dag_
      << " jointDistributions=" << jointDistributions_;
  return oss;
}

OT::String ContinuousBayesianNetwork::__str__(const OT::String & offset) const
{
  OT::OSS oss(false);
  oss << offset << getClassName() << "(dag=" << dag_ << ", joint distributions=" << jointDistributions_ << ")";
  return oss;
}

/* Virtual constructor */
ContinuousBayesianNetwork * ContinuousBayesianNetwork::clone() const
{
  return new ContinuousBayesianNetwork(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void ContinuousBayesianNetwork::computeRange()
{
  const OT::UnsignedInteger dimension = dag_.getNodes().getSize();
  setDimension(dimension);
  OT::Point lower(dimension);
  OT::Point upper(dimension);
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
  {
    const OT::Interval rangeI(jointDistributions_[i].getRange());
    const OT::UnsignedInteger dimension = rangeI.getDimension();
    // Check if the current node is a root node
    lower[i] = rangeI.getLowerBound()[dimension - 1];
    upper[i] = rangeI.getUpperBound()[dimension - 1];
  } // i
  setRange(OT::Interval(lower, upper));
}

/* Get one realization of the distribution */
OT::Point ContinuousBayesianNetwork::getRealization() const
{
  const OT::UnsignedInteger dimension = getDimension();
  OT::Point result(dimension);
  const OT::Indices order(dag_.getTopologicalOrder());

  for (OT::UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const OT::UnsignedInteger globalI = order[i];
    const OT::Distribution localDistribution(jointDistributions_[globalI]);
    const OT::Indices parents(dag_.getParents(globalI));
    const OT::UnsignedInteger conditioningDimension(parents.getSize());
    if (conditioningDimension == 0)
      result[globalI] = localDistribution.getRealization()[0];
    else
      {
	OT::Point y(conditioningDimension);
	for (OT::UnsignedInteger j = 0; j < conditioningDimension; ++j)
	  y[j] = result[order[j]];
	result[globalI] = localDistribution.computeConditionalQuantile(OT::RandomGenerator::Generate(), y);
      }
  } // i
  return result;
}

/* Get the PDF of the distribution */
OT::Scalar ContinuousBayesianNetwork::computePDF(const OT::Point & point) const
{
  const OT::Indices order(dag_.getTopologicalOrder());
  OT::Scalar pdf = 1.0;
  for (OT::UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const OT::UnsignedInteger globalI = order[i];
    const OT::Indices parents(dag_.getParents(globalI));
    const OT::UnsignedInteger conditioningDimension(parents.getSize());
    const OT::Scalar x = point[globalI];
    if (conditioningDimension == 0) pdf *= jointDistributions_[globalI].computePDF(x);
    else
      {
	OT::Point y(conditioningDimension);
	for (OT::UnsignedInteger j = 0; j < conditioningDimension; ++j)
	  y[j] = point[order[j]];
	const OT::Scalar conditionalPDF = jointDistributions_[globalI].computeConditionalPDF(x, y);
	pdf *= conditionalPDF;
      }
  } // i
  return pdf;
}

/* DAG and DistributionCollection accessor */
void ContinuousBayesianNetwork::setDAGAndDistributionCollection(const NamedDAG & dag,
    const DistributionCollection & jointDistributions)
{
  const OT::Indices order(dag.getTopologicalOrder());
  for (OT::UnsignedInteger i = 0; i < order.getSize(); ++i)
    if (jointDistributions[i].getDimension() != dag.getParents(i).getSize() + 1) throw OT::InvalidArgumentException(HERE) << "Error: expected a joint distribution of dimension=" << dag.getParents(i).getSize() + 1 << " for node=" << order[i] << " and its parents=" << dag.getParents(i) << ", got dimension=" << jointDistributions[i].getDimension();
  dag_ = dag;
  jointDistributions_ = jointDistributions;
  computeRange();
  setDescription(dag.getDescription());
}

NamedDAG ContinuousBayesianNetwork::getNamedDAG() const
{
  return dag_;
}

ContinuousBayesianNetwork::DistributionCollection ContinuousBayesianNetwork::getDistributionCollection() const
{
  return jointDistributions_;
}

/* Method save() stores the object through the StorageManager */
void ContinuousBayesianNetwork::save(OT::Advocate & adv) const
{
  OT::ContinuousDistribution::save(adv);
  adv.saveAttribute( "dag_", dag_ );
  adv.saveAttribute( "jointDistributions_", jointDistributions_ );
}

/* Method load() reloads the object from the StorageManager */
void ContinuousBayesianNetwork::load(OT::Advocate & adv)
{
  OT::ContinuousDistribution::load(adv);
  adv.loadAttribute( "dag_", dag_ );
  adv.loadAttribute( "jointDistributions_", jointDistributions_ );
  computeRange();
}

} /* namespace OTAGR */
