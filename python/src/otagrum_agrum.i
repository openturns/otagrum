%{
#include <agrum/graphs/diGraph.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/graphs/cliqueGraph.h>

#include <agrum/graphs/graphElements.h>
#include <agrum/multidim/potential.h>

using namespace gum;
%}

%pythoncode %{
  import pyAgrum as gum
%}

// minimal insertion of aGrUM classes
%ignore *::operator=;
%ignore *::operator++;
#pragma SWIG nowarn=320,317,401

%import (module="pyAgrum") <agrum/core/set.h>

%import (module="pyAgrum") <agrum/graphs/graphElements.h>
%import (module="pyAgrum") <agrum/graphs/diGraph.h>
%import (module="pyAgrum") <agrum/graphs/undiGraph.h>
%import (module="pyAgrum") <agrum/graphs/mixedGraph.h>


%import <agrum/multidim/potential.h>

// needed for template instantiation
// get rid of swig messages :
// 'swig/python detected a memory leak of type 'gum::Potential< double > *', no destructor found.'
// (other way to do that ?)
%template ( Potential_double ) gum::Potential<double>;
%pythoncode %{
Potential = Potential_double
%}

// forcing the type from otagr.Potential (from above) to pyAgrum.Potential
%pythonappend OTAGRUM::Utils::Discretize %{
    #casting into gum namespace
    val=gum.Potential(val)
    val.__var=args[1]
%}
%pythonappend OTAGRUM::ContinuousPC::getSkeleton %{
    #casting into gum namespace
    val=gum.UndiGraph(val)
%}
%pythonappend OTAGRUM::ContinuousPC::getPDAG %{
    #casting into gum namespace
    val=gum.MixedGraph(val)
%}
