// SWIG file ContinuousBayesianNetworkFactory.i

%{
#include "otagrum/ContinuousBayesianNetworkFactory.hxx"
#include <openturns/PythonWrappingFunctions.hxx>

namespace OT {
  template <>
  struct traitsPythonType< OT::DistributionFactory >
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::DistributionFactory >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__DistributionFactory, 0 ))) {
      OT::DistributionFactory * p_it = reinterpret_cast< OT::DistributionFactory * >( ptr );
      return p_it != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__DistributionFactoryImplementation, 0 ))) {
      OT::DistributionFactoryImplementation * p_impl = reinterpret_cast< OT::DistributionFactoryImplementation * >( ptr );
      return p_impl != NULL;
    }
    return false;
  }

  template <>
  inline
  OT::DistributionFactory
  convert< _PyObject_, OT::DistributionFactory >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__DistributionFactory, 0))) {
      OT::DistributionFactory * p_mf = reinterpret_cast< OT::DistributionFactory * >(ptr);
      return *p_mf;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__DistributionFactoryImplementation, 0 ))) {
        OT::DistributionFactoryImplementation * p_impl = reinterpret_cast< OT::DistributionFactoryImplementation * >( ptr );
        return *p_impl;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a DistributionFactory";
    }
    return OT::DistributionFactory();
  }

} // OT
%}

%include ContinuousBayesianNetworkFactory_doc.i

%copyctor OTAGRUM::ContinuousBayesianNetworkFactory;
%include otagrum/ContinuousBayesianNetworkFactory.hxx
