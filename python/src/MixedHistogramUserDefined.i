// SWIG file MixedHistogramUserDefined.i

%{
#include "otagr/MixedHistogramUserDefined.hxx"
%}

//%include MixedHistogramUserDefined_doc.i

%include otagr/MixedHistogramUserDefined.hxx
namespace OTAGR { %extend MixedHistogramUserDefined { MixedHistogramUserDefined(const MixedHistogramUserDefined & other) { return new OTAGR::MixedHistogramUserDefined(other); } } }
