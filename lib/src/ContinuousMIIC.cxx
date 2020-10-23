#include <agrum/tools/core/list.h>
//#include <chrono>

#include "otagrum/ContinuousMIIC.hxx"

#define TRACE(x)                                                               \
  {                                                                            \
    if (verbose_)                                                              \
      std::cout << x;                                                          \
  }

/*
namespace gum {
    // Hash function for OT::Indices
    template <> class HashFunc<OT::Indices> : public HashFuncBase<OT::Indices> {
        public:
            // Computes the hashed value (hv) of a key
            Size operator()(const OT::Indices &key) const {
                Size hv = 0;
                for(auto x : key) {
                    hv = (x + hv) * HashFuncConst::gold;
                }
                return hv & this->_hash_mask;
            };
    };
} // namespace gum
*/

namespace OTAGRUM
{

ContinuousMIIC::ContinuousMIIC(const OT::Sample &data)
  : OT::Object()
  , info_(data)
{
  // Complete unoriented graph
  for (gum::NodeId i = 0; i < info_.getDimension(); i++)
  {
    skeleton_.addNodeWithId(i);
    for (gum::NodeId j = 0; j < i; j++)
    {
      skeleton_.addEdge(i, j);
    }
  }
}

bool GreaterPairOn2nd::operator()(
  const std::pair <
  std::tuple< OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::Indices >*,
  double > & e1,
  const std::pair <
  std::tuple< OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::Indices >*,
  double > & e2) const
{
  return e1.second > e2.second;
}

bool GreaterTupleOnLast::operator()(
  const std::tuple< std::tuple< OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::UnsignedInteger >*,
  double, double, double >& e1,
  const std::tuple< std::tuple< OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::UnsignedInteger >*,
  double, double, double >& e2) const
{
  double p1XZ = std::get< 2 >(e1);
  double p1YZ = std::get< 3 >(e1);
  double p2XZ = std::get< 2 >(e2);
  double p2YZ = std::get< 3 >(e2);
  double I1 = std::get< 1 >(e1);
  double I2 = std::get< 1 >(e2);
  // First, we look at the sign of information.
  // Then, the probility values
  // and finally the abs value of information.
  if ( (I1 < 0 && I2 < 0) || (I1 >= 0 && I2 >= 0) )
  {
    if (std::max(p1XZ, p1YZ) == std::max(p2XZ, p2YZ))
    {
      return std::abs(I1) > std::abs(I2);
    }
    else
    {
      return std::max(p1XZ, p1YZ) > std::max(p2XZ, p2YZ);
    }
  }
  else
  {
    return I1 < I2;
  }
}

gum::UndiGraph ContinuousMIIC::learnSkeleton()
{
  // clear the vector of latents arcs to be sure
  latent_couples_.clear();

  initiation();
  iteration();

  skeleton_done_ = true;

  return skeleton_;
}

void ContinuousMIIC::setCMode(CorrectedMutualInformation::CModeTypes cmode)
{
  info_.setCMode(cmode);
}

void ContinuousMIIC::setKMode(CorrectedMutualInformation::KModeTypes kmode)
{
  info_.setKMode(kmode);
}

void ContinuousMIIC::setVerbosity(bool verbose)
{
  verbose_ = verbose;
}

bool ContinuousMIIC::getVerbosity() const
{
  return verbose_;
}

void ContinuousMIIC::setAlpha(double alpha)
{
  info_.setAlpha(alpha);
}

double ContinuousMIIC::getAlpha() const
{
  return info_.getAlpha();
}

std::vector< gum::Arc > ContinuousMIIC::getLatentVariables() const
{
  return latent_couples_;
}

//void ContinuousMIIC::addConstraints(
//gum::HashTable< std::pair< gum::NodeId, gum::NodeId >, char > constraints) {
//this->initial_marks_ = constraints;
//}
void ContinuousMIIC::addForbiddenArc(gum::Arc arc)
{
  addForbiddenArc(arc.head(), arc.tail());
}

void ContinuousMIIC::addForbiddenArc(std::string head, std::string tail)
{
  addForbiddenArc(idFromName(head), idFromName(tail));
}

void ContinuousMIIC::addForbiddenArc(OT::UnsignedInteger head, OT::UnsignedInteger tail)
{
  initial_marks_.insert({head, tail}, '-');
}

void ContinuousMIIC::addMandatoryArc(gum::Arc arc)
{
  addMandatoryArc(arc.head(), arc.tail());
}

void ContinuousMIIC::addMandatoryArc(std::string head, std::string tail)
{
  addMandatoryArc(idFromName(head), idFromName(tail));
}

void ContinuousMIIC::addMandatoryArc(OT::UnsignedInteger head, OT::UnsignedInteger tail)
{
  initial_marks_.insert({head, tail}, '>');
}

void ContinuousMIIC::initiation()
{
  unsigned int cutted_edges = 0;
  //auto start_initiation = std::chrono::steady_clock::now();
  TRACE("\n===== STARTING INITIATION =====" << std::endl);
  OT::UnsignedInteger  X, Y;
  gum::EdgeSet edges = skeleton_.edges();

  //std::vector< double > times;
  for (const auto &edge : edges)
  {
    //auto start = std::chrono::steady_clock::now();
    X = edge.first();
    Y = edge.second();
    TRACE("\t\n\nEdge: " << edge << std::endl);
    double IXY = info_.compute2PtCorrectedInformation(X, Y);
    TRACE("\tI(" << X << "," << Y << ") = " << IXY << std::endl);

    if (IXY <= 0)     //< K
    {
      skeleton_.eraseEdge(edge);
      cutted_edges++;
      TRACE("\tCutting edge " << edge << std::endl);
      sepset_.insert(edge, OT::Indices());
    }
    else
    {
      findBestContributor(X, Y, OT::Indices());
    }
    //auto end = std::chrono::steady_clock::now();
    //std::chrono::duration<double> diff = end - start;
    //times.push_back(diff.count());
    //TRACE("\tElapsed time for this edge: " << diff.count() << " s" << std::endl);

  }
  TRACE("===== ENDING INITIATION =====" << std::endl);
  //auto end_initiation = std::chrono::steady_clock::now();
  //std::chrono::duration<double> diff_initiation = end_initiation - start_initiation;
  TRACE("Summary:" << std::endl);
  //TRACE("\tElapsed time for initiation: " << diff_initiation.count()
  //<< " s" << std::endl);
  TRACE("\tNumber of cutted edges: " << cutted_edges << std::endl << std::endl);
}

void ContinuousMIIC::iteration()
{
  unsigned int cutted_edges = 0;
  //auto start_iteration = std::chrono::steady_clock::now();
  TRACE("\n===== STARTING ITERATION =====" << std::endl);
  // if no triples to further examine pass
  std::pair< std::tuple< OT::UnsignedInteger,
      OT::UnsignedInteger,
      OT::UnsignedInteger,
      OT::Indices >*,
      double > best;

  //std::vector< double > times;
  unsigned int n_iterations = 0;
  while (!rank_.empty() && rank_.top().second > 0.5)
  {
    n_iterations++;
    //auto start = std::chrono::steady_clock::now();
    best = rank_.pop();

    const OT::UnsignedInteger X = std::get< 0 >(*(best.first));
    const OT::UnsignedInteger Y = std::get< 1 >(*(best.first));
    const OT::UnsignedInteger Z = std::get< 2 >(*(best.first));
    OT::Indices U = std::move(std::get< 3 >(*(best.first)));
    double proba = best.second;

    TRACE("\n\t((" << X << ", " << Y << ", " << Z << ", " << U << "), "
          << proba << ")" << std::endl);

    U.add(Z);
    const double IXY_U = info_.compute2PtCorrectedInformation(X, Y, U);
    TRACE("\t\tI(" << X << "; " << Y << " | " << U << ") = " << IXY_U << std::endl);
    if (IXY_U < 0)
    {
      skeleton_.eraseEdge(gum::Edge(X, Y));
      cutted_edges++;
      TRACE("\t\tCutting edge " << gum::Edge(X, Y) << " with " << U
            << " as conditional set" << std::endl);
      sepset_.insert(gum::Edge(X, Y), std::move(U));
    }
    else
    {
      findBestContributor(X, Y, U);
    }

    delete best.first;
    //auto end = std::chrono::steady_clock::now();
    //std::chrono::duration<double> diff = end - start;
    //times.push_back(diff.count());
    //TRACE("\tElapsed time for this iteration: " << diff.count() << " s" << std::endl);
  }
  TRACE("===== ENDING ITERATION =====" << std::endl);
  TRACE("Summary:" << std::endl);
  //auto end_iteration = std::chrono::steady_clock::now();
  //std::chrono::duration<double> diff_iteration = end_iteration - start_iteration;
  TRACE("\tNumber of iterations: " << n_iterations << std::endl);
  //TRACE("\tElapsed time for iteration: " << diff_iteration.count()
  //<< " s" << std::endl);
  TRACE("\tNumber of cutted edges: " << cutted_edges << std::endl << std::endl);
}

void ContinuousMIIC::propagatesHead(gum::MixedGraph& graph, gum::NodeId node)
{
  const auto neighbours = graph.neighbours(node);
  TRACE("\tNode: " << node << std::endl);
  TRACE("\t\tNeighbours: " << neighbours << std::endl);
  for (const auto& neighbour : neighbours)
  {
    if (graph.neighbours(neighbour).contains(node))
    {
      if (!existsDirectedPath(graph, neighbour, node)
          && !(initial_marks_.exists({node, neighbour})
               && initial_marks_[ {node, neighbour}] == '-')
          && graph.parents(neighbour).empty())
      {
        graph.eraseEdge(gum::Edge(neighbour, node));
        graph.addArc(node, neighbour);
        TRACE("\t\t1. Removing edge (" << neighbour << "," << node << ")"
              << std::endl);
        TRACE("\t\t1. Adding arc (" << node << "," << neighbour << ")"
              << std::endl);
        propagatesHead(graph, neighbour);
      }
      else if (!existsDirectedPath(graph, node, neighbour)
               && !(initial_marks_.exists({neighbour, node})
                    && initial_marks_[ {neighbour, node}] == '-')
               && graph.parents(node).empty())
      {
        graph.eraseEdge(gum::Edge(neighbour, node));
        graph.addArc(neighbour, node);
        TRACE("\t\t2. Removing edge (" << neighbour << "," << node << ")"
              << std::endl);
        TRACE("\t\t2. Adding arc (" << neighbour << "," << node << ")"
              << std::endl);
      }
      else if (!existsDirectedPath(graph, node, neighbour)
               && !(initial_marks_.exists({neighbour, node})
                    && initial_marks_[ {neighbour, node}] == '-'))
      {
        graph.eraseEdge(gum::Edge(neighbour, node));
        graph.addArc(neighbour, node);
        if (!graph.parents(neighbour).empty()
            && !graph.parents(node).empty())
        {
          latent_couples_.push_back(gum::Arc(node, neighbour));
        }

        TRACE("\t\t3. Removing edge (" << neighbour << "," << node << ")"
              << std::endl);
        TRACE("\t\t3. Adding arc (" << neighbour << "," << node << ")"
              << std::endl);
      }
      else if (!existsDirectedPath(graph, neighbour, node)
               && !(initial_marks_.exists({node, neighbour})
                    && initial_marks_[ {node, neighbour}] == '-'))
      {
        graph.eraseEdge(gum::Edge(node, neighbour));
        graph.addArc(node, neighbour);
        if (!graph.parents(neighbour).empty()
            && !graph.parents(node).empty())
        {
          latent_couples_.push_back(gum::Arc(node, neighbour));
        }
        TRACE("\t\t4. Removing edge (" << neighbour << "," << node << ")"
              << std::endl);
        TRACE("\t\t4. Adding arc (" << node << "," << neighbour << ")"
              << std::endl);
      }
      //else if (!graph.parents(neighbour).empty()
      //&& !graph.parents(node).empty()) {
      //graph.eraseEdge(Edge(neighbour, node));
      //graph.addArc(node, neighbour);
      //__latent_couples.push_back(Arc(node, neighbour));
      //}
      else
      {
        graph.eraseEdge(gum::Edge(neighbour, node));
        TRACE("\t\t5. Removing edge (" << neighbour << "," << node << ")"
              << std::endl);
      }
    }
  }
}

gum::MixedGraph ContinuousMIIC::UGtoMG(const gum::UndiGraph& graph) const
{
  // Should be added in aGrUM as a constructor for MixedGraph alongside
  // another for DAGs.
  auto mixed_graph = gum::MixedGraph();
  for (const auto & nodeID : graph.nodes())
  {
    mixed_graph.addNodeWithId(nodeID);
  }
  for (const auto & edge : graph.edges())
  {
    auto X = edge.first();
    auto Y = edge.second();
    mixed_graph.addEdge(X, Y);
  }
  return mixed_graph;
}


gum::MixedGraph ContinuousMIIC::learnPDAG()
{
  TRACE("\n===== STARTING PDAG LEARNING =====" << std::endl);
  if (pdag_done_)
  {
    return pdag_;
  }
  if (!skeleton_done_)
  {
    learnSkeleton();
  }
  pdag_ = UGtoMG(skeleton_);

  // structure to store the orientations marks -, o, or >,
  // Considers the head of the arc/edge first node -* second node
  gum::HashTable< std::pair< gum::NodeId, gum::NodeId >, char > marks = initial_marks_;

  // marks always correspond to the head of the arc/edge. - is for a forbidden
  // arc, > for a mandatory arc
  // we start by adding the mandatory arcs
  for (auto iter = marks.begin(); iter != marks.end(); ++iter)
  {
    if (pdag_.existsEdge(iter.key().first, iter.key().second)
        && iter.val() == '>')
    {
      pdag_.eraseEdge(gum::Edge(iter.key().first, iter.key().second));
      pdag_.addArc(iter.key().first, iter.key().second);
    }
  }

  std::vector< std::tuple< std::tuple< OT::UnsignedInteger,
      OT::UnsignedInteger,
      OT::UnsignedInteger >*,
      double,
      double,
      double > >
      proba_triples = getUnshieldedTriples(pdag_, marks);


  std::tuple< std::tuple< OT::UnsignedInteger,
      OT::UnsignedInteger,
      OT::UnsignedInteger >*, double, double, double >
      best;

  if (!proba_triples.empty())
  {
    best = proba_triples[0];
  }

  while (!proba_triples.empty()
         && std::max(std::get< 2 >(best), std::get< 3 >(best)) > 0.5)
  {
    OT::UnsignedInteger X, Y, Z;
    X = std::get< 0 >(*std::get< 0 >(best));
    Y = std::get< 1 >(*std::get< 0 >(best));
    Z = std::get< 2 >(*std::get< 0 >(best));
    TRACE("\tTriple: (" << X << "," << Y << "," << Z << ")" << std::endl);

    const double I3 = std::get< 1 >(best);

    if (I3 <= 0)
    {
      // v-structure discovery
      if (marks[ {X, Z}] == 'o' && marks[ {Y, Z}] == 'o')  // If x-z-y
      {
        if (!existsDirectedPath(pdag_, Z, X, false))
        {
          pdag_.eraseEdge(gum::Edge(X, Z));
          pdag_.addArc(X, Z);
          TRACE("\t\t1.a Removing edge (" << X << "," << Z << ")" << std::endl);
          TRACE("\t\t1.a Adding arc (" << X << "," << Z << ")" << std::endl);
          marks[ {X, Z}] = '>';
          if (pdag_.existsArc(Z, X)
              && std::find(
                latent_couples_.begin(),
                latent_couples_.end(),
                gum::Arc(Z, X)) == latent_couples_.end()
              && std::find(
                latent_couples_.begin(),
                latent_couples_.end(),
                gum::Arc(X, Z)) == latent_couples_.end())
          {
            TRACE("\t\tAdding latent couple (" << Z << "," << X << ")"
                  << std::endl);
            latent_couples_.push_back(gum::Arc(Z, X));
          }
          if (!arc_probas_.exists(gum::Arc(X, Z)))
            arc_probas_.insert(gum::Arc(X, Z), std::get< 2 >(best));
        }
        else
        {
          pdag_.eraseEdge(gum::Edge(X, Z));
          TRACE("\t\t1.b Adding arc (" << X << "," << Z << ")" << std::endl);
          if (!existsDirectedPath(pdag_, X, Z, false))
          {
            pdag_.addArc(Z, X);
            TRACE("\t\t1.b Removing edge (" << X << "," << Z << ")"
                  << std::endl);
            marks[ {Z, X}] = '>';
          }
        }

        if (!existsDirectedPath(pdag_, Z, Y, false))
        {
          pdag_.eraseEdge(gum::Edge(Y, Z));
          pdag_.addArc(Y, Z);
          TRACE("\t\t1.c Removing edge (" << Y << "," << Z << ")"
                << std::endl);
          TRACE("\t\t1.c Adding arc (" << Y << "," << Z << ")"
                << std::endl);
          marks[ {Y, Z}] = '>';
          if (pdag_.existsArc(Z, Y)
              && std::find(
                latent_couples_.begin(),
                latent_couples_.end(),
                gum::Arc(Z, Y)) == latent_couples_.end()
              && std::find(
                latent_couples_.begin(),
                latent_couples_.end(),
                gum::Arc(Y, Z)) == latent_couples_.end())
          {
            latent_couples_.push_back(gum::Arc(Z, Y));
          }
          auto arc = gum::Arc(Y, Z);
          if (!arc_probas_.exists(arc))
            arc_probas_.insert(arc, std::get< 3 >(best));
        }
        else
        {
          pdag_.eraseEdge(gum::Edge(Y, Z));
          TRACE("\t\t1.d Removing edge (" << Y << "," << Z << ")"
                << std::endl);
          if (!existsDirectedPath(pdag_, Y, Z, false))
          {
            pdag_.addArc(Z, Y);
            TRACE("\t\t1.d Adding arc (" << Z << "," << Y << ")"
                  << std::endl);
            marks[ {Z, Y}] = '>';
          }
        }
      }
      else if (marks[ {X, Z}] == '>' && marks[ {Y, Z}] == 'o') // If x->z-y
      {
        if (!existsDirectedPath(pdag_, Z, Y, false))
        {
          pdag_.eraseEdge(gum::Edge(Y, Z));
          pdag_.addArc(Y, Z);
          TRACE("\t\t2.a Removing edge (" << Y << "," << Z << ")" << std::endl);
          TRACE("\t\t2.a Adding arc (" << Y << "," << Z << ")" << std::endl);
          marks[ {Y, Z}] = '>';
          if (pdag_.existsArc(Z, Y)
              && std::find(
                latent_couples_.begin(),
                latent_couples_.end(),
                gum::Arc(Z, Y)) == latent_couples_.end()
              && std::find(
                latent_couples_.begin(),
                latent_couples_.end(),
                gum::Arc(Y, Z)) == latent_couples_.end())
          {
            latent_couples_.push_back(gum::Arc(Z, Y));
          }
          auto arc = gum::Arc(Y, Z);
          if (!arc_probas_.exists(arc))
            arc_probas_.insert(arc, std::get< 3 >(best));
        }
        else
        {
          pdag_.eraseEdge(gum::Edge(Y, Z));
          TRACE("\t\t2.b Removing edge (" << Y << "," << Z << ")"
                << std::endl);
          if (!existsDirectedPath(pdag_, Y, Z, false))
          {
            pdag_.addArc(Z, Y);
            TRACE("\t\t2.b Adding arc (" << Y << "," << Z << ")"
                  << std::endl);
            marks[ {Z, Y}] = '>';
          }
        }
      }
      else if (marks[ {Y, Z}] == '>' && marks[ {X, Z}] == 'o')
      {
        if (!existsDirectedPath(pdag_, Z, X, false))
        {
          pdag_.eraseEdge(gum::Edge(X, Z));
          pdag_.addArc(X, Z);
          TRACE("\t\t3.a Removing edge (" << X << "," << Z << ")"
                << std::endl);
          TRACE("\t\t3.a Adding arc (" << X << "," << Z << ")"
                << std::endl);
          marks[ {X, Z}] = '>';
          if (pdag_.existsArc(Z, X)
              && std::find(
                latent_couples_.begin(),
                latent_couples_.end(),
                gum::Arc(Z, X)) == latent_couples_.end()
              && std::find(
                latent_couples_.begin(),
                latent_couples_.end(),
                gum::Arc(X, Z)) == latent_couples_.end())
          {
            latent_couples_.push_back(gum::Arc(Z, X));
          }
          auto arc = gum::Arc(X, Z);
          if (!arc_probas_.exists(arc))
            arc_probas_.insert(arc, std::get< 2 >(best));
        }
        else
        {
          pdag_.eraseEdge(gum::Edge(X, Z));
          TRACE("\t\t3.b Removing edge (" << X << "," << Z << ")"
                << std::endl);
          if (!existsDirectedPath(pdag_, X, Z, false))
          {
            pdag_.addArc(Z, X);
            TRACE("\t\t3.b Adding arc (" << X << "," << Z << ")"
                  << std::endl);
            marks[ {Z, X}] = '>';
          }
        }
      }

    }
    else
    {
      // orientation propagation
      if (marks[ {X, Z}] == '>' && marks[ {Y, Z}] == 'o'
          && marks[ {Z, Y}] != '-')
      {
        pdag_.eraseEdge(gum::Edge(Z, Y));
        TRACE("\t\t4. Removing edge (" << Z << "," << Y << ")"
              << std::endl);
        if (!existsDirectedPath(pdag_, Y, Z) && pdag_.parents(Y).empty())
        {
          pdag_.addArc(Z, Y);
          TRACE("\t\t4.a Adding arc (" << Z << "," << Y << ")"
                << std::endl);
          marks[ {Z, Y}] = '>';
          marks[ {Y, Z}] = '-';
          if (!arc_probas_.exists(gum::Arc(Z, Y)))
            arc_probas_.insert(gum::Arc(Z, Y), std::get< 3 >(best));
        }
        else if (!existsDirectedPath(pdag_, Z, Y)
                 && pdag_.parents(Z).empty())
        {
          pdag_.addArc(Y, Z);
          TRACE("\t\t4.b Adding arc (" << Y << "," << Z << ")"
                << std::endl);
          marks[ {Z, Y}] = '-';
          marks[ {Y, Z}] = '>';
          latent_couples_.push_back(gum::Arc(Y, Z));
          if (!arc_probas_.exists(gum::Arc(Y, Z)))
            arc_probas_.insert(gum::Arc(Y, Z), std::get< 3 >(best));
        }
        else if (!existsDirectedPath(pdag_, Y, Z))
        {
          pdag_.addArc(Z, Y);
          TRACE("\t\t4.c Adding arc (" << Z << "," << Y << ")"
                << std::endl);
          marks[ {Z, Y}] = '>';
          marks[ {Y, Z}] = '-';
          if (!arc_probas_.exists(gum::Arc(Z, Y)))
            arc_probas_.insert(gum::Arc(Z, Y), std::get< 3 >(best));
        }
        else if (!existsDirectedPath(pdag_, Z, Y))
        {
          pdag_.addArc(Y, Z);
          TRACE("\t\t4.d Adding arc (" << Y << "," << Z << ")"
                << std::endl);
          latent_couples_.push_back(gum::Arc(Y, Z));
          marks[ {Z, Y}] = '-';
          marks[ {Y, Z}] = '>';
          if (!arc_probas_.exists(gum::Arc(Y, Z)))
            arc_probas_.insert(gum::Arc(Y, Z), std::get< 3 >(best));
        }

      }
      else if (marks[ {Y, Z}] == '>' && marks[ {X, Z}] == 'o'
               && marks[ {Z, X}] != '-')
      {
        pdag_.eraseEdge(gum::Edge(Z, X));
        TRACE("\t\t5. Removing edge (" << Z << "," << X << ")"
              << std::endl);
        if (!existsDirectedPath(pdag_, X, Z) && pdag_.parents(X).empty())
        {
          pdag_.addArc(Z, X);
          TRACE("\t\t5.a Adding arc (" << Z << "," << X << ")"
                << std::endl);
          marks[ {Z, X}] = '>';
          marks[ {X, Z}] = '-';
          if (!arc_probas_.exists(gum::Arc(Z, X)))
            arc_probas_.insert(gum::Arc(Z, X), std::get< 2 >(best));
        }
        else if (!existsDirectedPath(pdag_, Z, X)
                 && pdag_.parents(Z).empty())
        {
          pdag_.addArc(X, Z);
          TRACE("\t\t5.b Adding arc (" << X << "," << Z << ")"
                << std::endl);
          marks[ {Z, X}] = '-';
          marks[ {X, Z}] = '>';
          latent_couples_.push_back(gum::Arc(X, Z));
          if (!arc_probas_.exists(gum::Arc(X, Z)))
            arc_probas_.insert(gum::Arc(X, Z), std::get< 2 >(best));
        }
        else if (!existsDirectedPath(pdag_, X, Z))
        {
          pdag_.addArc(Z, X);
          TRACE("\t\t5.c Adding arc (" << Z << "," << X << ")"
                << std::endl);
          marks[ {Z, X}] = '>';
          marks[ {X, Z}] = '-';
          if (!arc_probas_.exists(gum::Arc(Z, X)))
            arc_probas_.insert(gum::Arc(Z, X), std::get< 2 >(best));
        }
        else if (!existsDirectedPath(pdag_, Z, X))
        {
          pdag_.addArc(X, Z);
          TRACE("\t\t5.d Adding arc (" << X << "," << Z << ")"
                << std::endl);
          marks[ {Z, X}] = '-';
          marks[ {X, Z}] = '>';
          latent_couples_.push_back(gum::Arc(X, Z));
          if (!arc_probas_.exists(gum::Arc(X, Z)))
            arc_probas_.insert(gum::Arc(X, Z), std::get< 2 >(best));
        }
      }
    }

    delete std::get< 0 >(best);
    proba_triples.erase(proba_triples.begin());

    // actualisation of the list of triples
    proba_triples = updateProbaTriples(pdag_, proba_triples);


    if (!proba_triples.empty())
    {
      best = proba_triples[0];
    }


  }   // while

  // erasing the double headed arcs
  for (auto iter = latent_couples_.rbegin(); iter != latent_couples_.rend();
       ++iter)
  {
    pdag_.eraseArc(gum::Arc(iter->head(), iter->tail()));
    if (existsDirectedPath(pdag_, iter->head(), iter->tail()))
    {
      // if we find a cycle, we force the competing edge
      pdag_.addArc(iter->head(), iter->tail());
      pdag_.eraseArc(gum::Arc(iter->tail(), iter->head()));
      *iter = gum::Arc(iter->head(), iter->tail());
    }
  }

  pdag_done_ = true;
  return pdag_;
  TRACE("===== ENDING PDAG LEARNING =====" << std::endl);
}

NamedDAG ContinuousMIIC::learnDAG()
{
  TRACE("\n===== STARTING DAG LEARNING =====" << std::endl);
  if (dag_done_)
  {
    return dag_;
  }
  if(!pdag_done_)
  {
    learnPDAG();
  }

  // Second, orientate remaining edges
  const gum::Sequence< gum::NodeId > order = pdag_.topologicalOrder();
  // first, propagate existing orientations
  for (gum::NodeId x : order)
  {
    if (!pdag_.parents(x).empty())
    {
      propagatesHead(pdag_, x);
    }
  }
  // then decide the orientation by the topological order and propagate them
  for (gum::NodeId x : order)
  {
    if (!pdag_.neighbours(x).empty())
    {
      propagatesHead(pdag_, x);
    }
  }

  gum::DAG dag;
  for (auto node : pdag_)
  {
    dag.addNodeWithId(node);
  }
  for (const gum::Arc& arc : pdag_.arcs())
  {
    dag.addArc(arc.tail(), arc.head());
  }

  dag_ = NamedDAG(dag, namesFromData());
  dag_done_ = true;

  return dag_;
  TRACE("===== ENDING DAG LEARNING =====" << std::endl);
}

gum::NodeId ContinuousMIIC::idFromName(const std::string& name) const
{
  const auto &description = info_.getDataDescription();
  for (OT::UnsignedInteger i = 0; i < description.getSize(); i++)
  {
    if (name == description.at(i))
    {
      return gum::NodeId(i);
    }
  }

  throw OT::InvalidArgumentException(HERE) << "Error: name '" << name
      << "' is not a node name.";
}

std::vector< std::string > ContinuousMIIC::namesFromData() const
{
  std::vector< std::string > names;
  const auto &description = info_.getDataDescription();
  for (OT::UnsignedInteger i = 0; i < description.getSize(); i++)
  {
    names.push_back(description.at(i));
  }
  return names;
}


std::vector< std::tuple< std::tuple< OT::UnsignedInteger,
OT::UnsignedInteger,
OT::UnsignedInteger >*,
double, double, double > >
ContinuousMIIC::getUnshieldedTriples(
  const gum::MixedGraph& graph,
  gum::HashTable< std::pair< gum::NodeId, gum::NodeId >, char >& marks)
{
  TRACE("\tLooking for unshielded triples" << std::endl);
  std::vector< std::tuple< std::tuple< OT::UnsignedInteger,
      OT::UnsignedInteger,
      OT::UnsignedInteger >*,
      double,
      double,
      double > > triples;

  for (gum::NodeId Z : graph)
  {
    for (gum::NodeId X : graph.neighbours(Z))
    {
      for (gum::NodeId Y : graph.neighbours(Z))
      {
        if (Y < X && !graph.existsEdge(X, Y))
        {
          OT::Indices U;
          auto key = gum::Edge(X, Y);
          //std::pair< gum::NodeId, gum::NodeId > rev_key = {Y, X};
          if (sepset_.exists(key))
          {
            U = sepset_[key];
          }
          //else if (sepset_.exists(rev_key)) {
          //U = sepset_[rev_key];
          //}
          // remove z from ui if it's present
          const auto iter_Z_place = std::find(U.begin(), U.end(), Z);
          if (iter_Z_place != U.end())
          {
            U.erase(iter_Z_place);
          }

          const double IXYZ_U =
            info_.compute3PtCorrectedInformation(X, Y, Z, U);
          auto tup = new std::tuple< OT::UnsignedInteger,
          OT::UnsignedInteger,
          OT::UnsignedInteger > {X, Y, Z};
          std::tuple< std::tuple< OT::UnsignedInteger,
              OT::UnsignedInteger,
              OT::UnsignedInteger >*,
              double,
              double,
              double > triple{tup, IXYZ_U, 0.5, 0.5};
          triples.push_back(triple);
          if (!marks.exists({X, Z}))
          {
            marks.insert({X, Z}, 'o');
          }
          if (!marks.exists({Z, X}))
          {
            marks.insert({Z, X}, 'o');
          }
          if (!marks.exists({Y, Z}))
          {
            marks.insert({Y, Z}, 'o');
          }
          if (!marks.exists({Z, Y}))
          {
            marks.insert({Z, Y}, 'o');
          }
        }
      }
    }
  }
  triples = updateProbaTriples(graph, triples);
  std::sort(triples.begin(), triples.end(), GreaterTupleOnLast());
  for (const auto & triple : triples)
  {
    OT::UnsignedInteger X, Y, Z;
    X = std::get< 0 >(*std::get< 0 >(triple));
    Y = std::get< 1 >(*std::get< 0 >(triple));
    Z = std::get< 2 >(*std::get< 0 >(triple));

    const double IXYZ = std::get< 1 >(triple);
    double       PXZ = std::get< 2 >(triple);
    double       PYZ = std::get< 3 >(triple);

    TRACE("\t\t(" << X << ", " << Y << ", " << Z << ", "
          << IXYZ << ", " << PXZ << ", " << PYZ << ")" << std::endl);
  }
  return triples;
}

std::vector< std::tuple< std::tuple< OT::UnsignedInteger,
OT::UnsignedInteger,
OT::UnsignedInteger >*,
double, double, double > >
ContinuousMIIC::updateProbaTriples(
  const gum::MixedGraph& graph,
  std::vector< std::tuple< std::tuple< OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::UnsignedInteger >*,
  double, double, double > > proba_triples)
{
  for (auto& triple : proba_triples)
  {
    OT::UnsignedInteger X, Y, Z;
    X = std::get< 0 >(*std::get< 0 >(triple));
    Y = std::get< 1 >(*std::get< 0 >(triple));
    Z = std::get< 2 >(*std::get< 0 >(triple));
    const double IXYZ = std::get< 1 >(triple);
    double       PXZ = std::get< 2 >(triple);
    double       PYZ = std::get< 3 >(triple);

    if (IXYZ <= 0)
    {
      const double expo = std::exp(static_cast<double>(info_.getSize()) * IXYZ);
      const double P0 = (1 + expo) / (1 + 3 * expo);
      // distinguish betweeen the initialization and the update process
      if (PXZ == PYZ && PYZ == 0.5)
      {
        std::get< 2 >(triple) = P0;
        std::get< 3 >(triple) = P0;
      }
      else
      {
        if (graph.existsArc(X, Z) && PXZ >= P0)
        {
          std::get< 3 >(triple) = PXZ * (1 / (1 + expo) - 0.5) + 0.5;
        }
        else if (graph.existsArc(Y, Z) && PYZ >= P0)
        {
          std::get< 2 >(triple) = PYZ * (1 / (1 + expo) - 0.5) + 0.5;
        }
      }
    }
    else
    {
      const double expo = std::exp(- static_cast<double>(info_.getSize()) * IXYZ);
      if (graph.existsArc(X, Z) && PXZ >= 0.5)
      {
        std::get< 3 >(triple) = PXZ * (1 / (1 + expo) - 0.5) + 0.5;
      }
      else if (graph.existsArc(Y, Z) && PYZ >= 0.5)
      {
        std::get< 2 >(triple) = PYZ * (1 / (1 + expo) - 0.5) + 0.5;
      }
    }
  }
  std::sort(proba_triples.begin(), proba_triples.end(), GreaterTupleOnLast());
  return proba_triples;
}

bool ContinuousMIIC::existsDirectedPath(const gum::MixedGraph& graph,
                                        const OT::UnsignedInteger n1,
                                        const OT::UnsignedInteger n2,
                                        const bool countArc) const
{
  // not recursive version => use a FIFO for simulating the recursion
  gum::List< gum::NodeId > nodeFIFO;
  nodeFIFO.pushBack(n2);

  // mark[node] = successor if visited, else mark[node] does not exist
  gum::NodeProperty< gum::NodeId > mark;
  mark.insert(n2, n2);

  gum::NodeId current;

  while (!nodeFIFO.empty())
  {
    current = nodeFIFO.front();
    nodeFIFO.popFront();

    // check the parents

    for (const auto & new_one : graph.parents(current))
    {
      if (!countArc && current == n2 && new_one == n1)  // If countArc is set to false
        continue;                                       // paths of length 1 are ignored

      if (mark.exists(new_one))   // if this node is already marked, do not
        continue;                 // check it again

      if (graph.existsArc(current,
                          new_one))   // if there is a double arc, pass
        continue;

      mark.insert(new_one, current);

      if (new_one == n1)
      {
        return true;
      }

      nodeFIFO.pushBack(new_one);
    }
  }

  return false;
}


void ContinuousMIIC::findBestContributor(const OT::UnsignedInteger X,
    const OT::UnsignedInteger Y,
    const OT::Indices &U)
{
  TRACE("\n\tLooking for best contributor" << std::endl);
  double maxP = -1.;
  OT::UnsignedInteger maxZ = 0;

  const double IXY_U = info_.compute2PtCorrectedInformation(X, Y, U);

  for (const auto & Z : skeleton_.nodes())
  {
    if (Z != X && Z != Y && std::find(U.begin(), U.end(), Z) == U.end())
    {
      TRACE("\t\tCondidering node " << Z << std::endl);
      double Pnv = 0.;
      double Pb = 0.;

      const double IXYZ_U = info_.compute3PtCorrectedInformation(X, Y, Z, U);
      TRACE("\t\t\tI(" << X << ", " << Y << ", " << Z << " | "
            << U << ") = " << IXYZ_U << std::endl);
      double f1 = - static_cast<double>(info_.getSize()) * IXYZ_U;

      if (f1 > maxLog_)
      {
        Pnv = 0.;
      }
      else if (f1 < -maxLog_)
      {
        Pnv = 1.;
      }
      else
      {
        Pnv = 1 / (1 + std::exp(f1));
      }
      TRACE("\t\t\tPnv = " << Pnv << std::endl);

      const double IXZ_U = info_.compute2PtCorrectedInformation(X, Z, U);
      const double IYZ_U = info_.compute2PtCorrectedInformation(Y, Z, U);
      TRACE("\t\t\tI(" << X << ", " << Y << " | "
            << U << ") = " << IXY_U << std::endl);
      TRACE("\t\t\tI(" << X << ", " << Z << " | "
            << U << ") = " << IXZ_U << std::endl);
      TRACE("\t\t\tI(" << Y << ", " << Z << " | "
            << U << ") = " << IYZ_U << std::endl);

      f1 = - static_cast<double>(info_.getSize()) * (IXZ_U - IXY_U);
      double f2 = - static_cast<double>(info_.getSize()) * (IYZ_U - IXY_U);


      if (f1 > maxLog_ || f2 > maxLog_)
      {
        Pb = 0.;
      }
      else if (f1 < -maxLog_ && f2 < -maxLog_)
      {
        Pb = 1.;
      }
      else
      {
        double expo1 = 0.;
        double expo2 = 0.;
        if (f1 < -maxLog_)
        {
          expo1 = 0.;
        }
        else
        {
          expo1 = std::exp(f1);
        }
        if (f2 < -maxLog_)
        {
          expo2 = 0.;
        }
        else
        {
          expo2 = std::exp(f2);
        }
        Pb = 1 / (1 + expo1 + expo2);
      }
      TRACE("\t\t\tPb = " << Pb << std::endl);

      const double min_pnv_pb = std::min(Pnv, Pb);
      if (min_pnv_pb > maxP)
      {
        maxP = min_pnv_pb;
        maxZ = Z;
      }
    }
  }
  TRACE("\tBest contributor: (" << maxZ << ", " << maxP << ")" << std::endl);

  std::pair< std::tuple< OT::UnsignedInteger,
      OT::UnsignedInteger,
      OT::UnsignedInteger,
      OT::Indices >*, double > final_pair;

  auto tup = new std::tuple< OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::Indices > {X, Y, maxZ, U};
  final_pair.first = tup;
  final_pair.second = maxP;
  rank_.insert(final_pair);
}

} // namespace OTAGRUM
