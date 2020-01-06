//                                               -*- C++ -*-
/**
 *  @brief OTAgrum_ResourceMap.cxx
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

#include <string.h>
#include <unistd.h>

#include <openturns/Exception.hxx>
#include <openturns/Log.hxx>
#include <openturns/OSS.hxx>
#include <openturns/OTconfig.hxx>
#include <openturns/OTthread.hxx>
#include <openturns/ResourceMap.hxx>

#include "otagrum/OTAgrumResourceMap.hxx"

using namespace OT;

namespace OTAGRUM
{

CLASSNAMEINIT(OTAgrumResourceMap);

static pthread_mutex_t OTAgrumResourceMap_InstanceMutex_;

OTAgrumResourceMap_init::OTAgrumResourceMap_init()
{
  static pthread_once_t OTAgrumResourceMap_InstanceMutex_once = PTHREAD_ONCE_INIT;
  int rc = pthread_once(&OTAgrumResourceMap_InstanceMutex_once,
                        OTAgrumResourceMap::Initialize);
  if (rc != 0)
  {
    perror("OTAgrumResourceMap_init::OTAgrumResourceMap_init once "
           "Initialization failed");
    exit(1);
  }
}

void OTAgrumResourceMap::Initialize()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  int rc = pthread_mutex_init(&OTAgrumResourceMap_InstanceMutex_, &attr);
  if (rc != 0)
  {
    perror("ResourceMap::Initialize mutex initialization failed");
    exit(1);
  }
  ResourceMap::SetAsScalar("JunctionTreeBernsteinCopulaFactory-DefaultAlpha", 0.1);
  ResourceMap::SetAsUnsignedInteger("JunctionTreeBernsteinCopulaFactory-DefaultBinNumber", 5);
  ResourceMap::SetAsUnsignedInteger("JunctionTreeBernsteinCopulaFactory-DefaultMaximumConditioningSetSize", 5);
}

} // namespace OTAGRUM
