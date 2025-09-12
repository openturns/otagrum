//                                               -*- C++ -*-
/**
 *  @brief NamedJunctionTree
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

#ifndef OTAGRUM_JUNCTIONTREE_HXX
#define OTAGRUM_JUNCTIONTREE_HXX

#include <string>
#include <vector>

#include <agrum/base/graphs/cliqueGraph.h>
#include <agrum/BN/BayesNet.h>

#include <openturns/Description.hxx>
#include <openturns/Indices.hxx>

#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM
{
class OTAGRUM_API NamedJunctionTree : public OT::Object
{
public:
  typedef OT::Collection<OT::Indices> IndicesList;

  NamedJunctionTree();
  NamedJunctionTree(const gum::CliqueGraph &jt, const gum::BayesNet<double> &bn);
  NamedJunctionTree(const gum::CliqueGraph &jt, const std::vector<std::string> &names);

  OT::UnsignedInteger getSize() const;

  OT::Description getDescription() const;

  OT::Indices getClique(const OT::UnsignedInteger nod) const;
  OT::Indices getSeparator(const OT::UnsignedInteger nod1, const OT::UnsignedInteger nod2) const;

  OT::Indices getNeighbours(const OT::UnsignedInteger id) const;
  OT::Indices getNodes() const;

  IndicesList getCliquesCollection() const;
  IndicesList getSeparatorsCollection() const;

  NamedJunctionTree getMarginal(const OT::Indices & indices) const;

  OT::String __str__(const OT::String &offset = "") const override;

  OT::Indices getOrderMaxFirst() const;

private:
  gum::JunctionTree jt_;
  OT::Description map_;

  void checkConsistency() const;
};

} // namespace OTAGRUM
#endif // OTAGRUM_JUNCTIONTREE_H
