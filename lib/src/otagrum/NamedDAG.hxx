//                                               -*- C++ -*-
/**
 *  @brief NamedDAG
 *
 *  Copyright 2010-2020 Airbus-LIP6-Phimeca
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

#ifndef OTAGRUM_NAMED_DAG_HXX
#define OTAGRUM_NAMED_DAG_HXX
#include <string>
#include <vector>

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/graphs/DAG.h>

#include <openturns/Description.hxx>
#include <openturns/Indices.hxx>

#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM
{
class OTAGRUM_API NamedDAG : public OT::PersistentObject
{
  CLASSNAME;

public:
  NamedDAG();
  explicit NamedDAG(const gum::BayesNet<double> &bn);
  NamedDAG(const gum::DAG &dag, const std::vector<std::string> &names);

  OT::PersistentObject *clone() const override;

  OT::UnsignedInteger getSize() const;
  OT::Description getDescription() const;

  OT::Indices getParents(const OT::UnsignedInteger nod) const;
  OT::Indices getChildren(const OT::UnsignedInteger nod) const;
  OT::Indices getTopologicalOrder() const;

  gum::DAG getDAG() const;

  OT::String __str__(const OT::String &offset = "") const override;
  OT::String toDot() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate &adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate &adv) override;

private:
  void build_OTrepr_();

  gum::DAG dag_;

  OT::Description map_;
  OT::Description nodes_; // no need
  OT::Collection<OT::Indices> parents_;
  OT::Collection<OT::Indices> children_;
  OT::Indices topo_order_;
};

} // namespace OTAGRUM
#endif // OTAGRUM_NAMED_DAG_HXX
