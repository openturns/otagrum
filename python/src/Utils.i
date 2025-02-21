// SWIG file Utils.i

%{
#include <agrum/base/graphs/undiGraph.h>
#include <agrum/base/graphs/mixedGraph.h>
#include "otagrum/Utils.hxx"
%}

%include Utils_doc.i

%nodefaultctor Utils;

%ignore OTAGRUM::Utils::FromNodeSet;
%ignore OTAGRUM::Utils::FastPow2;
%ignore OTAGRUM::Utils::FasterPow2;
%ignore OTAGRUM::Utils::FastExp;
%ignore OTAGRUM::Utils::FasterExp;

%pythonprepend OTAGRUM::Utils::Discretize %{
        var = args[1]
%}
%pythonappend OTAGRUM::Utils::Discretize %{
        val._list_vars = [var]
%}

%include otagrum/Utils.hxx
