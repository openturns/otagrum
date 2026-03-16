//                                               -*- C++ -*-
/**
 *  @brief The DistributionBayesianNetwork distribution
 *
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

#include "otagrum/DistributionBayesianNetwork.hxx"
#include <cmath>

#include <openturns/DistFunc.hxx>
#include <openturns/Exception.hxx>
#include <openturns/OSS.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace OTAGRUM
{

CLASSNAMEINIT(DistributionBayesianNetwork)

static const Factory<DistributionBayesianNetwork>
Factory_DistributionBayesianNetwork;

/* Default constructor */
DistributionBayesianNetwork::DistributionBayesianNetwork()
  : ContinuousDistribution()
  , dag_()
  , joints_(0)
{
  setName("DistributionBayesianNetwork");
  setDAGAndJoints(dag_, joints_);
}

/* Parameters constructor */
DistributionBayesianNetwork::DistributionBayesianNetwork(const NamedDAG &dag,
    const DistributionCollection & joints)
  : ContinuousDistribution()
  , dag_(dag)
  , joints_(0)
{
  setName("DistributionBayesianNetwork");
  setDAGAndJoints(dag, joints);
}

/* Comparison operator */
Bool DistributionBayesianNetwork::
operator==(const DistributionBayesianNetwork &other) const
{
  if (this == &other)
    return true;
  return (dag_ == other.dag_) &&
         (joints_ == other.joints_);
}

Bool DistributionBayesianNetwork::equals(
  const DistributionImplementation &other) const
{
  const DistributionBayesianNetwork *p_other =
    dynamic_cast<const DistributionBayesianNetwork *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String DistributionBayesianNetwork::__repr__() const
{
  OSS oss(true);
  oss << "class=" << DistributionBayesianNetwork::GetClassName()
      << " name=" << getName() << " dimension=" << getDimension()
      << " dag=" << dag_ << " joints=" << joints_;
  return oss;
}

String DistributionBayesianNetwork::__str__(const String &offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(dag=" << dag_
      << ", joints=" << joints_ << ")";
  return oss;
}

/* Virtual constructor */
DistributionBayesianNetwork *DistributionBayesianNetwork::clone() const
{
  return new DistributionBayesianNetwork(*this);
}

/* Compute the numerical range of the distribution given the parameters values
 */
void DistributionBayesianNetwork::computeRange()
{
  const UnsignedInteger dimension = dag_.getSize();
  setDimension(dimension);
  Point lower(dimension);
  Point upper(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Interval rangeI(joints_[i].getRange());
    const UnsignedInteger dimI = joints_[i].getDimension();
    // Check if the current node is a root node
    lower[i] = rangeI.getLowerBound()[dimI - 1];
    upper[i] = rangeI.getUpperBound()[dimI - 1];
  } // i
  setRange(Interval(lower, upper));
}

/* Get one realization of the distribution */
Point DistributionBayesianNetwork::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  Point result(dimension);
  const Indices order(dag_.getTopologicalOrder());

  // The generation works this way:
  // + go through the nodes according to a topological order wrt the dag
  // + the ith node in this order has a global index order[i]
  // + its parents have global indices parents[i]
  // + for the ith node, sample the conditional joint corresponding
  //   to the multivariate joint linked to this node.
  //   The convention is that the (d-1) first components of this distribution
  //   is the distribution of the parents of the node IN THE CORRECT ORDER
  //   whild the d-th component is the current node.
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalI = order[i];
    const Distribution joint(joints_[globalI]);
    const Indices parents(dag_.getParents(globalI));
    const UnsignedInteger conditioningDimension(parents.getSize());
    if (conditioningDimension == 0)
    {
      result[globalI] = joint.getRealization()[0];
    }
    else
    {
      Point y(conditioningDimension);
      for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
        y[j] = result[parents[j]];
      result[globalI] = joint.computeConditionalQuantile(
                          RandomGenerator::Generate(), y);
    }
  } // i
  return result;
}

