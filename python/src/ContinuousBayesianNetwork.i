// SWIG file ContinuousBayesanNetwork.i

%{
#include <openturns/OT.hxx>
#include "otagrum/ContinuousBayesianNetwork.hxx"
%}

%include ContinuousBayesianNetwork_doc.i
%include otagrum/ContinuousBayesianNetwork.hxx

%template(DistributionCollection) OT::Collection<OT::Distribution>;

namespace OTAGRUM { %extend ContinuousBayesianNetwork { ContinuousBayesianNetwork(const ContinuousBayesianNetwork& other) { return new OTAGRUM::ContinuousBayesianNetwork(other); } } }
