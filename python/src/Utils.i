// SWIG file Utils.i

%{
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/mixedGraph.h>
#include "otagrum/Utils.hxx"
%}

%include Utils_doc.i

%nodefaultctor Utils;

%ignore OTAGRUM::Utils::FromNodeSet;

%include otagrum/Utils.hxx
