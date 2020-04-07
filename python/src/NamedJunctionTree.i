// SWIG file NamedJunctionTree.i


%template(IndicesList) OT::Collection<OT::Indices>;

%{
#include <agrum/tools/graphs/undiGraph.h>
#include <agrum/tools/graphs/mixedGraph.h>
#include "otagrum/NamedJunctionTree.hxx"

#include "openturns/PythonWrappingFunctions.hxx"

namespace OT {
  template <>
  inline
  bool
  canConvert< _PySequence_, OT::Indices >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Indices, 0))) {
      OT::Indices * p_it = reinterpret_cast< OT::Indices * >(ptr);
      return p_it != NULL;
    }
    return false;
  }

} // OT
%}

%include NamedJunctionTree_doc.i
%include otagrum/NamedJunctionTree.hxx

namespace OTAGRUM { %extend NamedJunctionTree { NamedJunctionTree(const NamedJunctionTree& other) { return new OTAGRUM::NamedJunctionTree(other); } } }
