//
// Created by phw on 01/02/18.
//

#ifndef OTAGR_CONTINUOUSPC_H
#define OTAGR_CONTINUOUSPC_H

#include "otagr/ContinuousTTest.hxx"
#include <agrum/graphs/mixedGraph.h>
#include <agrum/graphs/undiGraph.h>
#include <openturns/Sample.hxx>

namespace OTAGR {
class ContinuousPC {
public:
  explicit ContinuousPC(const OT::Sample &data,
                        OT::UnsignedInteger maxParents = 5, double alpha = 0.1);

  gum::UndiGraph getSkeleton();
  gum::MixedGraph getPDAG(const gum::UndiGraph &g);
  gum::UndiGraph getMoralGraph(const gum::MixedGraph &g) const;

  void setOptimalPolicy(bool policy) { optimalPolicy_ = policy; };
  bool getOptimalPolicy() const { return optimalPolicy_; };

  void setVerbosity(bool verbose) { verbose_ = verbose; };
  bool getVerbosity() const { return verbose_; };

  double getPValue(gum::NodeId x, gum::NodeId y) {
    gum::Edge e(x, y);
    if (pvalues_.exists(e)) {
      return pvalues_[e];
    } else {
      throw OT::InvalidArgumentException(HERE)
          << "Error: No p-value for edge (" << e.first() << "," << e.second()
          << ").";
    }
  }
  double getTTest(gum::NodeId x, gum::NodeId y) {
    gum::Edge e(x, y);
    if (ttests_.exists(e)) {
      return ttests_[e];
    } else {
      throw OT::InvalidArgumentException(HERE)
          << "Error: No ttest value for edge (" << e.first() << ","
          << e.second() << ").";
    }
  }

  const OT::Indices getSepset(gum::NodeId x, gum::NodeId y) {
    gum::Edge e(x, y);
    if (pvalues_.exists(e)) {
      return sepset_[e];
    } else {
      throw OT::InvalidArgumentException(HERE)
          << "Error: No Sepset for edge (" << e.first() << "," << e.second()
          << ").";
    }
  }

  const std::vector<gum::Edge> &getRemoved() { return removed_; };

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

  bool testCondSetWithSize_(gum::UndiGraph &g, OT::UnsignedInteger n);

  std::tuple<bool, double, double, OT::Indices>
  bestSeparator_(const gum::UndiGraph &g, gum::NodeId y, gum::NodeId z,
                 const OT::Indices &neighbours, OT::UnsignedInteger n);
};
} // namespace OTAGR
#endif // OTAGR_CONTINUOUSPC_H
