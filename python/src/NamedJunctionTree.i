// SWIG file NamedJunctionTree.i


%template(IndicesList) OT::Collection<OT::Indices>;

%{
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

%copyctor OTAGRUM::NamedJunctionTree;
%include otagrum/NamedJunctionTree.hxx
