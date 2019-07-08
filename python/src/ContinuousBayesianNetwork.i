// SWIG file ContinuousBayesanNetwork.i

%{
#include "otagrum/ContinuousBayesianNetwork.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

namespace OT {
  template <>
  struct traitsPythonType< OT::Distribution >
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::Distribution >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__Distribution, 0 ))) {
      OT::Distribution * p_it = reinterpret_cast< OT::Distribution * >( ptr );
      return p_it != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__DistributionImplementation, 0 ))) {
      OT::DistributionImplementation * p_impl = reinterpret_cast< OT::DistributionImplementation * >( ptr );
      return p_impl != NULL;
    }
    return false;
  }

  template <>
  inline
  OT::Distribution
  convert< _PyObject_, OT::Distribution >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Distribution, 0))) {
      OT::Distribution * p_mf = reinterpret_cast< OT::Distribution * >(ptr);
      return *p_mf;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__DistributionImplementation, 0 ))) {
        OT::DistributionImplementation * p_impl = reinterpret_cast< OT::DistributionImplementation * >( ptr );
        return *p_impl;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Distribution";
    }
    return OT::Distribution();
  }

} // OT
%}

%include ContinuousBayesianNetwork_doc.i

%include otagrum/ContinuousBayesianNetwork.hxx

namespace OTAGRUM { %extend ContinuousBayesianNetwork { ContinuousBayesianNetwork(const ContinuousBayesianNetwork& other) { return new OTAGRUM::ContinuousBayesianNetwork(other); } } }
