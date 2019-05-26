//                                               -*- C++ -*-
/**
 *  @brief Utils
 *
 *  Copyright 2010-2018 Airbus-LIP6-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef OTAGRUM_UTILS_HXX
#define OTAGRUM_UTILS_HXX

#include <openturns/Distribution.hxx>
#include <openturns/DistributionImplementation.hxx>
#include <openturns/PersistentObject.hxx>
#include <openturns/Point.hxx>
#include <openturns/StorageManager.hxx>
#include <openturns/UserDefined.hxx>

#include <agrum/multidim/potential.h>
#include <agrum/variables/discretizedVariable.h>

#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM {

class OTAGRUM_API Utils
{
public:

static gum::Potential<double> Discretize(const OT::DistributionImplementation &distribution,
                                         const gum::DiscretizedVariable<double> &v);

/* Helper function to discretize a continuous distribution */
static gum::Potential<double> Discretize(const OT::Distribution &distribution,
                                        const gum::DiscretizedVariable<double> &v,
                                        bool isTruncated = true);

static OT::Distribution FromMarginal(const gum::Potential<double> &pot);

static OT::Distribution FromPotential(const gum::Potential<double> &pot);

static OT::Indices FromNodeSet(const gum::NodeSet &clique);

private:
  Utils();
};

} /* namespace OTAGRUM */

#endif /* OTAGRUM_UTILS_HXX */
