// SWIG file Utils.i

%{
#include <agrum/tools/graphs/undiGraph.h>
#include <agrum/tools/graphs/mixedGraph.h>
#include "otagrum/Utils.hxx"
%}

%include Utils_doc.i

%nodefaultctor Utils;

%ignore OTAGRUM::Utils::FromNodeSet;
%ignore OTAGRUM::Utils::FastPow2;
%ignore OTAGRUM::Utils::FasterPow2;
%ignore OTAGRUM::Utils::FastExp;
%ignore OTAGRUM::Utils::FasterExp;

%include otagrum/Utils.hxx
