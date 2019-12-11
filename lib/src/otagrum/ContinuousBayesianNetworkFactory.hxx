//                                               -*- C++ -*-
/**
 *  @brief ContinuousBayesianNetworkFactory
 *
 *  Copyright 2010-2019 Airbus-LIP6-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef OTAGRUM_CONTINUOUSBAYESIANNETWORKFACTORY_HXX
#define OTAGRUM_CONTINUOUSBAYESIANNETWORKFACTORY_HXX

#include <openturns/OT.hxx>
#include "otagrum/ContinuousBayesianNetwork.hxx"

namespace OTAGRUM
{

/**
 * @class ContinuousBayesianNetworkFactory
 */
class OTAGRUM_API ContinuousBayesianNetworkFactory
  : public OT::DistributionFactoryImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  ContinuousBayesianNetworkFactory();

  /** Parameter constructor */
  explicit ContinuousBayesianNetworkFactory(const OT::Collection< OT::DistributionFactory > & factories,
                                            const NamedDAG & namedDag = NamedDAG(),
                                            const OT::Scalar alpha = 0.1,
                                            const OT::UnsignedInteger maximumConditioningSetSize = 5);

  /** Virtual constructor */
  virtual ContinuousBayesianNetworkFactory *clone() const;

  using OT::DistributionFactoryImplementation::build;

  OT::Distribution build(const OT::Sample &sample) const;
  OT::Distribution build() const;

  ContinuousBayesianNetwork buildAsContinuousBayesianNetwork(const OT::Sample &sample) const;
  ContinuousBayesianNetwork buildAsContinuousBayesianNetwork() const;

private:
  /* Factories */
  OT::Collection< OT::DistributionFactory > factories_;

  /* Named DAG */
  NamedDAG namedDAG_;

  /* Threshold */
  OT::Scalar alpha_;

  /* Maximum conditional set size */
  OT::UnsignedInteger maximumConditioningSetSize_;

}; /* class ContinuousBayesianNetworkFactory */

} // namespace OTAGRUM

#endif // OTAGRUM_CONTINUOUSBAYESIANNETWORKFACTORY_HXX
