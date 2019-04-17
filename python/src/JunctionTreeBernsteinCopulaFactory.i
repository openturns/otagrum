// SWIG file JunctionTreeBernsteinCopulaFactory.i

%{
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/mixedGraph.h>
#include "otagrum/JunctionTreeBernsteinCopulaFactory.hxx"
%}

%include JunctionTreeBernsteinCopulaFactory_doc.i

%include otagrum/JunctionTreeBernsteinCopulaFactory.hxx
namespace OTAGRUM { %extend JunctionTreeBernsteinCopulaFactory { JunctionTreeBernsteinCopulaFactory(const JunctionTreeBernsteinCopulaFactory & other) { return new OTAGRUM::JunctionTreeBernsteinCopulaFactory(other); } } }
