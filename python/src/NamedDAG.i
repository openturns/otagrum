// SWIG file NamedJunctionTree.i

%{
#include <agrum/graphs/DAG.h>
#include "otagrum/NamedDAG.hxx"
%}

%include NamedDAG_doc.i
%include otagrum/NamedDAG.hxx

namespace OTAGRUM { %extend NamedDAG { NamedDAG(const NamedDAG& other) { return new OTAGRUM::NamedDAG(other); } } }
