// SWIG file MixedHistogramUserDefined.i

%{
#include "otagrum/MixedHistogramUserDefined.hxx"
%}

//%include MixedHistogramUserDefined_doc.i

%include otagrum/MixedHistogramUserDefined.hxx
namespace OTAGRUM { %extend MixedHistogramUserDefined { MixedHistogramUserDefined(const MixedHistogramUserDefined & other) { return new OTAGRUM::MixedHistogramUserDefined(other); } } }
