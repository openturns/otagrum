//                                               -*- C++ -*-
/**
 *  @brief JunctionTree
 *
 *  Copyright 2010-2018 Airbus-LIP6-Phimeca
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

#include "otagrum/JunctionTree.hxx"
#include "otagrum/Utils.hxx"

namespace OTAGRUM {


JunctionTree::JunctionTree(const gum::JunctionTree &jt,
                           const std::vector<std::string> &names)
  : OT::Object()
  , jt_(jt)
{
  for (const auto &name : names) {
    map_.add(name);
  }
  checkConsistency();
}

void JunctionTree::checkConsistency() {
  // checks if names and jt are consistent (same set of nodeIds)
  gum::NodeSet s;
  for (const auto &nod : jt_.nodes())
    s += jt_.clique(nod);

  if (s.size() != map_.getSize())
    throw OT::InvalidArgumentException(HERE)
        << "Error: inconsistency between nodes (size is " << s.size()
        << ") and names (size is " << map_.getSize()
        << ") in OT::JunctionTree object";

  for (gum::NodeId nod = 0; nod < map_.getSize(); ++nod) {
    if (!s.exists(nod))
      throw OT::InvalidArgumentException(HERE)
          << "Error: please use range(0,max) as NodeSet (now : " << s.toString()
          << ").";
  }
}

OT::UnsignedInteger JunctionTree::getSize() { return map_.getSize(); }

OT::Description JunctionTree::getDescription() { return map_; }

OT::Indices JunctionTree::getClique(gum::NodeId nod) {
  return Utils::FromNodeSet(jt_.clique(nod));
}

OT::Indices JunctionTree::getSeparator(gum::Edge edge) {
  return Utils::FromNodeSet(jt_.separator(edge));
}

const gum::NodeSet &JunctionTree::getNeighbours(gum::NodeId id) {
  return jt_.neighbours(id);
}

gum::EdgeSet JunctionTree::getEdges() { return jt_.edges(); }

gum::NodeSet JunctionTree::getNodes() { return jt_.asNodeSet(); }

OT::Collection<OT::Indices> JunctionTree::getCliquesCollection() {
  OT::Collection<OT::Indices> res;
  for (const auto &cliq : jt_.nodes())
    res.add(getClique(cliq));
  return res;
}

OT::Collection<OT::Indices> JunctionTree::getSeparatorCollection() {
  OT::Collection<OT::Indices> res;
  for (const auto &edg : jt_.edges())
    res.add(getSeparator(edg));
  return res;
}

JunctionTree JunctionTree::getMarginal(OT::Indices indices) {
  // create the names and the mapping between indices and new nodeIds
  std::vector<std::string> m_names;
  std::vector<int> m_ids(getSize());
  std::fill(m_ids.begin(), m_ids.end(), -1);
  int j = 0;
  for (const auto i : indices) {
    m_names.push_back(map_[i]);
    m_ids[i] = j++;
  }

  // create the cliques that intersect indices, using the mapping to transpose
  // the NodeIds
  gum::JunctionTree m_jt;
  for (auto cli : jt_.nodes()) {
    gum::NodeSet transpose;
    for (auto nod : jt_.clique(cli)) {
      if (m_ids[nod] != -1)
        transpose.insert(gum::NodeId(m_ids[nod]));
    }
    if (transpose.size() != 0) {
      bool to_be_added = true;
      // is there any clique that already contains transpose ?
      for (auto it = m_jt.nodes().beginSafe(); it != m_jt.nodes().endSafe();
           ++it) {
        if (transpose.isSubsetOf(m_jt.clique(*it))) {
          to_be_added = false;
          break;
        }
      }
      if (to_be_added) {
        // is there any clique that is contained by transpose (and that should
        // be removed)
        for (auto it = m_jt.nodes().beginSafe(); it != m_jt.nodes().endSafe();
             ++it) {
          if (transpose.isSupersetOf(m_jt.clique(*it))) {
            m_jt.eraseNode(*it);
          }
        }
        m_jt.addNode(cli, transpose);
      }
    }
  }

  auto siznod = m_jt.nodes().size();
  if (siznod > 1) {
    // creates the edges from the definition of cliques instead
    // we sort the potential edges by the size of the separator k + the
    // existence
    // of the edge in the original junction tree (1)
    std::vector<gum::EdgeSet> sortedEdges((m_jt.size() + 1) * 2);
    for (auto it1 = m_jt.nodes().beginSafe(); it1 != m_jt.nodes().endSafe();
         ++it1)
      for (auto it2 = it1; it2 != m_jt.nodes().endSafe(); ++it2) {
        if (it1 == it2)
          continue;
        gum::Size bonus = (jt_.existsEdge(*it1, *it2)) ? 1 : 0;
        gum::Size siz = (m_jt.clique(*it1) * m_jt.clique(*it2)).size();
        if (siz > 0) {
          sortedEdges[siz * 2 + bonus].insert(gum::Edge(*it1, *it2));
        }
      }
    gum::Idx pos = sortedEdges.size() - 1;
    while (m_jt.edges().size() < siznod - 1) {
      while (sortedEdges[pos].empty()) {
        if (pos == 0)
          break;
        pos = pos - 1;
      }
      if (sortedEdges[pos].empty())
        break;
      auto edge = *(sortedEdges[pos].begin());
      try {
        auto v = m_jt.undirectedPath(edge.first(), edge.second());
      } catch (gum::NotFound) {
        m_jt.addEdge(edge.first(), edge.second());
      }
      sortedEdges[pos].erase(edge);
    }
  }
  return JunctionTree(m_jt, m_names);
}

OT::String JunctionTree::__str__(const OT::String &) const {
  std::stringstream ss;
  ss << "[";
  bool first = true;
  for (const auto &item : map_) {
    if (!first)
      ss << ",";
    first = false;
    ss << item;
  }
  ss << "]\n";

  for (auto cliq : jt_.nodes()) {
    ss << cliq << " : [";
    first = true;
    for (auto nod : jt_.clique(cliq)) {
      if (!first)
        ss << ",";
      first = false;
      ss << nod << "(" << map_[nod] << ")";
    }
    ss << "]\n";
  }
  for (auto edg : jt_.edges()) {
    ss << edg << " : [";
    first = true;
    for (auto nod : jt_.separator(edg)) {
      if (!first)
        ss << ",";
      first = false;
      ss << nod << "(" << map_[nod] << ")";
    }
    ss << "]\n";
  }
  return ss.str();
}
} // namespace
