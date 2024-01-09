//                                               -*- C++ -*-
/**
 *  @brief The ContinuousBayesianNetwork distribution
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
  , marginals_(0)
  , copulas_(0)
{
  setName("ContinuousBayesianNetwork");
  setDAGAndMarginalsAndCopulas(dag_, marginals_, copulas_);
}

/* Parameters constructor */
ContinuousBayesianNetwork::ContinuousBayesianNetwork(const NamedDAG &dag,
    const DistributionCollection & marginals,
    const DistributionCollection & copulas)
  : ContinuousDistribution()
  , dag_(dag)
  , marginals_(0)
  , copulas_(0)
{
  setName("ContinuousBayesianNetwork");
  setDAGAndMarginalsAndCopulas(dag, marginals, copulas);
}

/* Comparison operator */
Bool ContinuousBayesianNetwork::
operator==(const ContinuousBayesianNetwork &other) const
{
  if (this == &other)
    return true;
  return (dag_ == other.dag_) &&
         (marginals_ == other.marginals_) &&
         (copulas_ == other.copulas_);
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
      << " dag=" << dag_ << " marginals=" << marginals_ << ", copulas=" << copulas_;
  return oss;
}

String ContinuousBayesianNetwork::__str__(const String &offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(dag=" << dag_
      << ", marginals=" << marginals_ << ", copulas=" << copulas_ << ")";
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
    const Interval rangeI(marginals_[i].getRange());
    // Check if the current node is a root node
    lower[i] = rangeI.getLowerBound()[0];
    upper[i] = rangeI.getUpperBound()[0];
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
  // + for the ith node, sample the conditional copula corresponding
  //   to the multivariate copula linked to this node.
  //   The convention is that the (d-1) first components of this distribution
  //   is the distribution of the parents of the node IN THE CORRECT ORDER
  //   whild the d-th component is the current node.
  // + then map the copula realization into the actual realization using marginal
  //   quantiles
  // The generation of a copula realization
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalI = order[i];
    const Indices parents(dag_.getParents(globalI));
    const UnsignedInteger conditioningDimension(parents.getSize());
    if (conditioningDimension == 0)
    {
      result[globalI] = RandomGenerator::Generate();
    }
    else
    {
      const Distribution copula(copulas_[globalI]);
      Point y(conditioningDimension);
      for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
        y[j] = result[parents[j]];
      result[globalI] = copula.computeConditionalQuantile(
                          RandomGenerator::Generate(), y);
    }
  } // i
  // The generation of the actual realization
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    // No need to follow the topological order here
    result[i] = marginals_[i].computeScalarQuantile(result[i]);
  } // i
  return result;
}

/* Get the PDF of the distribution */
Scalar ContinuousBayesianNetwork::computePDF(const Point &point) const
{
  const Indices order(dag_.getTopologicalOrder());
  Scalar pdf = 1.0;
  // First compute the marginal part
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    pdf *= marginals_[i].computePDF(point[i]);
    if (pdf <= 0.0) return 0.0;
  } // i
  // Second, compute the copula part
  // a) map the given point into the copula space
  Point u(order.getSize());
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    // To avoid points exactly at the upper boundary of the support
    u[i] = std::min(1.0 - SpecFunc::ScalarEpsilon, marginals_[i].computeCDF(point[i]));
  } // i
  // b) compute the copula PDF
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalI = order[i];
    const Indices parents(dag_.getParents(globalI));
    const UnsignedInteger conditioningDimension(parents.getSize());
    const Scalar x = u[globalI];
    if (conditioningDimension > 0)
    {
      Point y(conditioningDimension);
      for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
        y[j] = u[parents[j]];
      const Scalar conditionalPDF =
        copulas_[globalI].computeConditionalPDF(x, y);
      pdf *= conditionalPDF;
      if (!(pdf > 0.0)) return 0.0;
    }
  } // i
  return pdf;
}

