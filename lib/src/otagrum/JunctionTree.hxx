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

#ifndef OTAGRUM_JUNCTIONTREE_HXX
#define OTAGRUM_JUNCTIONTREE_HXX

#include <string>
#include <vector>

#include <agrum/core/hashTable.h>
#include <agrum/graphs/cliqueGraph.h>

#include <openturns/Description.hxx>
#include <openturns/Indices.hxx>

#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM {
class OTAGRUM_API JunctionTree : public OT::Object {
public:

  JunctionTree(const gum::CliqueGraph &jt,
               const std::vector<std::string> &names);

  OT::UnsignedInteger getSize();

  OT::Description getDescription();

  OT::Indices getClique(gum::NodeId nod);
  OT::Indices getSeparator(gum::Edge edge);

  const gum::NodeSet &getNeighbours(gum::NodeId id);
  gum::EdgeSet getEdges();
  gum::NodeSet getNodes();

  OT::Collection<OT::Indices> getCliquesCollection();
  OT::Collection<OT::Indices> getSeparatorCollection();

  JunctionTree getMarginal(OT::Indices indices);

  OT::String __str__(const OT::String & offset = "") const;

private:
  gum::JunctionTree jt_;
  OT::Description map_;

  void checkConsistency();

};

}
#endif // OTAGRUM_JUNCTIONTREE_H
