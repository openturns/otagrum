//                                               -*- C++ -*-
/**
 *  @brief ContinuousPC
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

#include <algorithm>
#include <vector>

#include "otagrum/NamedDAG.hxx"
#include "otagrum/Utils.hxx"

namespace OTAGRUM
{
NamedDAG::NamedDAG(
  const gum::BayesNet<double> &bn)
  : dag_(bn.dag()), map_(bn.size())
{
  std::transform(bn.nodes().begin(), bn.nodes().end(), map_.begin(),
                 [&bn](const gum::NodeId nod) -> std::string
  {
    return bn.variable(nod).name();
  });
}

NamedDAG::NamedDAG(
  const gum::DAG &dag, const std::vector<std::string> &names)
  : dag_(dag), map_(dag.size())
{
  for (const auto &name : names)
  {
    map_.add(name);
  }
}

OT::UnsignedInteger NamedDAG::getSize() const
{
  return map_.getSize();
}

OT::Description NamedDAG::getDescription() const
{
  return map_;
}

OT::Indices NamedDAG::getParents(const OT::UnsignedInteger nod) const
{
  return Utils::FromNodeSet(dag_.parents(nod));
}

OT::Indices
NamedDAG::getChildren(const OT::UnsignedInteger nod) const
{
  return Utils::FromNodeSet(dag_.children(nod));
}

OT::Indices NamedDAG::getNodes() const
{
  return Utils::FromNodeSet(dag_.nodes().asNodeSet());
}
} // namespace OTAGRUM
