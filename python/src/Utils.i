// SWIG file Utils.i

%{
#include "otagrum/Utils.hxx"
%}

%include Utils_doc.i

%nodefaultctor Utils;

%ignore OTAGRUM::Utils::FromNodeSet;

%include otagrum/Utils.hxx
