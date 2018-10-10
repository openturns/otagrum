//                                               -*- C++ -*-
/**
 *  @brief OtAgr
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OTAGR_OTAGR_UTILS_HXX
#define OTAGR_OTAGR_UTILS_HXX

#include <openturns/Distribution.hxx>
#include <openturns/DistributionImplementation.hxx>
#include <openturns/PersistentObject.hxx>
#include <openturns/Point.hxx>
#include <openturns/StorageManager.hxx>
#include <openturns/UserDefined.hxx>

#include <agrum/multidim/potential.h>
#include <agrum/variables/discretizedVariable.h>

#include "otagr/otagrprivate.hxx"

namespace OTAGR {
gum::Potential<double>
discretize(const OT::DistributionImplementation &distribution,
           const gum::DiscretizedVariable<double> &v);

/* Helper function to discretize a continuous distribution */
gum::Potential<double> discretize(const OT::Distribution &distribution,
                                  const gum::DiscretizedVariable<double> &v,
                                  bool isTruncated = true);

OT::Distribution fromMarginal(const gum::Potential<double> &pot);

OT::Distribution fromPotential(const gum::Potential<double> &pot);

OT::Indices fromNodeSet(const gum::NodeSet &clique);

} /* namespace OTAGR */

#endif /* OTAGR_OTAGR_UTILS_HXX */
