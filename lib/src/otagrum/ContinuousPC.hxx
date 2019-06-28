//                                               -*- C++ -*-
/**
 *  @brief ContinuousPC
 *
 *  Copyright 2010-2019 Airbus-LIP6-Phimeca
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

#ifndef OTAGRUM_CONTINUOUSPC_HXX
#define OTAGRUM_CONTINUOUSPC_HXX

#include <agrum/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/graphs/algorithms/triangulations/junctionTreeStrategies/defaultJunctionTreeStrategy.h>
#include <agrum/graphs/cliqueGraph.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/graphs/undiGraph.h>

#include <openturns/Sample.hxx>

#include "otagrum/ContinuousTTest.hxx"
#include "otagrum/NamedDAG.hxx"
#include "otagrum/NamedJunctionTree.hxx"

namespace OTAGRUM
{

class OTAGRUM_API ContinuousPC : public OT::Object
{
public:
  explicit ContinuousPC(const OT::Sample &data,
                        const OT::UnsignedInteger maxConditioningSetSize = 5,
                        const double alpha = 0.1);

  NamedJunctionTree learnJunctionTree();
  NamedDAG learnDAG();

  gum::UndiGraph inferSkeleton();
  gum::MixedGraph inferPDAG(const gum::UndiGraph &g) const;

  gum::UndiGraph deriveMoralGraph(const gum::MixedGraph &g) const;
  gum::JunctionTree deriveJunctionTree(const gum::UndiGraph &g) const;
  gum::DAG deriveDAG(const gum::MixedGraph &p) const;

  void setVerbosity(bool verbose);
  bool getVerbosity() const;

  double getPValue(gum::NodeId x, gum::NodeId y) const;
  double getTTest(gum::NodeId x, gum::NodeId y) const;
  const OT::Indices getSepset(gum::NodeId x, gum::NodeId y) const;

  const std::vector<gum::Edge> &getRemoved() const;

  std::string skeletonToDot(const gum::UndiGraph &skeleton);
  std::string PDAGtoDot(const gum::MixedGraph &pdag);

private:
  gum::EdgeProperty<OT::Indices> sepset_;
  gum::EdgeProperty<double> pvalues_;
  gum::EdgeProperty<double> ttests_;
  std::vector<gum::Edge> removed_;

  OT::UnsignedInteger maxCondSet_;
  bool verbose_;
  ContinuousTTest tester_;

  bool testCondSetWithSize(gum::UndiGraph &g, OT::UnsignedInteger n) ;

  std::tuple<bool, double, double, OT::Indices>
  getSeparator(const gum::UndiGraph &g, gum::NodeId y, gum::NodeId z,
               const OT::Indices &neighbours, OT::UnsignedInteger n) const;
  std::vector<std::string> namesFromData(void) const;
};

} // namespace OTAGRUM

#endif // OTAGRUM_CONTINUOUSPC_HXX
