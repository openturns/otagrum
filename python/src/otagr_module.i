// SWIG file otagr_module.i

%module(docstring="otagr module") otagr

%{
#include <openturns/OT.hxx>
#include <openturns/PythonWrappingFunctions.hxx>
%}

// Prerequisites needed
%include typemaps.i

%include exception.i

%ignore *::load(OT::Advocate & adv);
%ignore *::save(OT::Advocate & adv) const;

%import base_module.i
%import uncertainty_module.i

%include otagr_agrum.i


// The new classes
%include otagr/otagrprivate.hxx
%include Utils.i
%include MixedHistogramUserDefined.i
%include JunctionTree.i
%include ContinuousTTest.i
%include ContinuousPC.i
