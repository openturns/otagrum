#include <agrum/tools/core/list.h>

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

//void ContinuousBIC::setCMode(CorrectedMutualInformation::CModeTypes cmode)
//{
  //info_.setCMode(cmode);
//}

void ContinuousBIC::setVerbosity(bool verbose)
{
  verbose_ = verbose;
}

bool ContinuousBIC::getVerbosity() const
{
  return verbose_;
}

//void ContinuousBIC::addForbiddenArc(gum::Arc arc)
//{
  //addForbiddenArc(arc.head(), arc.tail());
//}

//void ContinuousBIC::addForbiddenArc(std::string head, std::string tail)
//{
  //addForbiddenArc(idFromName(head), idFromName(tail));
//}

//void ContinuousBIC::addForbiddenArc(OT::UnsignedInteger head, OT::UnsignedInteger tail)
//{
  //initial_marks_.insert({head, tail}, '-');
//}

//void ContinuousBIC::addMandatoryArc(gum::Arc arc)
//{
  //addMandatoryArc(arc.head(), arc.tail());
//}

//void ContinuousBIC::addMandatoryArc(std::string head, std::string tail)
//{
  //addMandatoryArc(idFromName(head), idFromName(tail));
//}

//void ContinuousBIC::addMandatoryArc(OT::UnsignedInteger head, OT::UnsignedInteger tail)
//{
  //initial_marks_.insert({head, tail}, '>');
//}

NamedDAG ContinuousBIC::learnDAG()
{
  TRACE("\n===== STARTING DAG LEARNING =====" << std::endl);

  gum::DAG dag;

  dag_ = NamedDAG(dag, namesFromData());


  TRACE("===== ENDING DAG LEARNING =====" << std::endl);
  return dag_;
}

double ContinuousBIC::deltaScore(newDAG, oldDAG) {
    delta = 0.;
    return delta;
}

//gum::NodeId ContinuousBIC::idFromName(const std::string& name) const
//{
  //const auto &description = info_.getDataDescription();
  //for (OT::UnsignedInteger i = 0; i < description.getSize(); i++)
  //{
    //if (name == description.at(i))
    //{
      //return gum::NodeId(i);
    //}
  //}

  //throw OT::InvalidArgumentException(HERE) << "Error: name '" << name
      //<< "' is not a node name.";
//}

//std::vector< std::string > ContinuousBIC::namesFromData() const
//{
  //std::vector< std::string > names;
  //const auto &description = info_.getDataDescription();
  //for (OT::UnsignedInteger i = 0; i < description.getSize(); i++)
  //{
    //names.push_back(description.at(i));
  //}
  //return names;
//}

} // namespace OTAGRUM
