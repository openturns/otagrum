//                                               -*- C++ -*-
/**
 *  @file  OTAGRUMResourceMap.cxx
 *  @brief Implementation of the rational kernel
 *
 *  (C) Copyright 2005-2012 EDF-EADS-Phimeca
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  @author: $LastChangedBy: dutka $
 *  @date:   $LastChangedDate: 2008-05-21 11:21:38 +0200 (Wed, 21 May 2008) $
 *  Id:      $Id: OTAGRUMResourceMap.cxx 815 2008-05-21 09:21:38Z dutka $
 */

#include "OTAGRUMResourceMap.hxx"
#include "ResourceMap.hxx"
#include <unistd.h>
#include <string.h>
#include "OTthread.hxx"
#include "OTconfig.hxx"
#include "OSS.hxx"
#include "Exception.hxx"
#include "Log.hxx"

using namespace OT;

namespace OTOTAGRUM
{

CLASSNAMEINIT(OTAGRUMResourceMap);

static pthread_mutex_t OTAGRUMResourceMap_InstanceMutex_;

OTAGRUMResourceMap_init::OTAGRUMResourceMap_init()
{
  static pthread_once_t OTAGRUMResourceMap_InstanceMutex_once = PTHREAD_ONCE_INIT;
  int rc = pthread_once( &OTAGRUMResourceMap_InstanceMutex_once, OTAGRUMResourceMap::Initialize );
  if (rc != 0)
  {
    perror("OTAGRUMResourceMap_init::OTAGRUMResourceMap_init once Initialization failed");
    exit(1);
  }
}

void OTAGRUMResourceMap::Initialize()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init( &attr );
  pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
  int rc = pthread_mutex_init( &OTAGRUMResourceMap_InstanceMutex_, &attr );
  if (rc != 0)
  {
    perror("ResourceMap::Initialize mutex initialization failed");
    exit(1);
  }
  ResourceMap::SetAsUnsignedInteger("LibOTAGRUM-DegreePolynomialKernel", 3);
  ResourceMap::SetAsNumericalScalar("LibOTAGRUM-ConstantPolynomialKernel", 0);
  ResourceMap::SetAsUnsignedInteger("LibOTAGRUM-CacheSize", 100);
  ResourceMap::SetAsNumericalScalar("LibOTAGRUM-Epsilon", 1e-3);
  ResourceMap::SetAsUnsignedInteger("LibOTAGRUMRegression-NumberOfFolds", 3);
  ResourceMap::SetAsUnsignedInteger("LibOTAGRUM-Shrinking", 1);


}


}

