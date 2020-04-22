#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/core/hashFunc.h>
#include <agrum/tools/core/heap.h>
#include <agrum/tools/graphs/undiGraph.h>

#include <openturns/EmpiricalBernsteinCopula.hxx>

#include "otagrum/ContinuousMIIC.hxx"

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

namespace OTAGRUM {

    ContinuousMIIC::ContinuousMIIC(const OT::Sample &data, const int maxLog)
        : OT::Object()
        , maxLog_(maxLog)
        , verbose_(false)
        , data_(data) {
        data_ = (data.rank() + 0.5) / data.getSize();  // Switching data to rank space
    }

    ContinuousMIIC::~ContinuousMIIC() { }

    //gum::UndiGraph learnSkeleton() {
        // clear the vector of latents arcs to be sure
        //__latent_couples.clear();
        //Heap<
            //std::pair< std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
                       //double >,
            //GreaterPairOn2nd >
            //_rank;

        //HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > > sep_set;

        //initiation(graph, sep_set, _rank);
        //iteration(graph, sep_set, _rank);
    //}

    // Get key associated to an OT::Indices in order to store it in cache
    std::string ContinuousMIIC::GetKey(OT::Indices l,
                                       const OT::UnsignedInteger K) const {
        if (!l.isIncreasing()) {            // In order to be independent
            std::sort(l.begin(), l.end());  // of the order of indices
        }
        return l.__str__() + ":" + std::to_string(K);
    }

    // Compute the (copula) entropy on variables
    double ContinuousMIIC::computeEntropy(const OT::Indices &variables,
                                          const int K) {
        if (variables.getSize() > 1) {  // If there is more than one variable
            const auto key = GetKey(variables, K);
            if (!Hcache_.exists(key)) {  // if H(variables) haven't been computed
                auto marginal_data = data_.getMarginal(variables);
                auto bc = OT::EmpiricalBernsteinCopula(marginal_data, K, true);
                double H = -bc.computeLogPDF(marginal_data).computeMean()[0];
                Hcache_.insert(key, H);
                return H;
            }
            else {
                return Hcache_[key];
            }
        }
        else { // Else entropy is null
            return 0.;
        }
    }

    void ContinuousMIIC::setVerbosity(bool verbose) {
        verbose_ = verbose;
    }

    bool ContinuousMIIC::getVerbosity() const {
        return verbose_;
    }

} // namespace OTAGRUM
