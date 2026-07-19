//                                               -*- C++ -*-
/**
 *  @brief TabuList
 *
 *  Copyright 2010-2025 Airbus-LIP6-Phimeca
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

#include <agrum/base/graphs/algorithms/DAGCycleDetector.h>
#include <agrum/base/core/priorityQueue.h>

#include <openturns/Exception.hxx>
#include <openturns/RandomGenerator.hxx>

#include "otagrum/TabuList.hxx"
#include "otagrum/Utils.hxx"

#define TRACE(x)                                                               \
  {                                                                            \
    if (verbose_)                                                              \
      std::cout << x;                                                          \
  }

namespace OTAGRUM
{

TabuList::TabuList(const OT::Sample &data,
                   const NamedDAG &initial_dag,
                   const OT::UnsignedInteger max_parents,
                   const OT::UnsignedInteger restarts,
                   const OT::UnsignedInteger tabu_list_size)
  : OT::Object()
  , info_(data)
  , best_dag_(initial_dag.getDAG())
  , max_parents_(max_parents)
  , restarts_(restarts)
  , tabu_list_size_(tabu_list_size)
{
  if (restarts_ == 0)
    throw OT::InvalidArgumentException(HERE) << "Error: restarts must be >= 1, got 0.";
  info_.setKMode(CorrectedMutualInformation::KModeTypes::NoCorr);
  info_.setCMode(CorrectedMutualInformation::CModeTypes::Gaussian);
  best_score_ = computeScore(best_dag_); // Has to be done after KMode and CMode are set
  // Otherwise it uses default modes
}

TabuList::TabuList(const OT::Sample &data,
                   const OT::UnsignedInteger max_parents,
                   const OT::UnsignedInteger restarts,
                   const OT::UnsignedInteger tabu_list_size)
  : OT::Object()
  , info_(data)
  , max_parents_(max_parents)
  , restarts_(restarts)
  , tabu_list_size_(tabu_list_size)
{
  if (restarts_ == 0)
    throw OT::InvalidArgumentException(HERE) << "Error: restarts must be >= 1, got 0.";
  info_.setKMode(CorrectedMutualInformation::KModeTypes::NoCorr);
  info_.setCMode(CorrectedMutualInformation::CModeTypes::Gaussian);
  // Creating an empty dag (no arcs)
  for(gum::NodeId i = 0; i < data.getDimension(); ++i)
  {
    best_dag_.addNodeWithId(i);
  }
  best_score_ = computeScore(best_dag_);
}

void TabuList::setVerbosity(bool verbose)
{
  verbose_ = verbose;
}

bool TabuList::getVerbosity() const
{
  return verbose_;
}

void TabuList::setCMode(CorrectedMutualInformation::CModeTypes cmode)
{
  info_.setCMode(cmode);
}

OT::UnsignedInteger TabuList::getMaxParents() const
{
  return max_parents_;
}

OT::UnsignedInteger TabuList::getRestarts() const
{
  return restarts_;
}

NamedDAG TabuList::learnDAG()
{
  if(dag_done_)
  {
    return NamedDAG(best_dag_, namesFromData());
  }
  TRACE("\n===== STARTING DAG LEARNING =====" << std::endl);
  TRACE(" Initial DAG: " << best_dag_ << std::endl)
  TRACE(" Initial score: " << best_score_ << std::endl)

  TRACE("\n   == RUN 1 ==" << std::endl)
  gum::DAG dag = best_dag_;
  double score = tabuListAlgo(dag);
  TRACE("   Learned DAG: " << dag << std::endl)
  TRACE("   DAG score: " << score << std::endl)
  updateBest(dag, score);

  for(OT::UnsignedInteger i = 0; i < restarts_ - 1; ++i)
  {
    TRACE("\n   == RUN " << i + 2 << " ==" << std::endl)
    dag = randomDAG(info_.getDimension(), max_parents_);
    TRACE("   Initial DAG: " << dag << std::endl)
    score = tabuListAlgo(dag);
    TRACE("   Learned DAG: " << dag << std::endl)
    TRACE("   Score: " << score << std::endl)
    updateBest(dag, score);
  }

  dag_done_ = true;

  TRACE("\n Final DAG: " << best_dag_ << std::endl)
  TRACE(" Final score: " << best_score_ << std::endl)

  TRACE("\n===== ENDING DAG LEARNING =====" << std::endl);
  return NamedDAG(best_dag_, namesFromData());
}

double TabuList::tabuListAlgo(gum::DAG &dag)
{
  tabu_list_.clear();
  double score = computeScore(dag);
  bool converged = false;
  while(!converged)
  {
    auto best_change = findBestChange(dag);
    TRACE("   Best change: (" << best_change.first << ", "
          << best_change.second << ")" << std::endl)
    if(best_change.second > 0)
    {
      score += best_change.second;
      switch(best_change.first.type())
      {
        case gum::learning::GraphChangeType::ARC_ADDITION:
          dag.addArc(best_change.first.node1(),
                     best_change.first.node2());
          addElementTabuList(
            gum::learning::ArcDeletion(
                                       best_change.first.node1(),
                                       best_change.first.node2()));
          break;
        case gum::learning::GraphChangeType::ARC_DELETION:
          dag.eraseArc(gum::Arc(best_change.first.node1(),
                                best_change.first.node2()));
          addElementTabuList(
            gum::learning::ArcAddition(
                                       best_change.first.node1(),
                                       best_change.first.node2()));
          break;
        case gum::learning::GraphChangeType::ARC_REVERSAL:
          dag.eraseArc(gum::Arc(best_change.first.node1(),
                                best_change.first.node2()));
          dag.addArc(best_change.first.node2(),
                     best_change.first.node1());
          addElementTabuList(
            gum::learning::ArcReversal(
                                       best_change.first.node2(),
                                       best_change.first.node1()));
          break;
        case gum::learning::GraphChangeType::EDGE_ADDITION:
          break;
        case gum::learning::GraphChangeType::EDGE_DELETION:
          break;
        case gum::learning::GraphChangeType::ARC_TRIANGLE_DELETION1:
          break;
        case gum::learning::GraphChangeType::ARC_TRIANGLE_DELETION2:
          break;
          //default:
          //trhow error
      }
    }
    else
    {
      converged = true;
    }
    //TRACE("\n   Tabu List: " << tabu_list_ << std::endl)
  }
  return score;
}

void TabuList::updateBest(const gum::DAG &dag, const double score)
{
  if(score > best_score_)
  {
    best_score_ = score;
    best_dag_ = dag;
  }
}

gum::DAG TabuList::randomDAG(OT::UnsignedInteger size,
                             OT::UnsignedInteger max_parents,
                             OT::UnsignedInteger steps)
{
  gum::DAG dag;
  dag.addNodes(size);

  for(OT::UnsignedInteger i = 0; i < steps; ++i)
  {
    std::vector< gum::learning::GraphChange > legal_changes;
    legal_changes = findLegalChanges(dag, max_parents);
    if(!legal_changes.empty())
    {
      auto change = choseRandomChange(legal_changes);
      switch(change.type())
      {
        case gum::learning::GraphChangeType::ARC_ADDITION:
          dag.addArc(change.node1(), change.node2());
          break;
        case gum::learning::GraphChangeType::ARC_DELETION:
          dag.eraseArc(gum::Arc(change.node1(), change.node2()));
          break;
        case gum::learning::GraphChangeType::ARC_REVERSAL:
          dag.eraseArc(gum::Arc(change.node1(), change.node2()));
          dag.addArc(change.node2(), change.node1());
          break;
        case gum::learning::GraphChangeType::EDGE_ADDITION:
          break;
        case gum::learning::GraphChangeType::EDGE_DELETION:
          break;
        case gum::learning::GraphChangeType::ARC_TRIANGLE_DELETION1:
          break;
        case gum::learning::GraphChangeType::ARC_TRIANGLE_DELETION2:
          break;
          //default:
      }
    }
  }
  return dag;
}


std::vector< gum::learning::GraphChange > TabuList::legalChangesForPair(
  const gum::DAG &dag, const gum::DAGCycleDetector &dag_cycle_detector,
  gum::NodeId node1, gum::NodeId node2, OT::UnsignedInteger max_parents) const
{
  std::vector< gum::learning::GraphChange > changes;
  if(dag.existsArc(node1, node2))
  {
    if(!dag_cycle_detector.hasCycleFromReversal(node1, node2)
        && dag.parents(node1).size() < max_parents)
    {
      changes.push_back(gum::learning::ArcReversal(node1, node2));
    }
    changes.push_back(gum::learning::ArcDeletion(node1, node2));
  }
  else
  {
    if(!dag_cycle_detector.hasCycleFromAddition(node1, node2)
        && dag.parents(node2).size() < max_parents)
    {
      changes.push_back(gum::learning::ArcAddition(node1, node2));
    }
  }
  return changes;
}

std::vector< gum::learning::GraphChange > TabuList::findLegalChanges(
  const gum::DAG &dag, OT::UnsignedInteger max_parents)
{
  std::vector< gum::learning::GraphChange > changes;

  gum::DAGCycleDetector dag_cycle_detector;
  dag_cycle_detector.setDAG(dag);

  for(const auto node1 : dag.nodes())
  {
    for(const auto node2 : dag.nodes())
    {
      if(node1 != node2)
      {
        auto pair_changes = legalChangesForPair(dag, dag_cycle_detector, node1, node2, max_parents);
        changes.insert(changes.end(), pair_changes.begin(), pair_changes.end());
      }
    }
  }
  return changes;
}

gum::learning::GraphChange TabuList::choseRandomChange(
  const std::vector< gum::learning::GraphChange > &changes)
{
  return changes[OT::RandomGenerator::IntegerGenerate(changes.size())];
}

double TabuList::computeDeltaScore(const gum::DAG &dag, gum::learning::GraphChange change)
{
  //std::cout << "ComputeDeltaScore" << std::endl;
  switch(change.type())
  {
    case gum::learning::GraphChangeType::ARC_ADDITION:
      return computeDeltaScoreAddition(dag, change.node1(), change.node2());
      break;
    case gum::learning::GraphChangeType::ARC_DELETION:
      return computeDeltaScoreDeletion(dag, change.node1(), change.node2());
      break;
    case gum::learning::GraphChangeType::ARC_REVERSAL:
      return computeDeltaScoreReversal(dag, change.node1(), change.node2());
      break;
    case gum::learning::GraphChangeType::EDGE_DELETION:
      return 0.0;
      break;
    case gum::learning::GraphChangeType::EDGE_ADDITION:
      return 0.0;
      break;
    default:
      return 0.0;
  }
}

double TabuList::computeDeltaScoreAddition(const gum::DAG &dag,
    OT::UnsignedInteger node1,
    OT::UnsignedInteger node2)
{
  //std::cout << "\tComputeDeltaScoreAddition" << std::endl;
  auto parents = OT::Indices();
  for(const auto p : dag.parents(node2))
  {
    parents = parents + OT::UnsignedInteger(p);
  }
  auto Y = OT::Indices() + node2;
  double likelihood_part =
    info_.getSize() * (info_.compute2PtCorrectedInformation(Y, parents + node1)
                       - info_.compute2PtCorrectedInformation(Y, parents));
  double penalty_part = std::log(info_.getSize()) / 2;
  return likelihood_part - penalty_part;
}

double TabuList::computeDeltaScoreDeletion(const gum::DAG &dag,
    OT::UnsignedInteger node1,
    OT::UnsignedInteger node2)
{
  //std::cout << "\tComputeDeltaScoreDeletion" << std::endl;
  auto parents = OT::Indices();
  for(const auto p : dag.parents(node2))
  {
    parents = parents + OT::UnsignedInteger(p);
  }
  auto Y = OT::Indices() + node2;
  double likelihood_part =
    info_.getSize() * (info_.compute2PtCorrectedInformation(Y, parents - node1)
                       - info_.compute2PtCorrectedInformation(Y, parents));
  double penalty_part = - std::log(info_.getSize()) / 2;
  return likelihood_part - penalty_part;
}

double TabuList::computeDeltaScoreReversal(const gum::DAG &dag,
    OT::UnsignedInteger node1,
    OT::UnsignedInteger node2)
{
  //std::cout << "\tComputeDeltaScoreReversal" << std::endl;
  auto parentsX = OT::Indices();
  for(const auto p : dag.parents(node1))
  {
    parentsX = parentsX + OT::UnsignedInteger(p);
  }
  auto parentsY = OT::Indices();
  for(const auto p : dag.parents(node2))
  {
    parentsY = parentsY + OT::UnsignedInteger(p);
  }
  auto X = OT::Indices() + node1;
  auto Y = OT::Indices() + node2;

  double likelihood_part =
    info_.getSize() * (info_.compute2PtCorrectedInformation(X, parentsX + node2)
                       + info_.compute2PtCorrectedInformation(Y, parentsY - node1)
                       - info_.compute2PtCorrectedInformation(X, parentsX)
                       - info_.compute2PtCorrectedInformation(Y, parentsY));
  //double penalty_part = 0.; // No change in the penalty
  return likelihood_part;
}

double TabuList::computeScore(const gum::DAG &dag)
{
  double score = 0.;
  gum::DAG temp_dag;
  for(const auto node : dag.nodes())
  {
    temp_dag.addNodeWithId(node);
  }
  for(const auto & arc : dag.arcs())
  {
    score += computeDeltaScoreAddition(temp_dag, arc.tail(), arc.head());
    temp_dag.addArc(arc.tail(), arc.head());
  }
  return score;
}

double TabuList::getBestScore() const
{
  return best_score_;
}

std::vector< std::string > TabuList::namesFromData() const
{
  return Utils::NamesFromDescription(info_.getDataDescription());
}

std::pair< gum::learning::GraphChange, double >
TabuList::findBestChange(const gum::DAG &dag)
{
  gum::PriorityQueue< gum::learning::GraphChange, double, std::greater<double> >
  change_queue;
  gum::DAGCycleDetector dag_cycle_detector;
  dag_cycle_detector.setDAG(dag);
  for(const auto node1 : dag.nodes())
  {
    for(const auto node2 : dag.nodes())
    {
      if(node1 != node2)
      {
        for(const auto &change : legalChangesForPair(dag, dag_cycle_detector, node1, node2, max_parents_))
        {
          if(!tabu_list_.exists(change))
          {
            change_queue.insert(change, computeDeltaScore(dag, change));
          }
        }
      }
    }
  }

  if (change_queue.empty())
    return {gum::learning::ArcAddition( 0, 0), 0.0};
  return {change_queue.top(), change_queue.topPriority()};
}

void TabuList::addElementTabuList(gum::learning::GraphChange change)
{
  if(tabu_list_.size() == tabu_list_size_)
  {
    tabu_list_.popBack();
  }
  tabu_list_.pushFront(change);
}

} // namespace OTAGRUM
