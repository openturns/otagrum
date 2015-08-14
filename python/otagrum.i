// SWIG file openturnsotagrum.i
// Author : $LastChangedBy: dutka $
// Date : $LastChangedDate: 2008-08-28 17:36:47 +0200 (Thu, 28 Aug 2008) $
// Id : $Id: openturnsotagrum.i 916 2008-08-28 15:36:47Z dutka $

%module(docstring="Open TURNS BayesNetAgrum module is an interface to the aGrUM library that allows to manipulate Bayesian Networks in Open TURNS") otagrum
%feature("autodoc","1");

%{
#include "OT.hxx"
%}

// Prerequisites needed
%include typemaps.i
%include exception.i

%import aGrUM_wrap.i
%import pyAgrum.i
%import base_all.i
%import uncertainty_all.i

// The new classes
%include BayesNetAgrum.i
