#ifndef OTAGRUM_CONTINUOUSMIIC_HXX
#define OTAGRUM_CONTINUOUSMICC_HXX

#include <agrum/tools/graphs/mixedGraph.h>

#include <openturns/Sample.hxx>

#include "otagrum/NamedDAG.hxx"

namespace OTAGRUM{

//class GreaterTupleOnLast {
    //public:
        //bool operator()(
                //const std::pair< std::tuple< NodeId, NodeId, NodeId >*, double >& e1,
                //const std::pair< std::tuple< NodeId, NodeId, NodeId >*, double >& e2)
                //const;
//};

class OTAGRUM_API ContinuousMIIC : public OT::Object {
    public:
        explicit ContinuousMIIC(const OT::Sample &data);
        //gum::UndiGraph learnSkeleton();
        //gum::MixedGraph learnPDAG();
        //NamedDAG learnDAG();

        //void setVerbosity(bool verbose);
        //bool getVerbosity() const;
            
    private:
        bool verbose_;

        OT::Sample data_;

        //gum::EdgeProperty<OT::Indices> sepset_;

        //gum::UndiGraph skeleton_;
        //gum::MixedGraph pdag_;
        //NamedDAG dag_;
};

}

#endif // OTAGRUM_CONTINUOUSMIIC_HXX
