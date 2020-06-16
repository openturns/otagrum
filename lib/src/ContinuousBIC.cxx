#include <agrum/BN/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/BN/learning/structureUtils/graphChange.h>

#include <agrum/BN/learning/constraints/structuralConstraintSetStatic.h>
#include <agrum/BN/learning/constraints/structuralConstraintTabuList.h>
#include <agrum/BN/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/BN/learning/constraints/structuralConstraintDAG.h>
#include <agrum/BN/learning/constraints/structuralConstraintForbiddenArcs.h>
#include <agrum/BN/learning/constraints/structuralConstraintMandatoryArcs.h>


#include "otagrum/ContinuousBIC.hxx"

#define TRACE(x)                                                               \
  {                                                                            \
    if (verbose_)                                                              \
      std::cout << x;                                                          \
  }

namespace OTAGRUM
{

ContinuousBIC::ContinuousBIC(const OT::Sample &data)
  : OT::Object()
  , info_(data)
{
}

void ContinuousBIC::setVerbosity(bool verbose)
{
  verbose_ = verbose;
}

bool ContinuousBIC::getVerbosity() const
{
  return verbose_;
}

//NamedDAG ContinuousBIC::learnDAG()
//{
  //TRACE("\n===== STARTING DAG LEARNING =====" << std::endl);

  //gum::DAG dag;

  //dag_ = NamedDAG(dag, namesFromData());


  //TRACE("===== ENDING DAG LEARNING =====" << std::endl);
  //return dag_;
//}

double ContinuousBIC::deltaScore(gum::DAG dag) {
    double delta = 0.;
    gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintTabuList,
                                                  gum::learning::StructuralConstraintDAG >
        gen_constraint;
    gum::learning::GraphChangesGenerator4DiGraph< decltype(gen_constraint) > changeGenerator(
        gen_constraint);

    changeGenerator.setGraph(dag);

    std::cout << "DAG: " << dag << std::endl;

    for(auto it=changeGenerator.begin(); it!=changeGenerator.end(); ++it) {
        std::cout << "Change: " << *it << std::endl;
    }

    /*
    auto it = changeGenerator.begin();
    ++it;
    ++it;

    std::cout << "Chang: " << *it << std::endl;

    auto change = *it;
    
    changeGenerator.modifyGraph(change);

    for(auto it=changeGenerator.begin(); it!=changeGenerator.end(); ++it) {
        std::cout << "Change: " << *it << std::endl;
    }
    */

    
    return delta;
}

std::vector< std::string > ContinuousBIC::namesFromData() const
{
  std::vector< std::string > names;
  const auto &description = info_.getDataDescription();
  for (OT::UnsignedInteger i = 0; i < description.getSize(); i++)
  {
    names.push_back(description.at(i));
  }
  return names;
}

} // namespace OTAGRUM
