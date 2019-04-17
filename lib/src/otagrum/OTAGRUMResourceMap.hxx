//                                               -*- C++ -*-
/**
 *  @file  SVMResourceMap.hxx
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
 *  Chih-Chung Chang and Chih-Jen Lin, LIBSVM : a library for support vector machines.
 *  ACM Transactions on  Intelligent Systems and Technology, 2:27:1--27:27, 2011. Software
 *  available at http://www.csie.ntu.edu.tw/~cjlin/libsvm
 *
 *  @author: $LastChangedBy: dutka $
 *  @date:   $LastChangedDate: 2008-05-21 11:21:38 +0200 (Wed, 21 May 2008) $
 *  Id:      $Id: SVMResourceMap.hxx 815 2008-05-21 09:21:38Z dutka $
 */
#ifndef OTSVM_RESOURCEMAP
#define OTSVM_RESOURCEMAP

#include "Object.hxx"
#include "OTSVMprivate.hxx"

namespace OTSVM
{

class OTSVM_API SVMResourceMap
{
  CLASSNAME;

public:

  static void Initialize();

protected:

private:

};


struct OTSVM_API SVMResourceMap_init
{
  SVMResourceMap_init();
};
static SVMResourceMap_init __SVMResourceMap_initializer;

}
#endif

