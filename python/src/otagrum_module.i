// SWIG file otagrum_module.i

%module(docstring="otagr module") otagrum

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

%include otagrum_agrum.i


// The new classes
%include otagrum/otagrumprivate.hxx

%include Utils.i
%include NamedJunctionTree.i
%include NamedDAG.i
%include JunctionTreeBernsteinCopula.i
%include JunctionTreeBernsteinCopulaFactory.i
%include ContinuousTTest.i
%include ContinuousPC.i
%include CorrectedMutualInformation.i
%include ContinuousMIIC.i
%include TabuList.i
%include ContinuousBayesianNetwork.i
%include ContinuousBayesianNetworkFactory.i
