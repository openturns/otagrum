//                                               -*- C++ -*-
/**
 *  @brief ContinuousPC
 *
 *  Copyright 2010-2018 Airbus-LIP6-Phimeca
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
 * @param maxParents : the max parents than a node can have
 * @param alpha : the threshold for the hypothesis test
 */
ContinuousPC::ContinuousPC(const OT::Sample &data,
                           const OT::UnsignedInteger maxParents,
                           const double alpha)
    : OT::Object(), maxParents_(maxParents), verbose_(false),
      optimalPolicy_(true), tester_(data) {
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
ContinuousPC::bestSeparator(const gum::UndiGraph &g, gum::NodeId y,
                            gum::NodeId z, const OT::Indices &neighbours,
                            OT::UnsignedInteger n) {
  double t = 0.0;
  double p = 0.0;
  double pmax = -1.0;
  double tmax = 0.0;
  bool res = false;

  OT::Indices bestsep;

  IndicesCombinationIterator separator(neighbours, n);
  for (separator.setFirst(); !separator.isLast(); separator.next()) {
    bool ok = false;
    std::tie(t, p, ok) = tester_.isIndep(y, z, separator.current());
    TRACE(y << "-" << z << "|" << separator.current() << ":" << t << ", " << p
            << ", " << ok << "\n")

    if (ok) {
      if (!getOptimalPolicy()) // the first separator found is correct
      {
        return std::make_tuple(true, t, p, separator.current());
      }

      res = true;
      if (p > pmax) {
        bestsep = separator.current();
      }
    }
    if (p > pmax) {
      pmax = p;
      tmax = t;
    }
  }
  return std::make_tuple(res, tmax, pmax, bestsep);
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

  gum::PriorityQueue<gum::Edge, double> queue;
  bool atLeastOneInThisStep = false;

  do {
    atLeastOneInThisStep = false;
    queue.clear();
    for (const auto &edge : g.edges()) {
      const auto y = edge.first();
      const auto z = edge.second();
      const auto nei = g.neighbours(y) * g.neighbours(z);

      if (nei.size() >= n) {
        bool resYZ = false;
        double pYZ = 0.0, tYZ = 0.0;
        OT::Indices sepYZ;

        std::tie(resYZ, tYZ, pYZ, sepYZ) =
            bestSeparator(g, y, z, Utils::FromNodeSet(nei), n);

        pvalues_.set(edge, std::max(pvalues_.getWithDefault(edge, 0), pYZ));
        ttests_.set(edge, std::max(ttests_.getWithDefault(edge, -10000), tYZ));

        if (resYZ) // we found at least one separator
        {
          queue.insert(edge, -pYZ);
          sepset_.set(edge, sepYZ);
        }
      }
    }

    gum::EdgeSet uncutable;
    uncutable.clear();
    gum::NodeId u;
    while (!queue.empty()) {
      const auto &edge = queue.pop();
      if (!uncutable.contains(edge)) {
        TRACE("==>" << edge << " cut. Sepset=" << sepset_[edge]
                    << ", pvalue=" << pvalues_[edge] << std::endl);
        atLeastOneInThisStep = true;
        removed_.push_back(edge);
        g.eraseEdge(edge);

        // add the uncuttable in the case of optimalPolicy
        if (getOptimalPolicy()) {
          u = edge.first();
          for (const auto &v : g.neighbours(u))
            uncutable.insert(gum::Edge(u, v));
          u = edge.second();
          for (const auto &v : g.neighbours(u))
            uncutable.insert(gum::Edge(u, v));
        }
      }
    }
  } while (atLeastOneInThisStep);

  return true;
}

// From complete graph g, remove as much as possible edge (y,z) in g
// if (y,z) is removed, it means that sepset_[Edge(y,z)] contains X, set of
// nodes, such that y and z are tested as independent given X.
gum::UndiGraph ContinuousPC::getSkeleton() {
  gum::UndiGraph g;
  tester_.clearCache();
  sepset_.clear();
  pvalues_.clear();

  TRACE("== PC algo starting " << std::endl);
  // create the complete graph
  for (gum::NodeId i = 0; i < tester_.getDimension(); ++i) {
    g.addNodeWithId(i);
    for (gum::NodeId j = 0; j < i; ++j) {
      g.addEdge(i, j);
    }
  }

  // for each size of sepset from 0 to n-1
  for (OT::UnsignedInteger n = 0; n < maxParents_; ++n) {
    TRACE("==  Size of conditioning set " << n << std::endl);
    // clear the pdfs not used anymore (due to the dimension of data)
    if (n > 0)
      tester_.clearCacheLevel(n - 1);

    // perform all the tests for size n
    if (!testCondSetWithSize(g, n))
      break;
  }
  TRACE("== end" << std::endl);

  return g;
}

// for all triplet x-y-z (no edge between x and z), if y is in sepset[x,z]
// then x->y<-z.
// the ordering process uses the size of the p-value as a priority.
gum::MixedGraph ContinuousPC::getPDAG(const gum::UndiGraph &g) const {
  gum::MixedGraph cpdag;

  gum::PriorityQueue<Triplet, double> queue;
  for (auto x : g.nodes()) {
    cpdag.addNodeWithId(x);

    if (g.neighbours(x).size() > 1) {
      IndicesCombinationIterator couple(Utils::FromNodeSet(g.neighbours(x)), 2);
      for (couple.setFirst(); !couple.isLast(); couple.next()) {
        bool ok = false;
        double t = 0.0, p = 0.0;
        const gum::NodeId y = couple.current()[0];
        const gum::NodeId z = couple.current()[1];
        if (!g.existsEdge(y, z)) // maybe unshielded collider
        {
          // if (OTAGR::isIn(sepset_[gum::Edge(x, z)], y)) {
          //  queue.insert(Triplet{x, y, z}, pvalues_[gum::Edge(x, z)]);
          //}
          OT::Indices indX;
          indX = indX + OT::UnsignedInteger(x);
          std::tie(t, p, ok) = tester_.isIndep(y, z, indX);
          if (!ok) {
            queue.insert(Triplet{y, x, z}, p);
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

gum::UndiGraph ContinuousPC::getMoralGraph(const gum::MixedGraph &g) const {
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

NamedJunctionTree ContinuousPC::getJunctionTree(const gum::UndiGraph &g) const {
  gum::DefaultTriangulation triangulation;
  gum::HashTable<std::string,int> mods;
  std::vector<std::string> names;

  for (int i = 0; i < map_.len(); i++) {
    mods.insert(map_[i],2);
    names.push_back(); // triangulation needs modalities. We just say that mods
                       // are all the same
  }
  triangulation.setGraph(&g, &mods);

  return NamedJunctionTree(triangulation.junctionTree(), names);
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

double ContinuousPC::getPValue(gum::NodeId x, gum::NodeId y) {
  gum::Edge e(x, y);
  if (pvalues_.exists(e)) {
    return pvalues_[e];
  } else {
    throw OT::InvalidArgumentException(HERE)
        << "Error: No p-value for edge (" << e.first() << "," << e.second()
        << ").";
  }
}
double ContinuousPC::getTTest(gum::NodeId x, gum::NodeId y) {
  gum::Edge e(x, y);
  if (ttests_.exists(e)) {
    return ttests_[e];
  } else {
    throw OT::InvalidArgumentException(HERE)
        << "Error: No ttest value for edge (" << e.first() << "," << e.second()
        << ").";
  }
}

const OT::Indices ContinuousPC::getSepset(gum::NodeId x, gum::NodeId y) {
  gum::Edge e(x, y);
  if (pvalues_.exists(e)) {
    return sepset_[e];
  } else {
    throw OT::InvalidArgumentException(HERE)
        << "Error: No Sepset for edge (" << e.first() << "," << e.second()
        << ").";
  }
}

} // namespace OTAGRUM
