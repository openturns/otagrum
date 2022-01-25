//                                               -*- C++ -*-
/**
 *  @brief NamedJunctionTree
 *
 *  Copyright 2010-2022 Airbus-LIP6-Phimeca
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

#include <algorithm>
#include <vector>

#include "otagrum/NamedJunctionTree.hxx"
#include "otagrum/Utils.hxx"

namespace OTAGRUM
{
NamedJunctionTree::NamedJunctionTree() {};

NamedJunctionTree::NamedJunctionTree(const gum::CliqueGraph &jt,
                                     const gum::BayesNet<double> &bn)
  : OT::Object(), jt_(jt), map_(bn.size())
{
  std::transform(bn.nodes().begin(), bn.nodes().end(), map_.begin(),
                 [&bn](const gum::NodeId nod) -> std::string
  {
    return bn.variable(nod).name();
  });
}

NamedJunctionTree::NamedJunctionTree(const gum::CliqueGraph &jt,
                                     const std::vector<std::string> &names)
  : OT::Object(), jt_(jt)
{
  for (const auto &name : names)
  {
    map_.add(name);
  }
  checkConsistency();
}

void NamedJunctionTree::checkConsistency() const
{
  // checks if names and jt are consistent (same set of nodeIds)
  gum::NodeSet s;
  for (const auto &nod : jt_.nodes())
    s += jt_.clique(nod);

  if (s.size() != map_.getSize())
    throw OT::InvalidArgumentException(HERE)
        << "Error: inconsistency between nodes (size is " << s.size()
        << ") and names (size is " << map_.getSize()
        << ") in OT::NamedJunctionTree object";

  for (gum::NodeId nod = 0; nod < map_.getSize(); ++nod)
  {
    if (!s.exists(nod))
      throw OT::InvalidArgumentException(HERE)
          << "Error: please use range(0,max) as NodeSet (now : " << s.toString()
          << ").";
  }
}

OT::UnsignedInteger NamedJunctionTree::getSize() const
{
  return map_.getSize();
}

OT::Description NamedJunctionTree::getDescription() const
{
  return map_;
}

OT::Indices NamedJunctionTree::getClique(OT::UnsignedInteger nod) const
{
  return Utils::FromNodeSet(jt_.clique(gum::NodeId(nod)));
}

OT::Indices NamedJunctionTree::getSeparator(OT::UnsignedInteger nod1,
    OT::UnsignedInteger nod2) const
{
  return Utils::FromNodeSet(
           jt_.separator(gum::Edge(gum::NodeId(nod1), gum::NodeId(nod2))));
}

OT::Indices NamedJunctionTree::getNeighbours(OT::UnsignedInteger id) const
{
  return Utils::FromNodeSet(jt_.neighbours(gum::NodeId(id)));
}

OT::Indices NamedJunctionTree::getNodes() const
{
  return Utils::FromNodeSet(jt_.asNodeSet());
}

NamedJunctionTree::IndicesList NamedJunctionTree::getCliquesCollection() const
{
  IndicesList res;
  for (const auto &cliq : jt_.nodes())
    res.add(getClique(cliq));
  return res;
}

NamedJunctionTree::IndicesList NamedJunctionTree::getSeparatorsCollection() const
{
  IndicesList res;
  for (const auto &edg : jt_.edges())
    res.add(getSeparator(edg.first(), edg.second()));
  return res;
}

NamedJunctionTree NamedJunctionTree::getMarginal(OT::Indices indices) const
{
  // create the names and the mapping between indices and new nodeIds
  std::vector<std::string> m_names;
  std::vector<int> m_ids(getSize());
  std::fill(m_ids.begin(), m_ids.end(), -1);
  int j = 0;
  for (const auto i : indices)
  {
    m_names.push_back(map_[i]);
    m_ids[i] = j++;
  }

  // create the cliques that intersect indices, using the mapping to transpose
  // the NodeIds
  gum::JunctionTree m_jt;
  for (auto cli : jt_.nodes())
  {
    gum::NodeSet transpose;
    for (auto nod : jt_.clique(cli))
    {
      if (m_ids[nod] != -1)
        transpose.insert(gum::NodeId(m_ids[nod]));
    }
    if (transpose.size() != 0)
    {
      bool to_be_added = true;
      // is there any clique that already contains transpose ?
      for (auto it = m_jt.nodes().beginSafe(); it != m_jt.nodes().endSafe();
           ++it)
      {
        if (transpose.isProperSubsetOf(m_jt.clique(*it)))
        {
          to_be_added = false;
          break;
        }
      }
      if (to_be_added)
      {
        // is there any clique that is contained by transpose (and that should
        // be removed)
        for (auto it = m_jt.nodes().beginSafe(); it != m_jt.nodes().endSafe();
             ++it)
        {
          if (transpose.isProperSupersetOf(m_jt.clique(*it)))
          {
            m_jt.eraseNode(*it);
          }
        }
        m_jt.addNode(cli, transpose);
      }
    }
  }

  auto siznod = m_jt.nodes().size();
  if (siznod > 1)
  {
    // creates the edges from the definition of cliques instead
    // we sort the potential edges by the size of the separator k + the
    // existence
    // of the edge in the original junction tree (1)
    std::vector<gum::EdgeSet> sortedEdges((m_jt.size() + 1) * 2);
    for (auto it1 = m_jt.nodes().beginSafe(); it1 != m_jt.nodes().endSafe();
         ++it1)
      for (auto it2 = it1; it2 != m_jt.nodes().endSafe(); ++it2)
      {
        if (it1 == it2)
          continue;
        gum::Size bonus = (jt_.existsEdge(*it1, *it2)) ? 1 : 0;
        gum::Size siz = (m_jt.clique(*it1) * m_jt.clique(*it2)).size();
        if (siz > 0)
        {
          sortedEdges[siz * 2 + bonus].insert(gum::Edge(*it1, *it2));
        }
      }
    gum::Idx pos = sortedEdges.size() - 1;
    while (m_jt.edges().size() < siznod - 1)
    {
      while (sortedEdges[pos].empty())
      {
        if (pos == 0)
          break;
        pos = pos - 1;
      }
      if (sortedEdges[pos].empty())
        break;
      auto edge = *(sortedEdges[pos].begin());
      try
      {
        auto v = m_jt.undirectedPath(edge.first(), edge.second());
      }
      catch (gum::NotFound)
      {
        m_jt.addEdge(edge.first(), edge.second());
      }
      sortedEdges[pos].erase(edge);
    }
  }
  return NamedJunctionTree(m_jt, m_names);
}

OT::String NamedJunctionTree::__str__(const OT::String &pref) const
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

  for (auto cliq : jt_.nodes())
  {
    ss << cliq << " : [";
    first = true;
    for (auto nod : jt_.clique(cliq))
    {
      if (!first)
        ss << ",";
      first = false;
      ss << nod << "(" << map_[nod] << ")";
    }
    ss << "]\n" << pref;
  }
  for (auto edg : jt_.edges())
  {
    ss << edg << " : [";
    first = true;
    for (auto nod : jt_.separator(edg))
    {
      if (!first)
        ss << ",";
      first = false;
      ss << nod << "(" << map_[nod] << ")";
    }
    ss << "]\n";
  }
  return ss.str();
}

OT::Indices NamedJunctionTree::getOrderMaxFirst() const
{
  OT::Indices res;
  gum::NodeId root;
  gum::NodeSet marked;
  int max;

  while (marked.size() < jt_.size())
  {
    // Looking for the largest possible root in the junction tree
    max = -1;
    for (auto nod : jt_.nodes())
    {
      if (!marked.exists(nod))
      {
        if (jt_.neighbours(nod).size() <= gum::Size(1))
        {
          if (max < 0 ||
              (jt_.clique(nod).size() > gum::Size(max))) // we found a new max
          {
            root = nod;
            max = jt_.clique(nod).size();
          }
        }
      }
    }

    // root is OK
    assert(max != -1);

    gum::NodeSet tbv; // nodes to be visited
    tbv.insert(root);
    while (tbv.size() > gum::Size(0))
    {
      gum::NodeId nod = *tbv.begin();
      tbv.erase(nod);
      marked.insert(nod);
      res.add(OT::UnsignedInteger(nod));
      for (auto nei : jt_.neighbours(nod))
      {
        if (!marked.exists(nei))
        {
          tbv.insert(nei);
        }
      }
    } // while (DFS)
  }   // while (maybe more than one connex component

  return res;
}
} // namespace OTAGRUM
