//                                               -*- C++ -*-
/**
 *  @brief ContinuousPC
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

#ifndef OTAGRUM_CONTINUOUSPC_HXX
#define OTAGRUM_CONTINUOUSPC_HXX

#include <agrum/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/graphs/algorithms/triangulations/junctionTreeStrategies/defaultJunctionTreeStrategy.h>
#include <agrum/graphs/cliqueGraph.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/graphs/undiGraph.h>

#include <openturns/Sample.hxx>

#include "otagrum/ContinuousTTest.hxx"
#include "otagrum/NamedJunctionTree.hxx"

namespace OTAGRUM
{

class OTAGRUM_API ContinuousPC : public OT::Object
{
public:
  explicit ContinuousPC(const OT::Sample &data,
                        const OT::UnsignedInteger maxParents = 5,
                        const double alpha = 0.1);

  gum::UndiGraph learnSkeleton();
  NamedJunctionTree learnJunctionTree();

  gum::MixedGraph learnPDAG(const gum::UndiGraph &g) const;
  gum::UndiGraph getMoralGraph(const gum::MixedGraph &g) const;
  NamedJunctionTree getJunctionTree(const gum::UndiGraph &g) const;

  void setOptimalPolicy(bool policy)
  {
    optimalPolicy_ = policy;
  };
  bool getOptimalPolicy() const
  {
    return optimalPolicy_;
  };

  void setVerbosity(bool verbose)
  {
    verbose_ = verbose;
  };
  bool getVerbosity() const
  {
    return verbose_;
  };

  double getPValue(gum::NodeId x, gum::NodeId y);
  double getTTest(gum::NodeId x, gum::NodeId y);
  const OT::Indices getSepset(gum::NodeId x, gum::NodeId y);

  const std::vector<gum::Edge> &getRemoved()
  {
    return removed_;
  };

  std::string skeletonToDot(const gum::UndiGraph &skeleton);
  std::string PDAGtoDot(const gum::MixedGraph &pdag);

private:
  gum::EdgeProperty<OT::Indices> sepset_;
  gum::EdgeProperty<double> pvalues_;
  gum::EdgeProperty<double> ttests_;
  std::vector<gum::Edge> removed_;

  OT::UnsignedInteger maxParents_;
  bool verbose_;
  bool optimalPolicy_;
  ContinuousTTest tester_;

  bool testCondSetWithSize(gum::UndiGraph &g, OT::UnsignedInteger n);

  std::tuple<bool, double, double, OT::Indices>
  bestSeparator(const gum::UndiGraph &g, gum::NodeId y, gum::NodeId z,
                const OT::Indices &neighbours, OT::UnsignedInteger n);
};

} // namespace OTAGRUM

#endif // OTAGRUM_CONTINUOUSPC_HXX
