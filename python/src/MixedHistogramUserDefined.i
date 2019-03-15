// SWIG file MixedHistogramUserDefined.i

%{
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/mixedGraph.h>
#include "otagrum/MixedHistogramUserDefined.hxx"
%}

%include MixedHistogramUserDefined_doc.i


%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OTAGRUM::MixedHistogramUserDefined::PointCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) || OT::isAPythonSequenceOf<OT::_PySequence_>($input);
}


%typemap(in) const OTAGRUM::MixedHistogramUserDefined::PointCollection & {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    try {
      $1 = OT::buildCollectionFromPySequence< OT::Point >($input);
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Point");
    }
  }
}


%include otagrum/MixedHistogramUserDefined.hxx
namespace OTAGRUM { %extend MixedHistogramUserDefined { MixedHistogramUserDefined(const MixedHistogramUserDefined & other) { return new OTAGRUM::MixedHistogramUserDefined(other); } } }
