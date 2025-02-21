%{
#include <agrum/base/graphs/diGraph.h>
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/undiGraph.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/graphs/cliqueGraph.h>

#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/multidim/tensor.h>

using namespace gum;
%}

%pythoncode %{
  import pyagrum as gum
%}

// minimal insertion of aGrUM classes
%ignore *::operator=;
%ignore *::operator++;
#pragma SWIG nowarn=320,317,401

%import (module="pyagrum") <agrum/base/core/set.h>
%import (module="pyagrum") <agrum/base/core/types.h>
%import (module="pyagrum") <agrum/base/graphs/graphElements.h>
%import (module="pyagrum") <agrum/base/graphs/diGraph.h>
%import (module="pyagrum") <agrum/base/graphs/undiGraph.h>
%import (module="pyagrum") <agrum/base/graphs/mixedGraph.h>


