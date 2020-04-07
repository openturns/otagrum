%{
#include <agrum/tools/graphs/diGraph.h>
#include <agrum/tools/graphs/DAG.h>
#include <agrum/tools/graphs/undiGraph.h>
#include <agrum/tools/graphs/mixedGraph.h>
#include <agrum/tools/graphs/cliqueGraph.h>

#include <agrum/tools/graphs/graphElements.h>
#include <agrum/tools/multidim/potential.h>

using namespace gum;
%}

%pythoncode %{
  import pyAgrum as gum
%}

// minimal insertion of aGrUM classes
%ignore *::operator=;
%ignore *::operator++;
#pragma SWIG nowarn=320,317,401

%import (module="pyAgrum") <agrum/tools/core/set.h>
%import (module="pyAgrum") <agrum/tools/core/types.h>
%import (module="pyAgrum") <agrum/tools/graphs/graphElements.h>
%import (module="pyAgrum") <agrum/tools/graphs/diGraph.h>
%import (module="pyAgrum") <agrum/tools/graphs/undiGraph.h>
%import (module="pyAgrum") <agrum/tools/graphs/mixedGraph.h>


