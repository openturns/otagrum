//                                               -*- C++ -*-
/**
 *  @brief The ContinuousBayesianNetwork distribution
 *
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

#include "otagrum/ContinuousBayesianNetwork.hxx"
#include <cmath>

#include <openturns/DistFunc.hxx>
#include <openturns/Exception.hxx>
#include <openturns/OSS.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/SpecFunc.hxx>
#include <openturns/Uniform.hxx>

using namespace OT;

namespace OTAGRUM
{

CLASSNAMEINIT(ContinuousBayesianNetwork)

static const Factory<ContinuousBayesianNetwork>
Factory_ContinuousBayesianNetwork;

/* Default constructor */
ContinuousBayesianNetwork::ContinuousBayesianNetwork()
  : ContinuousDistribution()
  , dag_()
  , jointDistributions_(0)
{
  setName("ContinuousBayesianNetwork");
  setDAGAndDistributionCollection(dag_, jointDistributions_);
}

/* Parameters constructor */
ContinuousBayesianNetwork::ContinuousBayesianNetwork(const NamedDAG &dag,
    const DistributionCollection &jointDistributions)
  : ContinuousDistribution()
  , dag_(dag)
  , jointDistributions_(0)
{
  setName("ContinuousBayesianNetwork");
  setDAGAndDistributionCollection(dag, jointDistributions);
}

/* Comparison operator */
Bool ContinuousBayesianNetwork::
operator==(const ContinuousBayesianNetwork &other) const
{
  if (this == &other)
    return true;
  return (dag_ == other.dag_) &&
         (jointDistributions_ == other.jointDistributions_);
}

Bool ContinuousBayesianNetwork::equals(
  const DistributionImplementation &other) const
{
  const ContinuousBayesianNetwork *p_other =
    dynamic_cast<const ContinuousBayesianNetwork *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String ContinuousBayesianNetwork::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ContinuousBayesianNetwork::GetClassName()
      << " name=" << getName() << " dimension=" << getDimension()
      << " dag=" << dag_ << " jointDistributions=" << jointDistributions_;
  return oss;
}

String ContinuousBayesianNetwork::__str__(const String &offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(dag=" << dag_
      << ", joint distributions=" << jointDistributions_ << ")";
  return oss;
}

/* Virtual constructor */
ContinuousBayesianNetwork *ContinuousBayesianNetwork::clone() const
{
  return new ContinuousBayesianNetwork(*this);
}

/* Compute the numerical range of the distribution given the parameters values
 */
void ContinuousBayesianNetwork::computeRange()
{
  const UnsignedInteger dimension = dag_.getSize();
  setDimension(dimension);
  Point lower(dimension);
  Point upper(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Interval rangeI(jointDistributions_[i].getRange());
    const UnsignedInteger dimensionI = rangeI.getDimension();
    // Check if the current node is a root node
    lower[i] = rangeI.getLowerBound()[dimensionI - 1];
    upper[i] = rangeI.getUpperBound()[dimensionI - 1];
  } // i
  setRange(Interval(lower, upper));
}

/* Get one realization of the distribution */
Point ContinuousBayesianNetwork::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  Point result(dimension);
  const Indices order(dag_.getTopologicalOrder());

  // The generation works this way:
  // + go through the nodes according to a topological order wrt the dag
  // + the ith node in this order has a global index order[i]
  // + its parents have global indices parents[i]
  // + for the ith node, sample the conditional distribution corresponding
  //   to the multivariate distribution linked to this node.
  //   The convention is that the (d-1) first components of this distribution
  //   is the distribution of the parents of the node IN THE CORRECT ORDER
  //   whild the d-th component is the current node.
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalI = order[i];
    const Distribution localDistribution(jointDistributions_[globalI]);
    const Indices parents(dag_.getParents(globalI));
    const UnsignedInteger conditioningDimension(parents.getSize());
    if (conditioningDimension == 0)
    {
      result[globalI] = localDistribution.getRealization()[0];
    }
    else
    {
      Point y(conditioningDimension);
      for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
        y[j] = result[parents[j]];
      result[globalI] = localDistribution.computeConditionalQuantile(
                          RandomGenerator::Generate(), y);
    }
  } // i
  return result;
}

/* Get the PDF of the distribution */
Scalar ContinuousBayesianNetwork::computePDF(const Point &point) const
{
  const Indices order(dag_.getTopologicalOrder());
  Scalar pdf = 1.0;
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalI = order[i];
    const Indices parents(dag_.getParents(globalI));
    const UnsignedInteger conditioningDimension(parents.getSize());
    const Scalar x = point[globalI];
    if (conditioningDimension == 0)
      pdf *= jointDistributions_[globalI].computePDF(x);
    else
    {
      Point y(conditioningDimension);
      for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
        y[j] = point[parents[j]];
      const Scalar conditionalPDF =
        jointDistributions_[globalI].computeConditionalPDF(x, y);
      pdf *= conditionalPDF;
    }
  } // i
  return pdf;
}

/* DAG and DistributionCollection accessor */
void ContinuousBayesianNetwork::setDAGAndDistributionCollection(
  const NamedDAG &dag, const DistributionCollection &jointDistributions)
{
  const Indices order(dag.getTopologicalOrder());
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalIndex = order[i];
    if (jointDistributions[globalIndex].getDimension() != dag.getParents(globalIndex).getSize() + 1)
      throw InvalidArgumentException(HERE)
          << "Error: expected a joint distribution of dimension="
          << dag.getParents(globalIndex).getSize() + 1 << " for node=" << globalIndex
          << " and its parents=" << dag.getParents(globalIndex)
          << ", got dimension=" << jointDistributions[globalIndex].getDimension();
  }
  dag_ = dag;
  jointDistributions_ = jointDistributions;
  computeRange();
  setDescription(dag.getDescription());
}

NamedDAG ContinuousBayesianNetwork::getNamedDAG() const
{
  return dag_;
}

ContinuousBayesianNetwork::DistributionCollection
ContinuousBayesianNetwork::getDistributionCollection() const
{
  return jointDistributions_;
}

/* Method save() stores the object through the StorageManager */
void ContinuousBayesianNetwork::save(Advocate &adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute("dag_", dag_);
  adv.saveAttribute("jointDistributions_", jointDistributions_);
}

/* Method load() reloads the object from the StorageManager */
void ContinuousBayesianNetwork::load(Advocate &adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute("dag_", dag_);
  adv.loadAttribute("jointDistributions_", jointDistributions_);
  computeRange();
}

} // namespace OTAGRUM
