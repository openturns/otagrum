#                                               -*- Autoconf -*-
#
#  ot_check_xercesc.m4
#
#  (C) Copyright 2005-2009 EDF-EADS-Phimeca
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  @author: $LastChangedBy: dutka $
#  @date:   $LastChangedDate: 2009-09-30 17:51:57 +0200 (mer. 30 sept. 2009) $
#  Id:      $Id: ot_check_xercesc.m4 1357 2009-09-30 15:51:57Z dutka $
#
#
#  This file is intended to be include in the configure.in file
#  of Open TURNS project to check whether XercesC is available on the
#  build platform.
#
# OT_CHECK_XERCESC( [XERCESC_INSTALL_PATH = /usr] )
# -------------------------------------------------
#
AC_DEFUN([OT_CHECK_XERCESC],
[
 AC_ARG_WITH([xercesc],
  AC_HELP_STRING([--with-xercesc@<:@=DIR@:>@], [add XML XercesC support. @<:@]m4_default([$1], /usr)[@:>@]),
                 [], [withval=yes])

  WITH_XERCESC=0

  # saving values for compilation variables
  saved_CPPFLAGS=$CPPFLAGS
  saved_LDFLAGS=$LDFLAGS
  saved_LIBS=$LIBS

  xercesc_default_path=m4_default([$1], /usr)
  if ! test x${withval} = xno
  then
    # ask for XercesC support
    AC_MSG_NOTICE([checking whether XercesC is here and working])

    # we're trying to find the correct XercesC installation path
    xercesc_install_path=$xercesc_default_path
    test x${withval} = xyes || xercesc_install_path=$withval

    AC_MSG_CHECKING([XercesC install path])
    test -d ${xercesc_install_path} || AC_MSG_ERROR([$xercesc_install_path: incorrect path])
    AC_MSG_RESULT([$xercesc_install_path])



    # we test the header file presence and usability
    xercesc_include_path=$xercesc_install_path/include
    CPPFLAGS="${CPPFLAGS} -I${xercesc_include_path}"
    AC_LANG_PUSH(C++)
    AC_CHECK_HEADERS([xercesc/util/XercesVersion.hpp xercesc/util/PlatformUtils.hpp xercesc/dom/DOM.hpp xercesc/framework/StdOutFormatTarget.hpp xercesc/parsers/XercesDOMParser.hpp],
                     [xercesc_header_found=yes],
                     [xercesc_header_found=no],
                     [])
    AC_LANG_POP(C++)
    test x${xercesc_header_found} = xno && AC_MSG_ERROR([XercesC include file NOT FOUND])
    AC_SUBST(xercesc_include_path)

    xercesc_version=undefined
    AC_MSG_CHECKING([XercesC version])
    AC_LANG_PUSH(C++)
    AC_LINK_IFELSE(AC_LANG_PROGRAM([#include <xercesc/util/XercesVersion.hpp>
                                    #include <iostream>
                                   ],
                                   [std::cout << INVK_CAT2_SEP_PERIOD(XERCES_VERSION_MAJOR,XERCES_VERSION_MINOR) << std::endl;]),
                   [test -x "conftest$ac_exeext" && ot_run="./conftest$ac_exeext" && xercesc_version=`eval "$ot_run" 2>&5`]
                  )
    AC_LANG_POP(C++)
    AC_MSG_RESULT($xercesc_version)
    AC_SUBST(xercesc_version)

    # we test the library file presence and usability
    xercesc_lib_path=$xercesc_install_path/lib
    xercesc_lib_name=xerces-c
    LDFLAGS="${LDFLAGS} -L${xercesc_lib_path}"
    LIBS="${LIBS} -l${xercesc_lib_name}"
    AC_LANG_PUSH(C++)
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <xercesc/util/PlatformUtils.hpp>]],
                                    [[XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize()]])
                   ],
                   [xercesc_lib_found=yes],
                   [xercesc_lib_found=no])
    AC_LANG_POP(C++)
    test x${xercesc_lib_found} = xno && AC_MSG_ERROR([XercesC library NOT FOUND])
    AC_SUBST(xercesc_lib_path)
    AC_SUBST(xercesc_lib_name)



    # after all tests are successful, we support XercesC
    WITH_XERCESC=1
    AC_MSG_NOTICE([XercesC support is OK])

  else
    # no XercesC support
    AC_MSG_NOTICE([No XercesC support])
  fi

  # Propagate test into atlocal
  AC_SUBST(WITH_XERCESC)

  # Propagate test into Makefiles...
  AM_CONDITIONAL(WITH_XERCESC, test $WITH_XERCESC = 1)

  # ... and into source files
  AC_DEFINE_UNQUOTED(HAS_XERCESC, $WITH_XERCESC, [Support for Xerces-C XML library])

  # restoring saved values
  CPPFLAGS=$saved_CPPFLAGS
  LDFLAGS=$saved_LDFLAGS
  LIBS=$saved_LIBS

])
