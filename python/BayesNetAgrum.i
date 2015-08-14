// SWIG file BayesNetAgrum.i
// Author : $LastChangedBy: dutka $
// Date : $LastChangedDate: 2008-10-15 17:56:07 +0200 (Wed, 15 Oct 2008) $
// Id : $Id: BayesNetAgrum.i 972 2008-10-15 15:56:07Z dutka $

%{
#include "BayesNetAgrum.hxx"
%}

%include BayesNetAgrum.hxx

 //%template(VectorFloat) std::vector<float>;

namespace OpenTURNS { namespace Uncertainty { namespace Distribution { %extend BayesNetAgrum { BayesNetAgrum(const BayesNetAgrum & other) { return new OT::Uncertainty::Distribution::BayesNetAgrum::BayesNetAgrum(other); } } }}}

