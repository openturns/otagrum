//                                               -*- C++ -*-
/**
 *  @brief Utils
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

#include <stdint.h>
#include "otagrum/Utils.hxx"

#include <openturns/Description.hxx>
#include <openturns/Histogram.hxx>
#include <openturns/Os.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/ResourceMap.hxx>
#include <openturns/UserDefined.hxx>
#include <openturns/MixedHistogramUserDefined.hxx>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/BIF/BIFWriter.h>

#include <agrum/base/core/exceptions.h>

#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/IDiscretizedVariable.h>
#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/base/variables/rangeVariable.h>

namespace OTAGRUM
{

gum::Tensor<double>
Utils::Discretize(const OT::DistributionImplementation &distribution,
                  const gum::DiscretizedVariable<double> &v)
{
  return Discretize(OT::Distribution(distribution), v);
}


/* Helper function to discretize a continuous distribution */
gum::Tensor<double>
Utils::Discretize(const OT::Distribution &distribution,
                  const gum::DiscretizedVariable<double> &v,
                  bool isTruncated)
{
  if (distribution.getDimension() != 1)
    throw OT::InvalidArgumentException(HERE)
        << "Error: cannot discretize a distribution with dimension > 1 (in "
        << distribution.__repr__() << ".";

  if (!isTruncated)
  {
    double lowerBound = distribution.getRange().getLowerBound()[0];
    double upperBound = distribution.getRange().getUpperBound()[0];

    if (lowerBound < v.tick(0) || upperBound > v.tick(v.domainSize()))
      throw OT::InvalidArgumentException(HERE)
          << "Error: out of bounds for variable " << v.toString()
          << "(should be between " << lowerBound << " and " << upperBound
          << ").";
  }

  std::vector<double> result(v.domainSize());
  double sum = 0.0;
  for (gum::Size i = 0; i < v.domainSize(); ++i)
  {
    result[i] =
      distribution.computeProbability(OT::Interval(v.tick(i), v.tick(i + 1)));
    sum += result[i];
  }

  if (!isTruncated)
  {
    if (sum < 1.0 - sqrt(sqrt(OT::ResourceMap::GetAsScalar(
                                "Distribution-DefaultCDFEpsilon"))))
      throw OT::InternalException(HERE)
          << "Error: the discretization is not adapted to the distribution. "
          "There is a mass leakage of "
          << 1.0 - sum;
  }

  // Normalize the discretization to take into account roundoff errors
  for (gum::Size i = 0; i < v.domainSize(); ++i)
    result[i] /= sum;

  gum::Tensor<double> p;
  p.add(v);
  p.fillWith(result);

  return p;
}


OT::Distribution Utils::FromTensor(const gum::Tensor<double> &pot)
{
  if (pot.nbrDim() < 1)
  {
    throw OT::InvalidArgumentException(HERE)
        << "Error: potential must have at least one dimension"
        << pot.toString();
  }
  if (pot.nbrDim() == 1)
  {
    return FromMarginal(pot);
  }
  OT::Description description(OT::UnsignedInteger(pot.nbrDim()));

  // list of variables
  OT::Collection<OT::Point> ticksCollection(0);
  OT::Indices kind(0);
  for (OT::UnsignedInteger dim = 0; dim < pot.nbrDim(); dim++)
  {
    const auto &var = pot.variable(dim);
    description[dim] = var.name();
    OT::Point p;
    if (var.varType() == gum::VarType::DISCRETIZED)
    {
      kind.add(OT::MixedHistogramUserDefined::CONTINUOUS);

      for (const auto tick :
           dynamic_cast<const gum::DiscretizedVariable<double> &>(var)
           .ticksAsDoubles())
      {
        p.add(tick);
      }
    }
    else
    {
      kind.add(OT::MixedHistogramUserDefined::DISCRETE);
      for (unsigned long v = 0; v < var.domainSize(); v++)
      {
        p.add(v);
      }
    }
    ticksCollection.add(p);
  }

  // values
  OT::Point probabilityTable(0);
  gum::Instantiation I(pot);
  for (I.setFirst(); !I.end(); I.inc())
  {
    probabilityTable.add(pot.get(I));
  }

  OT::MixedHistogramUserDefined distribution(ticksCollection, kind,
      probabilityTable);
  distribution.setDescription(description);
  return distribution;
}


OT::Distribution Utils::FromMarginal(const gum::Tensor<double> &pot)
{
  if (pot.nbrDim() != 1)
  {
    throw OT::InvalidArgumentException(HERE)
        << "Error: no marginal with dimension != 1 in " << pot.toString();
  }

  const auto &v = pot.variable(0);

  gum::Instantiation inst(pot);
  OT::UnsignedInteger collectionSize = v.domainSize();

  // the probas of the Tensor in a Point collection
  OT::Point probas(collectionSize);
  for (inst.setFirst(); !inst.end(); ++inst)
  {
    probas[inst.val(0)] = pot.get(inst);
  }

  OT::Distribution res;

  switch (v.varType())
  {
    case gum::VarType::DISCRETIZED:
    {
      // we use the ticks to create a Histogram distribution
      auto vv = static_cast<const gum::DiscretizedVariable<double> &>(v);

      OT::Point ws(collectionSize);
      for (inst.setFirst(); !inst.end(); ++inst)
      {
        auto i = inst.val(0);
        ws[i] = (vv.tick(i + 1) - vv.tick(i));
      }
      res = OT::Histogram(vv.tick(0), ws, probas);
      break;
    }
    case gum::VarType::RANGE:
    {
      // we use the range to create a meaningful UserDefined distribution
      auto vv = static_cast<const gum::RangeVariable &>(v);

      OT::Sample val(vv.domainSize(), 1);
      for (auto i = vv.minVal(); i <= vv.maxVal(); i++)
        val[i - vv.minVal()][0] = i;
      res = OT::UserDefined(val, probas);
      break;
    }
    case gum::VarType::LABELIZED:
    {
      // if at least one label can not be parser as double,
      // we can just use the index to create a UserDefined distribution
      OT::Sample val(v.domainSize(), 1);
      bool ok = true;
      OT::Scalar value;
      for (unsigned long i = 0; i < v.domainSize(); i++)
      {
        std::istringstream iss(v.label(i));
        if (iss >> value)
          val[i][0] = value;
        else
        {
          ok = false;
          break;
        }
      }

      if (!ok) // at least on label has not been correctly parsed as
        // Scalar. Then we just use index
        for (unsigned long i = 0; i < v.domainSize(); i++)
          val[i][0] = i;

      res = OT::UserDefined(val, probas);
      break;
    }
    default:
      throw OT::InternalException(HERE) << "This case should never happen";
  }

  const OT::Description description(1, v.name());
  res.setDescription(description);
  return res;
}


OT::Indices Utils::FromNodeSet(const gum::NodeSet &clique)
{
  auto indices = OT::Indices();

  for (auto nod : clique)
    indices.add(nod);

  return indices;
}


Utils::Utils() {}

} /* namespace OTAGRUM */
