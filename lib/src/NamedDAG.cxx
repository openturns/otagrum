//                                               -*- C++ -*-
/**
 *  @brief NamedDAG
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

#include <openturns/PersistentCollection.hxx>
#include <openturns/PersistentObjectFactory.hxx>

#include "otagrum/NamedDAG.hxx"
#include "otagrum/Utils.hxx"

namespace OTAGRUM {

CLASSNAMEINIT(NamedDAG);

static const OT::Factory<NamedDAG> Factory_NamedDAG;

NamedDAG::NamedDAG(){};
NamedDAG::NamedDAG(const gum::BayesNet<double> &bn)
    : dag_(bn.dag()), map_(bn.size()) {
  std::transform(bn.nodes().begin(), bn.nodes().end(), map_.begin(),
                 [&bn](const gum::NodeId nod) -> std::string {
                   return bn.variable(nod).name();
                 });
}

NamedDAG::NamedDAG(const gum::DAG &dag, const std::vector<std::string> &names)
    : dag_(dag), map_(dag.size()) {
  std::copy(names.begin(), names.end(), map_.begin());
}

OT::PersistentObject *NamedDAG::clone() const { return new NamedDAG(*this); }

OT::UnsignedInteger NamedDAG::getSize() const { return map_.getSize(); }

OT::Description NamedDAG::getDescription() const { return map_; }

OT::Indices NamedDAG::getParents(const OT::UnsignedInteger nod) const {
  return Utils::FromNodeSet(dag_.parents(nod));
}

OT::Indices NamedDAG::getChildren(const OT::UnsignedInteger nod) const {
  return Utils::FromNodeSet(dag_.children(nod));
}

OT::Indices NamedDAG::getNodes() const {
  return Utils::FromNodeSet(dag_.nodes().asNodeSet());
}

OT::Indices NamedDAG::getTopologicalOrder() const {
  OT::Indices res;
  for (auto nod : dag_.topologicalOrder()) {
    res.add(OT::UnsignedInteger(nod));
  }
  return res;
}

OT::String NamedDAG::__str__(const OT::String &pref) const {
  std::stringstream ss;
  ss << pref << "[";
  bool first = true;
  for (const auto &item : map_) {
    if (!first)
      ss << ",";
    first = false;
    ss << item;
  }
  ss << "]\n" << pref;

  ss << "[";
  first = true;
  for (const auto &nod : getNodes()) {
    for (const auto &chi : getChildren(nod)) {
      if (!first)
        ss << ",";
      first = false;
      ss << map_[nod] << "->" << map_[chi];
    }
  }
  ss << "]\n";

  return ss.str();
}

OT::String NamedDAG::toDot() const {
  std::stringstream ss;
  ss << "digraph {\n";
  for (const auto &nod : getNodes()) {
    for (const auto &chi : getChildren(nod)) {
      ss << "    \"" << map_[nod] << "\"->\"" << map_[chi] << "\"\n";
    }
  }
  ss << "}\n";

  return ss.str();
}
/* Method save() stores the object through the StorageManager */
void NamedDAG::save(OT::Advocate &adv) const {
  std::cerr << "In NamedDAG::save" << std::endl;
  OT::PersistentObject::save(adv);
  std::cerr << "map_=" << map_ << std::endl;
  adv.saveAttribute("map_", map_);
  OT::Indices nodes(getNodes());
  std::cerr << "nodes_=" << nodes << std::endl;
  adv.saveAttribute("nodes_", nodes);
  OT::PersistentCollection<OT::Indices> parentsByNodes;
  for (const auto &node : getNodes())
    parentsByNodes.add(getParents(node));
  std::cerr << "parentsByNodes_=" << parentsByNodes << std::endl;
  adv.saveAttribute("parentsByNodes_", parentsByNodes);
}

/* Method load() reloads the object from the StorageManager */
void NamedDAG::load(OT::Advocate &adv) {
  std::cerr << "In NamedDAG::load" << std::endl;
  OT::PersistentObject::load(adv);
  adv.loadAttribute("map_", map_);
  std::cerr << "map_=" << map_ << std::endl;
  OT::Indices nodes;
  adv.loadAttribute("nodes_", nodes);
  std::cerr << "nodes_=" << nodes << std::endl;
  OT::PersistentCollection<OT::Indices> parentsByNodes;
  adv.loadAttribute("parentsByNodes_", parentsByNodes);
  std::cerr << "parentsByNodes_=" << parentsByNodes << std::endl;

  dag_.clear();
  std::cerr << "dag cleared" << std::endl;
  for (const auto &nod : nodes) {
    dag_.addNodeWithId(nod);
  }
  std::cerr << "nodes added" << std::endl;
  for (int i = 0; i < nodes.getSize(); ++i)
    {
      const int nod = nodes[i];
      const OT::Indices parents(parentsByNodes[i]);
      for (const auto &par : parents)
	{
	  std::cerr << "arc " << map_[par] << "->" << map_[nod] << std::endl;
	  dag_.addArc(par, nod);
	}
    }
  std::cerr << "arc added" << std::endl;
  std::cerr << "ok" << std::endl;
}

} // namespace OTAGRUM