/* Get the log-PDF of the distribution */
Scalar ContinuousBayesianNetwork::computeLogPDF(const Point &point) const
{
  const Indices order(dag_.getTopologicalOrder());
  Scalar logPDF = 0.0;
  // First compute the marginal part
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const Scalar marginalLogPDF = marginals_[i].computeLogPDF(point[i]);
    if (marginalLogPDF == -SpecFunc::MaxScalar)
        return -SpecFunc::MaxScalar;
    logPDF += marginalLogPDF;
  } // i
  // Second, compute the copula part
  // a) map the given point into the copula space
  Point u(order.getSize());
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    // To avoid points exactly at the upper boundary of the support
    u[i] = std::min(1.0 - SpecFunc::ScalarEpsilon, marginals_[i].computeCDF(point[i]));
  } // i
  // b) compute the copula PDF
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalI = order[i];
    const Indices parents(dag_.getParents(globalI));
    const UnsignedInteger conditioningDimension(parents.getSize());
    const Scalar x = u[globalI];
    if (conditioningDimension > 0)
    {
      Point y(conditioningDimension);
      for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
        y[j] = u[parents[j]];
      const Scalar conditionalPDF =
        copulas_[globalI].computeConditionalPDF(x, y);
      if (!(conditionalPDF > 0.0))
        return -SpecFunc::MaxScalar;
      logPDF += std::log(conditionalPDF);
    }
  } // i
  return logPDF;
}

/* DAG, marginals and copulas accessor */
void ContinuousBayesianNetwork::setDAGAndMarginalsAndCopulas(const NamedDAG &dag,
    const DistributionCollection &marginals,
    const DistributionCollection & copulas)
{
  const Indices order(dag.getTopologicalOrder());
  const UnsignedInteger size = order.getSize();
  if (marginals.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected a collection of marginals of size=" << size << ", got size=" << marginals.getSize();
  if (copulas.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected a collection of copulas of size=" << size << ", got size=" << copulas.getSize();
  for (UnsignedInteger i = 0; i < order.getSize(); ++i)
  {
    const UnsignedInteger globalIndex = order[i];
    if (copulas[globalIndex].getDimension() != dag.getParents(globalIndex).getSize() + 1)
      throw InvalidArgumentException(HERE)
          << "Error: expected a copula of dimension="
          << dag.getParents(globalIndex).getSize() + 1 << " for node=" << globalIndex
          << " and its parents=" << dag.getParents(globalIndex)
          << ", got dimension=" << copulas[globalIndex].getDimension();
  }
  dag_ = dag;
  marginals_ = marginals;
  copulas_ = copulas;
  computeRange();
  setDescription(dag.getDescription());
}

gum::DAG ContinuousBayesianNetwork::getDAG() const
{
  return dag_.getDAG();
}

Indices ContinuousBayesianNetwork::getParents(const UnsignedInteger nodeId) const
{
    return dag_.getParents(nodeId);
}

ContinuousBayesianNetwork::DistributionCollection
ContinuousBayesianNetwork::getMarginals() const
{
  return marginals_;
}

Distribution
ContinuousBayesianNetwork::getMarginal(const UnsignedInteger i) const
{
  if (i >= marginals_.getSize()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  return marginals_[i];
}

Distribution
ContinuousBayesianNetwork::getCopulaAtNode(const UnsignedInteger i) const
{
  if (i >= copulas_.getSize()) throw InvalidArgumentException(HERE) << "The index of a copula distribution must be in the range [0, dim-1]";
  return copulas_[i];
}

ContinuousBayesianNetwork::DistributionCollection
ContinuousBayesianNetwork::getCopulas() const
{
  return copulas_;
}

/* Method save() stores the object through the StorageManager */
void ContinuousBayesianNetwork::save(Advocate &adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute("dag_", dag_);
  adv.saveAttribute("marginals_", marginals_);
  adv.saveAttribute("copulas_", copulas_);
}

/* Method load() reloads the object from the StorageManager */
void ContinuousBayesianNetwork::load(Advocate &adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute("dag_", dag_);
  adv.loadAttribute("marginals_", marginals_);
  adv.loadAttribute("copulas_", copulas_);
  computeRange();
}

} // namespace OTAGRUM
