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
  explicit TabuList(const OT::Sample &data, const NamedDAG &initial_dag,
                    const unsigned int max_parents = 4, const unsigned int restarts = 1,
                    const unsigned int tabu_list_size = 2);
  explicit TabuList(const OT::Sample &data,
                    const unsigned int max_parents = 4,
                    const unsigned int restarts = 1,
                    const unsigned int tabu_list_size = 2);

  //NamedDAG learnDAG();

  void setVerbosity(bool verbose);
  bool getVerbosity() const;

  void setCMode(CorrectedMutualInformation::CModeTypes cmode);
  //CorrectedMutualInformation::CModeTypes getCMode();

  unsigned int getMaxParents() const;
  unsigned int getRestarts() const;

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

  gum::DAG randomDAG(unsigned int size,
                     unsigned int max_parents,
                     unsigned int steps = 50);
  std::vector< gum::learning::GraphChange >
  findLegalChanges(const gum::DAG &dag, unsigned int max_parents);
  gum::learning::GraphChange
  choseRandomChange(const std::vector< gum::learning::GraphChange > &changes);

  std::pair< gum::learning::GraphChange, double > findBestChange(gum::DAG dag);
  void updateBest(const gum::DAG &dag, const double score);
  double tabuListAlgo(gum::DAG &dag, unsigned int max_parents);


  CorrectedMutualInformation info_;
  gum::DAG best_dag_;
  double best_score_;

  unsigned int max_parents_;
  unsigned int restarts_;
  unsigned int tabu_list_size_;

  bool verbose_ = false;
  bool dag_done_ = false;

  //gum::Queue queue_;
  gum::List< gum::learning::GraphChange > tabu_list_;
};

}

#endif // OTAGRUM_TABULIST_HXX
