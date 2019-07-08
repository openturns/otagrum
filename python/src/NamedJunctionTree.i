// SWIG file NamedJunctionTree.i

%{
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/mixedGraph.h>
#include "otagrum/NamedJunctionTree.hxx"
%}

%include openturns/swig/BaseTypCollection.i
%include NamedJunctionTree_doc.i
%include otagrum/NamedJunctionTree.hxx

namespace OTAGRUM { %extend NamedJunctionTree { NamedJunctionTree(const NamedJunctionTree& other) { return new OTAGRUM::NamedJunctionTree(other); } } }
