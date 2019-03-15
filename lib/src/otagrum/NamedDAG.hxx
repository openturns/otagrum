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

#ifndef OTAGRUM_NAMED_DAG_HXX
#define OTAGRUM_NAMED_DAG_HXX
#include <string>
#include <vector>

#include <agrum/BN/BayesNet.h>
#include <agrum/graphs/DAG.h>

#include <openturns/Description.hxx>
#include <openturns/Indices.hxx>

#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM
{
class OTAGRUM_API NamedDAG : public OT::Object
{
public:
  NamedDAG();
  explicit NamedDAG(const gum::BayesNet<double> &bn);
  NamedDAG(const gum::DAG &dag,
           const std::vector<std::string> &names);

  OT::UnsignedInteger getSize() const;
  OT::Description getDescription() const;

  OT::Indices getParents(const OT::UnsignedInteger nod) const;
  OT::Indices getChildren(const OT::UnsignedInteger nod) const;
  OT::Indices getNodes() const;

  virtual OT::String __str__(const OT::String &offset = "") const;
  OT::String toDot() const;
private:
  gum::DAG dag_;
  OT::Description map_;
};

} // namespace OTAGRUM
#endif // OTAGRUM_NAMED_DAG_HXX
