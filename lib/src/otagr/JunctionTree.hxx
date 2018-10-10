//
// Created by phw on 28/11/17.
//

#ifndef OTAGR_JUNCTIONTREE_H
#define OTAGR_JUNCTIONTREE_H
#include <string>
#include <vector>

#include <agrum/core/hashTable.h>
#include <agrum/graphs/cliqueGraph.h>

#include <openturns/Description.hxx>
#include <openturns/Indices.hxx>

namespace OTAGR {
class JunctionTree {
private:
  gum::JunctionTree jt_;
  OT::Description map_;

  OT::Indices fromNodeSet_(const gum::NodeSet &clique);

  void checkConsistency_();

public:
  JunctionTree(const gum::JunctionTree &jt,
               const std::vector<std::string> &names);

  ~JunctionTree();

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

  std::string __str__();
};
};
#endif // OTAGR_JUNCTIONTREE_H
