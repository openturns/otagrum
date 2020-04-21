#include "otagrum/ContinuousMIIC.hxx"

namespace OTAGRUM {
    ContinuousMIIC::ContinuousMIIC(const OT::Sample &data)
        : OT::Object()
        , verbose_(false)
        , data_(data) {
        data_ = (data.rank() + 0.5) / data.getSize();  // Switching data to rank space
    }

    void ContinuousMIIC::setVerbosity(bool verbose) {
        verbose_ = verbose;
    }

    bool ContinuousMIIC::getVerbosity() const {
        return verbose_;
    }

}

