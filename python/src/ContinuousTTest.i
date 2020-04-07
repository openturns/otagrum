// SWIG file ContinuousTTest.i

%{
#include <tuple>
#include <agrum/tools/graphs/undiGraph.h>
#include <agrum/tools/graphs/mixedGraph.h>

#include "otagrum/IndicesManip.hxx"
#include "otagrum/ContinuousTTest.hxx"
%}


%typemap(out) std::tuple<double, double, bool> {
  $result = Py_BuildValue("ddO",std::get<0>($1),
                                std::get<1>($1),
                                std::get<2>($1)?Py_True:Py_False);
}

%include ContinuousTTest_doc.i
%include "otagrum/ContinuousTTest.hxx"
