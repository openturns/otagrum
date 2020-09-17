#ifndef OTAGRUM_TABULIST_HXX
#define OTAGRUM_TABULIST_HXX

#include <agrum/tools/graphs/DAG.h>
#include <agrum/tools/core/list.h>
#include <agrum/BN/learning/structureUtils/graphChange.h>

#include <openturns/Sample.hxx>

#include "otagrum/NamedDAG.hxx"
#include "otagrum/CorrectedMutualInformation.hxx"
#include "otagrum/IndicesManip.hxx"

namespace OTAGRUM
{

class OTAGRUM_API TabuList : public OT::Object
{
public:
  // Constructor
  TabuList(const OT::Sample &data,
           const NamedDAG &initial_dag,
           const OT::UnsignedInteger max_parents = 4,
           const OT::UnsignedInteger restarts = 1,
           const OT::UnsignedInteger tabu_list_size = 2);
  explicit TabuList(const OT::Sample &data,
                    const OT::UnsignedInteger max_parents = 4,
                    const OT::UnsignedInteger restarts = 1,
                    const OT::UnsignedInteger tabu_list_size = 2);

  //NamedDAG learnDAG();

  void setVerbosity(bool verbose);
  bool getVerbosity() const;

  void setCMode(CorrectedMutualInformation::CModeTypes cmode);
  //CorrectedMutualInformation::CModeTypes getCMode();

  OT::UnsignedInteger getMaxParents() const;
  OT::UnsignedInteger getRestarts() const;

  double getBestScore() const;

  NamedDAG learnDAG();

private:
  //void tabu_list_();
  std::vector< std::string > namesFromData() const;
  double computeDeltaScore(gum::DAG dag, gum::learning::GraphChange change);
  double computeDeltaScoreAddition(gum::DAG dag,
                                   OT::UnsignedInteger node1,
                                   OT::UnsignedInteger node2);
  double computeDeltaScoreDeletion(gum::DAG dag,
                                   OT::UnsignedInteger node1,
                                   OT::UnsignedInteger node2);
  double computeDeltaScoreReversal(gum::DAG dag,
                                   OT::UnsignedInteger node1,
                                   OT::UnsignedInteger node2);
  double computeScore(gum::DAG dag);
  void addElementTabuList(gum::learning::GraphChange change);

  gum::DAG randomDAG(OT::UnsignedInteger size,
                     OT::UnsignedInteger max_parents,
                     OT::UnsignedInteger steps = 50);
  std::vector< gum::learning::GraphChange >
  findLegalChanges(const gum::DAG &dag, OT::UnsignedInteger max_parents);
  gum::learning::GraphChange
  choseRandomChange(const std::vector< gum::learning::GraphChange > &changes);

  std::pair< gum::learning::GraphChange, double > findBestChange(gum::DAG dag);
  void updateBest(const gum::DAG &dag, const double score);
  double tabuListAlgo(gum::DAG &dag);


  CorrectedMutualInformation info_;
  gum::DAG best_dag_;
  double best_score_;

  OT::UnsignedInteger max_parents_;
  OT::UnsignedInteger restarts_;
  OT::UnsignedInteger tabu_list_size_;

  bool verbose_ = false;
  bool dag_done_ = false;

  //gum::Queue queue_;
  gum::List< gum::learning::GraphChange > tabu_list_;
};

}

#endif // OTAGRUM_TABULIST_HXX
