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

#include <iomanip>
#include <sstream>
#include <tuple>

#include <agrum/core/hashFunc.h>
#include <agrum/core/priorityQueue.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/mixedGraph.h>

#include "otagrum/ContinuousPC.hxx"
#include "otagrum/Utils.hxx"

#define TRACE(x)                                                               \
  {                                                                            \
    if (verbose_)                                                              \
      std::cout << x;                                                          \
  }

// Triplet class, its HashFunc and its textual represenration
class Triplet {
public:
  gum::NodeId x, y, z;

  bool operator==(const Triplet &t) const {
    return (x == t.x) && (y == t.y) && (z == t.z);
  }

  std::string toString() const {
    std::stringstream sstr;
    sstr << "{" << x << "," << y << "," << z << "}";
    return sstr.str();
  }
};

namespace gum {
/// the hash function for Triplet
template <> class HashFunc<Triplet> : public HashFuncBase<Triplet> {
public:
  /// computes the hashed value of a key
  Size operator()(const Triplet &key) const {
    return (((key.x * HashFuncConst::gold + key.y) * HashFuncConst::gold +
             key.z) *
            HashFuncConst::gold) &
           this->_hash_mask;
  };
};
} // namespace gum

inline std::ostream &operator<<(std::ostream &aStream, const Triplet &i) {
  aStream << i.toString();
  return aStream;
}

