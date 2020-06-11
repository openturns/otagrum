#ifndef OTAGRUM_CONTINUOUSBIC_HXX
#define OTAGRUM_CONTINUOUSBIC_HXX

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

class OTAGRUM_API ContinuousBIC : public OT::Object
{
public:
  // Constructor
  explicit ContinuousBIC(const OT::Sample &data);

  NamedDAG learnDAG();

  //void addForbiddenArc(gum::Arc arc);
  //void addForbiddenArc(std::string head, std::string tail);
  //void addForbiddenArc(OT::UnsignedInteger head, OT::UnsignedInteger tail);

  //void addMandatoryArc(gum::Arc arc);
  //void addMandatoryArc(std::string head, std::string tail);
  //void addMandatoryArc(OT::UnsignedInteger head, OT::UnsignedInteger tail);

  //void setCMode(CorrectedMutualInformation::CModeTypes cmode);

  void setVerbosity(bool verbose);
  bool getVerbosity() const;

private:
  //gum::NodeId idFromName(const std::string& name) const;
  //std::vector< std::string > namesFromData() const;

  int maxLog_ = 100;
  bool verbose_ = false;

  gum::HashTable< std::pair< OT::UnsignedInteger,
      OT::UnsignedInteger >, char > initial_marks_;

  CorrectedMutualInformation info_;
  NamedDAG dag_;
};

}

#endif // OTAGRUM_CONTINUOUSBIC_HXX
