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

%import (module="pyAgrum") <agrum/core/types.h>
%import (module="pyAgrum") <agrum/graphs/graphElements.h>
%import (module="pyAgrum") <agrum/graphs/diGraph.h>
%import (module="pyAgrum") <agrum/graphs/undiGraph.h>
%import (module="pyAgrum") <agrum/graphs/mixedGraph.h>


