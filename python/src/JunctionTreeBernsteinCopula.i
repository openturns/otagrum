// SWIG file JunctionTreeBernsteinCopula.i

%{
#include "otagrum/JunctionTreeBernsteinCopula.hxx"
%}

%include JunctionTreeBernsteinCopula_doc.i

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OTAGRUM::JunctionTreeBernsteinCopula::IndicesCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) || OT::isAPythonSequenceOf<OT::_PySequence_>($input);
}


%typemap(in) const OTAGRUM::JunctionTreeBernsteinCopula::IndicesCollection & {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    try {
      $1 = OT::buildCollectionFromPySequence< OT::Indices >($input);
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Indices");
    }
  }
}

%copyctor OTAGRUM::JunctionTreeBernsteinCopula;
%include otagrum/JunctionTreeBernsteinCopula.hxx
