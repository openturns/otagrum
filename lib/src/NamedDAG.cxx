//                                               -*- C++ -*-
/**
 *  @brief NamedDAG
 *
 *  Copyright 2010-2020 Airbus-LIP6-Phimeca
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

#include <openturns/PersistentCollection.hxx>
#include <openturns/PersistentObjectFactory.hxx>

#include "otagrum/NamedDAG.hxx"
#include "otagrum/Utils.hxx"

namespace OTAGRUM
{

CLASSNAMEINIT(NamedDAG);

static const OT::Factory<NamedDAG> Factory_NamedDAG;

NamedDAG::NamedDAG() {};
NamedDAG::NamedDAG(const gum::BayesNet<double> &bn)
  : dag_(bn.dag()), map_(bn.size())
{
  build_OTrepr_();
  std::transform(bn.nodes().begin(), bn.nodes().end(), map_.begin(),
                 [&bn](const gum::NodeId nod) -> std::string
  {
    return bn.variable(nod).name();
  });
}

NamedDAG::NamedDAG(const gum::DAG &dag, const std::vector<std::string> &names)
  : dag_(dag), map_(dag.size())
{
  build_OTrepr_();
  std::copy(names.begin(), names.end(), map_.begin());
}

void NamedDAG::build_OTrepr_()
{
  if (dag_.nextNodeId() != dag_.size())
  {
    throw OT::InvalidArgumentException(HERE)
        << "nodeIds in the gum::dag are not contiguous.";
  }

  topo_order_.clear();
  parents_.clear();
  parents_.resize(dag_.size());
  children_.clear();
  children_.resize(dag_.size());
  for (const auto n : dag_.topologicalOrder())
  {
    topo_order_.add(n);
    parents_[n] = Utils::FromNodeSet(dag_.parents(n));
    children_[n] = Utils::FromNodeSet(dag_.children(n));
  }
}

OT::PersistentObject *NamedDAG::clone() const
{
  return new NamedDAG(*this);
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
  return parents_[nod];
}

OT::Indices NamedDAG::getChildren(const OT::UnsignedInteger nod) const
{
  return children_[nod];
}

OT::Indices NamedDAG::getTopologicalOrder() const
{
  return topo_order_;
}

OT::String NamedDAG::__str__(const OT::String &pref) const
{
  std::stringstream ss;
  ss << pref << "[";
  bool first = true;
  for (const auto &item : map_)
  {
    if (!first)
      ss << ",";
    first = false;
    ss << item;
  }
  ss << "]\n" << pref;

  ss << "[";
  first = true;
  for (const auto &nod : topo_order_)
  {
    for (const auto &chi : getChildren(nod))
    {
      if (!first)
        ss << ",";
      first = false;
      ss << map_[nod] << "->" << map_[chi];
    }
  }
  ss << "]\n";

  return ss.str();
}

OT::String NamedDAG::toDot() const
{
  std::stringstream ss;
  ss << "digraph {\n";
  for (const auto &nod : topo_order_)
  {
    for (const auto &chi : getChildren(nod))
    {
      ss << "    \"" << map_[nod] << "\"->\"" << map_[chi] << "\"\n";
    }
  }
  ss << "}\n";

  return ss.str();
}
/* Method save() stores the object through the StorageManager */
void NamedDAG::save(OT::Advocate &adv) const
{
  OT::PersistentObject::save(adv);
  adv.saveAttribute("map_", map_);
  OT::PersistentCollection<OT::Indices> parentsByNodes;
  parentsByNodes = parents_;
  adv.saveAttribute("parentsByNodes_", parentsByNodes);
}

/* Method load() reloads the object from the StorageManager */
void NamedDAG::load(OT::Advocate &adv)
{
  OT::PersistentObject::load(adv);
  adv.loadAttribute("map_", map_);
  OT::PersistentCollection<OT::Indices> parentsByNodes;
  adv.loadAttribute("parentsByNodes_", parentsByNodes);

  dag_.clear();
  OT::UnsignedInteger graphsize = map_.getSize();
  for (gum::NodeId nod = 0; nod < graphsize; ++nod)
  {
    dag_.addNodeWithId(nod);
  }
  for (gum::NodeId nod = 0; nod < graphsize; ++nod)
  {
    const OT::Indices parents(parentsByNodes[nod]);
    for (const auto &par : parents)
      dag_.addArc(par, nod);
  }
  build_OTrepr_();
}

} // namespace OTAGRUM