/* Get the PDF of the distribution */
Scalar DistributionBayesianNetwork::computePDF(const Point &point) const
{
  const Indices order(dag_.getTopologicalOrder());
  Scalar pdf = 1.0;
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalI = order[i];
    const Distribution joint(joints_[globalI]);
    const Indices parents(dag_.getParents(globalI));
    const UnsignedInteger conditioningDimension(parents.getSize());
    const Scalar x = point[globalI];
    if (conditioningDimension == 0)
      {
        pdf *= joint.computePDF(x);
      }
    else
    {
      Point y(conditioningDimension);
      for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
        y[j] = point[parents[j]];
      const Scalar conditionalPDF =
        joint.computeConditionalPDF(x, y);
      pdf *= conditionalPDF;
      if (!(pdf > 0.0)) return 0.0;
    }
  } // i
  return pdf;
}

/* Get the log-PDF of the distribution */
Scalar DistributionBayesianNetwork::computeLogPDF(const Point &point) const
{
  const Indices order(dag_.getTopologicalOrder());
  Scalar logPDF = 0.0;
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalI = order[i];
    const Distribution joint(joints_[globalI]);
    const Indices parents(dag_.getParents(globalI));
    const UnsignedInteger conditioningDimension(parents.getSize());
    const Scalar x = point[globalI];
    if (conditioningDimension == 0)
    {
      logPDF += joint.computeLogPDF(x);
    }
    else
      {
      Point y(conditioningDimension);
      for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
        y[j] = point[parents[j]];
      const Scalar conditionalPDF =
        joint.computeConditionalPDF(x, y);
      if (!(conditionalPDF > 0.0))
        return -SpecFunc::MaxScalar;
      logPDF += std::log(conditionalPDF);
    }
  } // i
  return logPDF;
}

/* DAG and joints accessor */
void DistributionBayesianNetwork::setDAGAndJoints(const NamedDAG &dag,
    const DistributionCollection & joints)
{
  const Indices order(dag.getTopologicalOrder());
  const UnsignedInteger size = order.getSize();
  if (joints.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected a collection of joints of size=" << size << ", got size=" << joints.getSize();
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalIndex = order[i];
    if (joints[globalIndex].getDimension() != dag.getParents(globalIndex).getSize() + 1)
      throw InvalidArgumentException(HERE)
          << "Error: expected a joint of dimension="
          << dag.getParents(globalIndex).getSize() + 1 << " for node=" << globalIndex
          << " and its parents=" << dag.getParents(globalIndex)
          << ", got dimension=" << joints[globalIndex].getDimension();
  }
  dag_ = dag;
  joints_ = joints;
  computeRange();
  setDescription(dag.getDescription());
}

gum::DAG DistributionBayesianNetwork::getDAG() const
{
  return dag_.getDAG();
}

Indices DistributionBayesianNetwork::getParents(const UnsignedInteger nodeId) const
{
  return dag_.getParents(nodeId);
}

Distribution
DistributionBayesianNetwork::getMarginal(const UnsignedInteger i) const
{
  if (i >= joints_.getSize()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (joints_[i].getDimension() == 1)
    return joints_[i];
  throw NotYetImplementedException(HERE) << "In getMarginal";
}

DistributionBayesianNetwork::DistributionCollection
DistributionBayesianNetwork::getJoints() const
{
  return joints_;
}

Distribution
DistributionBayesianNetwork::getJointAtNode(const UnsignedInteger i) const
{
  if (i >= joints_.getSize()) throw InvalidArgumentException(HERE) << "The index of a joint distribution must be in the range [0, dim-1]";
  return joints_[i];
}

/* Method save() stores the object through the StorageManager */
void DistributionBayesianNetwork::save(Advocate &adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute("dag_", dag_);
  adv.saveAttribute("joints_", joints_);
}

/* Method load() reloads the object from the StorageManager */
void DistributionBayesianNetwork::load(Advocate &adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute("dag_", dag_);
  adv.loadAttribute("joints_", joints_);
  computeRange();
}

} // namespace OTAGRUM
