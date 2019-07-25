//                                               -*- C++ -*-
/**
 *  @brief OTAgrum_ResourceMap
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
#ifndef OTAGRUM_RESOURCEMAP
#define OTAGRUM_RESOURCEMAP

#include "otagrum/otagrumprivate.hxx"
#include <openturns/OT.hxx>

namespace OTAGRUM {

class OTAGRUM_API OTAgrumResourceMap {
  CLASSNAME;

public:
  static void Initialize();
};

struct OTAGRUM_API OTAgrumResourceMap_init {
  OTAgrumResourceMap_init();
};
static OTAgrumResourceMap_init __OTAgrumResourceMap_initializer;

} // namespace OTAGRUM
#endif
