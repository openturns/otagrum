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
// The %import directives below only pull in partial/forward declarations of
// aGrUM types that are actually wrapped by the separate pyagrum SWIG module
// (see the runtime type-table sharing mechanism documented on otagrum_agrum.i
// as a whole). This partial visibility is expected to trigger, for the
// affected aGrUM headers:
//   317 - "Specialization of non-template" (template specializations SWIG
//         cannot resolve because it only sees a forward declaration here)
//   320 - "Explicit template instantiation ignored" (same root cause)
//   401 - "Nothing known about base class ... Ignored." (base classes defined
//         in headers we deliberately do not %import, since pyagrum owns them)
// These are harmless given the %import-only strategy; a *new* warning code
// appearing here would need investigation, not blanket suppression.
#pragma SWIG nowarn=320,317,401

%import (module="pyagrum") <agrum/base/core/set.h>
%import (module="pyagrum") <agrum/base/core/types.h>
%import (module="pyagrum") <agrum/base/graphs/graphElements.h>
%import (module="pyagrum") <agrum/base/graphs/diGraph.h>
%import (module="pyagrum") <agrum/base/graphs/undiGraph.h>
%import (module="pyagrum") <agrum/base/graphs/mixedGraph.h>


