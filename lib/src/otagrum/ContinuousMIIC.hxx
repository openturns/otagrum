#ifndef OTAGRUM_CONTINUOUSMIIC_HXX
#define OTAGRUM_CONTINUOUSMICC_HXX

#include <agrum/tools/graphs/undiGraph.h>
#include <agrum/tools/graphs/mixedGraph.h>
#include <agrum/tools/core/heap.h>
#include <agrum/tools/core/hashTable.h>

#include <openturns/Sample.hxx>

#include "otagrum/NamedDAG.hxx"
#include "otagrum/CorrectedMutualInformation.hxx"
#include "otagrum/IndicesManip.hxx"

namespace OTAGRUM
{

class GreaterPairOn2nd
{
public:
  bool operator()(
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
    double > & e2) const;
};


class GreaterTupleOnLast
{
public:
  bool operator()(
    const std::tuple< std::tuple< OT::UnsignedInteger,
    OT::UnsignedInteger,
    OT::UnsignedInteger >*,
    double, double, double >& e1,
    const std::tuple< std::tuple< OT::UnsignedInteger,
    OT::UnsignedInteger,
    OT::UnsignedInteger >*,
    double, double, double >& e2) const;
};

class OTAGRUM_API ContinuousMIIC : public OT::Object
{
public:
  // Constructor
  explicit ContinuousMIIC(const OT::Sample &data);

  gum::UndiGraph learnSkeleton();
  gum::MixedGraph learnPDAG();
  NamedDAG learnDAG();

  const std::vector< gum::Arc > getLatentVariables() const;
  void addForbiddenArc(gum::Arc arc);
  void addForbiddenArc(std::string head, std::string tail);
  void addForbiddenArc(OT::UnsignedInteger head, OT::UnsignedInteger tail);
  void addMandatoryArc(gum::Arc arc);
  void addMandatoryArc(std::string head, std::string tail);
  void addMandatoryArc(OT::UnsignedInteger head, OT::UnsignedInteger tail);

  void setCMode(CorrectedMutualInformation::CModeTypes cmode);
  void setKMode(CorrectedMutualInformation::KModeTypes kmode);

  void setVerbosity(bool verbose);
  bool getVerbosity() const;

  void setAlpha(double alpha);
  double getAlpha() const;

private:
  void initiation();
  void iteration();
  void findBestContributor(const OT::UnsignedInteger X,
                           const OT::UnsignedInteger Y,
                           const OT::Indices &U);
  std::vector< std::tuple< std::tuple< OT::UnsignedInteger,
      OT::UnsignedInteger,
      OT::UnsignedInteger >*,
      double,
      double,
      double > >
      getUnshieldedTriples(const gum::MixedGraph& graph,
                           gum::HashTable< std::pair< OT::UnsignedInteger,
                           OT::UnsignedInteger >,
                           char >& marks);

  std::vector< std::tuple< std::tuple< OT::UnsignedInteger,
      OT::UnsignedInteger,
      OT::UnsignedInteger >*,
      double, double, double > >
      updateProbaTriples(
        const gum::MixedGraph& graph,
        std::vector< std::tuple< std::tuple< OT::UnsignedInteger,
        OT::UnsignedInteger,
        OT::UnsignedInteger >*,
        double, double, double > > proba_triples);

  void propagatesHead(gum::MixedGraph& graph, gum::NodeId node);

  bool existsDirectedPath(const gum::MixedGraph& graph,
                          const OT::UnsignedInteger n1,
                          const OT::UnsignedInteger n2,
                          const bool countArc = true) const;

  gum::MixedGraph UGtoMG(const gum::UndiGraph& graph) const;
  gum::NodeId idFromName(const std::string& name) const;
  std::vector< std::string > namesFromData() const;

  int maxLog_ = 100;
  bool verbose_ = false;

  bool skeleton_done_ = false;
  bool pdag_done_ = false;
  bool dag_done_ = false;

  CorrectedMutualInformation info_;

  gum::HashTable< std::pair< OT::UnsignedInteger,
      OT::UnsignedInteger >, char > initial_marks_;
  std::vector< gum::Arc > latent_couples_;
  gum::ArcProperty< double > arc_probas_;
  gum::EdgeProperty< OT::Indices > sepset_;
  gum::Heap <
  std::pair< std::tuple< OT::UnsignedInteger,
      OT::UnsignedInteger,
      OT::UnsignedInteger,
      OT::Indices >*, double >,
      GreaterPairOn2nd > rank_;

  gum::UndiGraph skeleton_;
  gum::MixedGraph pdag_;
  NamedDAG dag_;
};

}

#endif // OTAGRUM_CONTINUOUSMIIC_HXX
