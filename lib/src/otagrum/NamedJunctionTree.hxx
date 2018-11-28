//                                               -*- C++ -*-
/**
 *  @brief NamedJunctionTree
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

#ifndef OTAGRUM_JUNCTIONTREE_HXX
#define OTAGRUM_JUNCTIONTREE_HXX

#include <string>
#include <vector>

#include <agrum/graphs/cliqueGraph.h>
#include <agrum/BN/BayesNet.h>

#include <openturns/Description.hxx>
#include <openturns/Indices.hxx>

#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM
{
class OTAGRUM_API NamedJunctionTree : public OT::Object
{
public:
  NamedJunctionTree(const gum::CliqueGraph &jt, const gum::BayesNet<double> &bn);
  NamedJunctionTree(const gum::CliqueGraph &jt,
                    const std::vector<std::string> &names);

  OT::UnsignedInteger getSize() const;

  OT::Description getDescription() const;

  OT::Indices getClique(gum::NodeId nod) const;
  OT::Indices getSeparator(gum::Edge edge) const;

  const gum::NodeSet &getNeighbours(gum::NodeId id) const;
  gum::EdgeSet getEdges() const;
  gum::NodeSet getNodes() const;

  OT::Collection<OT::Indices> getCliquesCollection() const;
  OT::Collection<OT::Indices> getSeparatorsCollection() const;

  NamedJunctionTree getMarginal(OT::Indices indices) const;

  OT::String __str__(const OT::String &offset = "") const;

  OT::Indices getOrderMaxFirst() const;

private:
  gum::JunctionTree jt_;
  OT::Description map_;

  void checkConsistency() const;
};

} // namespace OTAGRUM
#endif // OTAGRUM_JUNCTIONTREE_H