namespace OTAGRUM {
/**
 * create an learner using PC algorithm with continuous variables
 *
 * @param data : the data
 * @param maxConditioningSetSize : the size max for the conditioning set in a
 * independence test
 * @param alpha : the threshold for the independence test
 */
ContinuousPC::ContinuousPC(const OT::Sample &data,
                           const OT::UnsignedInteger maxConditioningSetSize,
                           const double alpha)
    : OT::Object(), maxCondSet_(maxConditioningSetSize), verbose_(false),
      tester_(data), skel_done_(false), pdag_done_(false), dag_done_(false),
      jt_done_(false) {
  tester_.setAlpha(alpha);
  removed_.reserve(data.getDimension() * data.getDimension() /
                   3); // a rough estimation ...
}

/**
 * Search for the best separator between y and z in g, of size n, among
 * neighbours.
 *
 * @param g : the graph giving the neigbours
 * @param y : the node y
 * @param z : and the node z to separate
 * @param neighbours : the nodes X which separate
 * @param n : the size of separators
 * @return a std:tuple of (isIndep,ttestvalue,proba,sep).
 * Note that if proba=0, then no separator have been found
 *
 * @warning when optimalPolicy is set, the pair is the one with the best
 * p-value. Otherwise, it is the first one
 */
std::tuple<bool, double, double, OT::Indices>
ContinuousPC::getSeparator(const gum::UndiGraph & /*g*/, gum::NodeId y,
                           gum::NodeId z, const OT::Indices &neighbours,
                           OT::UnsignedInteger n) const {
  double t = 0.0;
  double p = 0.0;
  double pmax = -1.0;
  double tmax = 0.0;
  bool ok = false;

  const OT::Indices nullsep;

  IndicesCombinationIterator separator(neighbours, n);
  for (separator.setFirst(); !separator.isLast(); separator.next()) {
    std::tie(t, p, ok) = tester_.isIndep(y, z, separator.current());
    if (!ok) {
      TRACE(y << "-" << z << "|" << separator.current() << ":" << t << ", " << p
              << ", " << ok << "\n")
    } else {
      return std::make_tuple(true, t, p, separator.current());
    }
    if (p > pmax) {
      pmax = p;
      tmax = t;
    }
  }
  return std::make_tuple(false, tmax, pmax, nullsep);
}

//
/**
 * test all possible separator set of size n in graph g if a possible
 * separator set X is found for y,z then the edge is removed and the separator
 * is kept in sepset_
 *
 * @param g : the graph
 * @param n : the size of searched separators (beginning with n=0)
 * @return true if at least one separator has been found (if an edge has been
 * cut)
 */
bool ContinuousPC::testCondSetWithSize(gum::UndiGraph &g,
                                       OT::UnsignedInteger n) {
  if (g.sizeEdges() == 0)
    return false;

  bool atLeastOneInThisStep = false;
  gum::EdgeProperty<gum::NodeSet> intersections;
  for (const auto &edge : g.edges()) {
    const auto y = edge.first();
    const auto z = edge.second();
    const auto nei = g.neighbours(y) * g.neighbours(z);
    if (nei.size() >= n)
      intersections.set(edge, nei);
  }

  for (const auto &edge : g.edges()) {
    if (!intersections.exists(edge)) {
      continue;
    }
    const auto &nei = intersections[edge];
    const auto y = edge.first();
    const auto z = edge.second();
    bool resYZ = false;
    double pYZ = 0.0, tYZ = 0.0;
    OT::Indices sepYZ;

    std::tie(resYZ, tYZ, pYZ, sepYZ) =
        getSeparator(g, y, z, Utils::FromNodeSet(nei), n);

    if (resYZ) // we found at least one separator
    {
      sepset_.set(edge, sepYZ);
      pvalues_.set(edge, pYZ);
      ttests_.set(edge, tYZ);
      TRACE("==>" << edge << " cut. Sepset=" << sepset_[edge]
                  << ", pvalue=" << pvalues_[edge] << std::endl);
      atLeastOneInThisStep = true;
      removed_.push_back(edge);
      g.eraseEdge(edge);
    } else {
      pvalues_.set(edge, std::max(pvalues_.getWithDefault(edge, 0), pYZ));
      ttests_.set(edge, std::max(ttests_.getWithDefault(edge, -10000), tYZ));
    }
  }

  return atLeastOneInThisStep;
}

// From complete graph g, remove as much as possible edge (y,z) in g
// if (y,z) is removed, it means that sepset_[Edge(y,z)] contains X, set of
// nodes, such that y and z are tested as independent given X.
gum::UndiGraph ContinuousPC::inferSkeleton_() {
  gum::UndiGraph g;
  tester_.clearCache();
  sepset_.clear();
  pvalues_.clear();
  removed_.clear();

  TRACE("== PC algo starting " << std::endl);
  // create the complete graph
  for (gum::NodeId i = 0; i < tester_.getDimension(); ++i) {
    g.addNodeWithId(i);
    for (gum::NodeId j = 0; j < i; ++j) {
      g.addEdge(i, j);
    }
  }

  // for each size of sepset from 0 to n-1
  for (OT::UnsignedInteger n = 0; n < maxCondSet_; ++n) {
    TRACE("==  Size of conditioning set " << n << std::endl);
    // clear the pdfs not used anymore (due to the dimension of data)
    if (n > 0)
      tester_.clearCacheLevel(n - 1);

    testCondSetWithSize(g, n);
    // perform all the tests for size n
    // if (!testCondSetWithSize(g, n))
    // break;
  }
  TRACE("== end" << std::endl);

  return g;
}

std::vector<std::string> ContinuousPC::namesFromData(void) const {
  std::vector<std::string> names;
  const auto &description = tester_.getDataDescription();
  for (OT::UnsignedInteger i = 0; i < description.getSize(); i++) {
    names.push_back(description.at(i));
  }
  return names;
}

NamedJunctionTree ContinuousPC::learnJunctionTree() {
  if (jt_done_)
    return jt_;
  if (!pdag_done_)
    learnPDAG();

  jt_ = NamedJunctionTree(deriveJunctionTree_(deriveMoralGraph_(pdag_)),
                          namesFromData());
  jt_done_ = true;
  return jt_;
}

NamedDAG ContinuousPC::learnDAG() {
  if (dag_done_)
    return dag_;
  if (!pdag_done_)
    learnPDAG();

  dag_ = NamedDAG(deriveDAG_(pdag_), namesFromData());
  dag_done_ = true;
  return dag_;
}

gum::MixedGraph ContinuousPC::learnPDAG() {
  if (pdag_done_)
    return pdag_;
  if (!skel_done_)
    learnSkeleton();

  pdag_ = inferPDAG_(skel_);
  pdag_done_ = true;
  return pdag_;
}

gum::UndiGraph ContinuousPC::learnSkeleton() {
  if (skel_done_)
    return skel_;

  skel_ = inferSkeleton_();
  skel_done_ = true;
  return skel_;
}
// for all triplet x-y-z (no edge between x and z), if y is not in sepset[x,z]
// then x->y<-z.
// the ordering process uses the p-value as a priority.
gum::MixedGraph ContinuousPC::inferPDAG_(const gum::UndiGraph &g) const {
  gum::MixedGraph cpdag;

  gum::PriorityQueue<Triplet, double> queue;
  for (auto y : g.nodes()) {
    cpdag.addNodeWithId(y);

    if (g.neighbours(y).size() > 1) {
      IndicesCombinationIterator couple(Utils::FromNodeSet(g.neighbours(y)), 2);
      for (couple.setFirst(); !couple.isLast(); couple.next()) {
        const gum::NodeId x = couple.current()[0];
        const gum::NodeId z = couple.current()[1];
        if (!g.existsEdge(x, z)) // maybe unshielded collider
        {
          // bool ok = false;
          // double t = 0.0, p = 0.0;
          // OT::Indices indX;
          // indX = indX + OT::UnsignedInteger(x);
          // std::tie(t, p, ok) = tester_.isIndep(y, z, indX);
          const auto xz = gum::Edge(x, z);
          if (sepset_.exists(xz)) {
            if (!sepset_[xz].contains(y)) {
              queue.insert(Triplet{x, y, z}, pvalues_[xz]);
            }
          }
        }
      }
    }
  }

  for (auto e : g.edges()) {
    cpdag.addEdge(e.first(), e.second());
  }
  while (!queue.empty()) {
    Triplet t = queue.pop();
    if (!(cpdag.existsArc(t.y, t.x) || cpdag.existsArc(t.y, t.z))) {
      // we can add the v-structure
      cpdag.eraseEdge(gum::Edge(t.x, t.y));
      cpdag.eraseEdge(gum::Edge(t.z, t.y));
      cpdag.addArc(t.x, t.y);
      cpdag.addArc(t.z, t.y);
    }
  }

  return cpdag;
}

gum::UndiGraph ContinuousPC::deriveMoralGraph_(const gum::MixedGraph &g) const {
  gum::UndiGraph moral;
  for (auto x : g.nodes())
    moral.addNodeWithId(x);

  for (auto edge : g.edges())
    moral.addEdge(edge.first(), edge.second());

  for (auto x : g.nodes()) {
    for (auto par : g.parents(x))
      moral.addEdge(par, x);

    if (g.parents(x).size() > 1) {
      for (auto par1 : g.parents(x))
        for (auto par2 : g.parents(x))
          if (par1 != par2)
            if (!g.existsEdge(par1, par2))
              moral.addEdge(par1, par2);
    }
  }

  return moral;
}

gum::JunctionTree
ContinuousPC::deriveJunctionTree_(const gum::UndiGraph &g) const {
  gum::DefaultTriangulation triangulation;
  gum::NodeProperty<gum::Size> mods;

  const auto &description = tester_.getDataDescription();
  for (OT::UnsignedInteger i = 0; i < description.getSize(); i++) {
    // triangulation needs modalities. We just say that mods
    // are all the same
    mods.insert(i, 2);
  }
  triangulation.setGraph(&g, &mods);

  return triangulation.junctionTree();
}

static bool isAdjacent(const gum::MixedGraph &p, const gum::NodeId i,
                       const gum::NodeId j) {
  if (p.children(i).contains(j))
    return true;
  if (p.parents(i).contains(j))
    return true;
  if (p.neighbours(i).contains(j))
    return true;

  return false;
}

// Rule 1: Orient i-j into i->j whenever there is an arrow k->i
// such that k and j are nonadjacent.
static bool checkRule1(const gum::MixedGraph &p, gum::DAG &dag,
                       const gum::NodeId i, const gum::NodeId j) {
  if (std::any_of(dag.parents(i).begin(), dag.parents(i).end(),
                  [&](const gum::NodeId k) { return !isAdjacent(p, k, j); })) {
    try {
      dag.addArc(i, j);
    } catch (gum::InvalidDirectedCycle &e) {
      dag.addArc(j, i);
    }
    return true;
  }

  return false;
}

// Rule 2: Orient i-j into i->j whenever there is a chain
// i->k->j.
static bool checkRule2(gum::DAG &dag, const gum::NodeId i,
                       const gum::NodeId j) {
  if (std::any_of(
          dag.parents(j).begin(), dag.parents(j).end(),
          [&](const gum::NodeId k) { return dag.parents(k).contains(i); })) {
    try {
      dag.addArc(i, j);
    } catch (gum::InvalidDirectedCycle &e) {
      dag.addArc(j, i);
    }
    return true;
  }

  return false;
}

// Rule 3: Orient i-j into i->j whenever there are two chains
// i-k->j and i-l->j such that k and l are nonadjacent.
static bool checkRule3(const gum::MixedGraph &p, gum::DAG &dag,
                       const gum::NodeId i, const gum::NodeId j) {
  const auto candidates = dag.parents(j) * p.neighbours(i);
  if (candidates.size() <= 1)
    return false;

  for (const auto &k : candidates) {
    for (const auto &l : candidates) {
      if (k == l || isAdjacent(p, k, l))
        continue;
      try {
        dag.addArc(i, j);
      } catch (gum::InvalidDirectedCycle &e) {
        dag.addArc(j, i);
      }
      return true;
    }
  }

  return false;
}

gum::DAG ContinuousPC::deriveDAG_(const gum::MixedGraph &p) const {
  gum::EdgeSet remainings;
  gum::DAG dag;
  for (const auto nod : p.nodes()) {
    dag.addNodeWithId(nod);
  }
  for (const auto &arc : p.arcs()) {
    try {
      dag.addArc(arc.tail(), arc.head());
    } catch (gum::InvalidDirectedCycle &e) {
      GUM_SHOWERROR(e);
    }
  }
  for (const auto &edge : p.edges()) {
    remainings.insert(edge);
  }

  gum::EdgeSet toBeRemoved;
  while (remainings.size() > 0) {
    bool found = false;
    for (const auto &edge : remainings) {
      if (checkRule1(p, dag, edge.first(), edge.second()) ||
          checkRule1(p, dag, edge.second(), edge.first())) {
        toBeRemoved.insert(edge);
        found = true;
      }
    }
    remainings = remainings - toBeRemoved;
    toBeRemoved.clear();
    for (const auto &edge : remainings) {
      if (checkRule2(dag, edge.first(), edge.second()) ||
          checkRule2(dag, edge.second(), edge.first())) {
        toBeRemoved.insert(edge);
        found = true;
      }
    }
    remainings = remainings - toBeRemoved;
    toBeRemoved.clear();
    for (const auto &edge : remainings) {
      if (checkRule3(p, dag, edge.first(), edge.second()) ||
          checkRule3(p, dag, edge.second(), edge.first())) {
        toBeRemoved.insert(edge);
        found = true;
      }
    }
    remainings = remainings - toBeRemoved;
    toBeRemoved.clear();

    // Rule 4: Orient i-j into i->j whenever there are two chains
    // i-k->l and k->l->j such that k and j are nonadjacent.
    // (not used for dag from cpdag)

    if (!found) // No edge has been oriented in the last step : we try to find
    {
      // a good candidate among the remaining
      gum::Arc candidate(0, 0);
      found = false;
      auto minPar = dag.size();
      auto minBadPar = dag.size();
      for (const auto &edge : remainings) {
        auto npf = dag.parents(edge.first()).size();
        auto nps = dag.parents(edge.second()).size();
        if (npf == 0 &&
            nps == 0) // there cannot be better candidate, so we quit
        {
          candidate = gum::Arc(edge.first(), edge.second());
          found = true;
          break;
        }
        if (npf == 0) {
          if (minPar > nps) // it could be a good candidate
          {
            minPar = nps;
            candidate = gum::Arc(edge.second(), edge.first());
            found = true;
          }
        }
        if (nps == 0) {
          if (minPar > npf) // it could be a good candidate
          {
            minPar = npf;
            candidate = gum::Arc(edge.first(), edge.second());
            found = true;
          }
        }
        if (!found) {
          // while not found we keep the best bad candidate
          auto np = std::min(npf, nps);
          if (minBadPar > np) {
            minBadPar = np;
            if (np == npf) // then we add a parent to first
            {
              candidate = gum::Arc(edge.second(), edge.first());
            } else // we add a parent to second
            {
              candidate = gum::Arc(edge.first(), edge.second());
            }
          }
        }
      } // for
      try {
        dag.addArc(candidate.tail(), candidate.head());
      } catch (gum::InvalidDirectedCycle &e) {
      }
      remainings.erase(gum::Edge(candidate.tail(), candidate.head()));
      //@todo : A warning if !found (if we add a bad condidate)
    } // if (! found)
  }

  return dag;
}

std::string ContinuousPC::skeletonToDot(const gum::UndiGraph &skeleton) {
  std::stringstream ss;
  ss << "digraph \"skeleton\" {" << std::endl
     << "  edge [dir = none];" << std::endl
     << "  node [shape = ellipse];" << std::endl;
  ss << "  ";
  for (const auto node : skeleton.nodes()) {
    ss << node << "; ";
  }
  ss << std::endl;
  for (const auto edge : skeleton.edges()) {
    ss << "  " << edge.first() << "->" << edge.second()
       << " [label=\"t=" << std::setprecision(3)
       << getTTest(edge.first(), edge.second())
       << "\np=" << std::setprecision(3)
       << getPValue(edge.first(), edge.second()) << "\"]" << std::endl;
  }
  ss << "}";
  return ss.str();
}

std::string ContinuousPC::PDAGtoDot(const gum::MixedGraph &pdag) {
  std::stringstream ss;
  ss << "digraph \"PDAG\" {" << std::endl
     << "  edge [];" << std::endl
     << "  node [shape = ellipse];" << std::endl;
  ss << "  ";
  for (const auto node : pdag.nodes()) {
    ss << node << "; ";
  }
  ss << std::endl;
  for (const auto edge : pdag.edges()) {
    ss << "  " << edge.first() << "->" << edge.second()
       << " [dir=none,label=\"t=" << std::setprecision(3)
       << getTTest(edge.first(), edge.second())
       << "\np=" << std::setprecision(3)
       << getPValue(edge.first(), edge.second()) << "\"]" << std::endl;
  }
  ss << std::endl;
  for (const auto arc : pdag.arcs()) {
    ss << "  " << arc.first() << "->" << arc.second()
       << " [dir=none,label=\"t=" << std::setprecision(3)
       << getTTest(arc.first(), arc.second()) << "\np=" << std::setprecision(3)
       << getPValue(arc.first(), arc.second()) << "\"]" << std::endl;
  }
  ss << "}";
  return ss.str();
}

double ContinuousPC::getPValue(gum::NodeId x, gum::NodeId y) const {
  gum::Edge e(x, y);
  if (pvalues_.exists(e)) {
    return pvalues_[e];
  } else {
    throw OT::InvalidArgumentException(HERE)
        << "Error: No p-value for edge (" << e.first() << "," << e.second()
        << ").";
  }
}

double ContinuousPC::getTTest(gum::NodeId x, gum::NodeId y) const {
  gum::Edge e(x, y);
  if (ttests_.exists(e)) {
    return ttests_[e];
  } else {
    throw OT::InvalidArgumentException(HERE)
        << "Error: No ttest value for edge (" << e.first() << "," << e.second()
        << ").";
  }
}

OT::Indices ContinuousPC::getSepset(gum::NodeId x, gum::NodeId y) const {
  gum::Edge e(x, y);
  if (pvalues_.exists(e)) {
    return sepset_[e];
  } else {
    throw OT::InvalidArgumentException(HERE)
        << "Error: No Sepset for edge (" << e.first() << "," << e.second()
        << ").";
  }
}

void ContinuousPC::setVerbosity(bool verbose) { verbose_ = verbose; };

bool ContinuousPC::getVerbosity() const { return verbose_; };

const std::vector<gum::Edge> &ContinuousPC::getRemoved_() const {
  return removed_;
}

bool ContinuousPC::isRemoved(gum::NodeId x, gum::NodeId y) const {
  return (std::find(removed_.begin(), removed_.end(), gum::Edge(x, y)) !=
          removed_.end());
}

bool ContinuousPC::isRemoved(const std::string &x, const std::string &y) const {
  return isRemoved(idFromName(x), idFromName(y));
}

double ContinuousPC::getPValue(const std::string &x,
                               const std::string &y) const {
  return getPValue(idFromName(x), idFromName(y));
}
double ContinuousPC::getTTest(const std::string &x,
                              const std::string &y) const {
  return getTTest(idFromName(x), idFromName(y));
}

OT::Description
ContinuousPC::getSepsetNames(const std::string &x, const std::string &y) const {
  OT::Description res;
  const auto &description = tester_.getDataDescription();

  auto inds = getSepset(idFromName(x), idFromName(y));
  for (OT::UnsignedInteger i = 0; i < inds.getSize(); i++) {
    res.add(description.at(inds[i]));
  }

  return res;
}

gum::NodeId ContinuousPC::idFromName(const std::string &n) const {
  const auto &description = tester_.getDataDescription();
  for (OT::UnsignedInteger i = 0; i < description.getSize(); i++) {
    if (n == description.at(i)) {
      return gum::NodeId(i);
    }
  }

  throw OT::InvalidArgumentException(HERE)
      << "Error: name '" << n << "' is not a node name.";
}

OT::Description ContinuousPC::getTrace() const {
  const auto &description = tester_.getDataDescription();

  OT::Description res(removed_.size());
  for (OT::UnsignedInteger i = 0; i < removed_.size(); i++) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(3) << i << std::setfill(' ');
    ss << " : ";
    const auto edg = description[removed_[i].first()] + "-" +
                     description[removed_[i].second()];
    std::string cond = "";
    for (const auto s : sepset_[removed_[i]]) {
      if (cond != "") {
        cond += ",";
      }
      cond += description[s];
    }
    ss << std::setw(10) << edg << "|" << std::left << std::setw(20) << cond;
    ss << "  p=" << std::left << std::setw(10) << pvalues_[removed_[i]];
    ss << "  ttest=" << ttests_[removed_[i]];
    res[i] = ss.str();
  }
  return res;
}
} // namespace OTAGRUM
