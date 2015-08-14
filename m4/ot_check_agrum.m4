#                                               -*- Autoconf -*-
#
#  ot_check_agrum.m4
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
#
#  This file is intended to be include in the configure.in file
#  of Open TURNS modules project to check whether Agrum is available on the
#  build platform.

AC_DEFUN([OT_CHECK_AGRUM],
[
 AC_ARG_WITH([agrum],
  AC_HELP_STRING([--with-agrum@<:@=DIR@:>@], [add aGrUM library. @<:@]m4_default([$1], /usr)[@:>@]),
                 [], [withval=yes])

  WITH_AGRUM=0

  # saving values for compilation variables
  saved_CPPFLAGS=$CPPFLAGS
  saved_LDFLAGS=$LDFLAGS
  saved_LIBS=$LIBS


  agrum_default_path=m4_default([$1], /usr)
  if ! test x${withval} = xno
  then
    # ask for Agrum support
    AC_MSG_NOTICE([checking whether aGrUM is here and working])



    # we're trying to find the correct Agrum installation path
    agrum_install_path=$agrum_default_path
    test x${withval} = xyes || agrum_install_path=$withval

    AC_MSG_CHECKING([aGrUM install path])
    test -d ${agrum_install_path} || AC_MSG_ERROR([$agrum_install_path: incorrect path])
    AC_MSG_RESULT([$agrum_install_path])

    # we test the header file presence and usability   
    agrum_include_path="-I$agrum_install_path/include"
    CPPFLAGS="${CPPFLAGS} ${agrum_include_path}"
    AC_LANG_PUSH(C++)
    AC_CHECK_HEADERS([agrum/BN/BayesNet.h agrum/BN/inference/lazyPropagation.h], 
                     [agrum_header_found=yes],
                     [agrum_header_found=no],
                     [])
    AC_LANG_POP(C++)
    test x${agrum_header_found} = xno && AC_MSG_ERROR([aGrUM include file NOT FOUND])
    AC_SUBST(agrum_include_path)


   # we test the library file presence and usability
    agrum_lib_path=$agrum_install_path/lib
    agrum_ldflags="-L${agrum_lib_path}"
    agrum_libs="-lagrum"
    LDFLAGS="${LDFLAGS} ${agrum_ldflags}"
    LIBS="${LIBS} ${agrum_libs}"
    AC_LANG_PUSH(C++)
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include "agrum/BN/BayesNet.h"]],
                                    [[gum::BayesNet<double> * bn = new gum::BayesNet<double>();]])
                   ],
                   [agrum_lib_found=yes],
                   [agrum_lib_found=no])
    AC_LANG_POP(C++)
    test x${agrum_lib_found} = xno && AC_MSG_ERROR([aGrUM library NOT FOUND])
    AC_SUBST(agrum_ldflags)
    AC_SUBST(agrum_libs)



    # after all tests are successful, we support Agrum
    WITH_AGRUM=1
    AC_MSG_NOTICE([aGrUM support is OK])

  else
    # no Agrum support
    AC_MSG_NOTICE([No aGrUM support])
  fi

  # Propagate test into atlocal
  AC_SUBST(WITH_AGRUM)

  # Propagate test into Makefiles...
  AM_CONDITIONAL(WITH_AGRUM, test $WITH_AGRUM = 1)

  # ... and into source files
  AC_DEFINE_UNQUOTED(HAS_AGRUM, $WITH_AGRUM, [Support for aGrUM library])

  # restoring saved values
  CPPFLAGS=$saved_CPPFLAGS
  LDFLAGS=$saved_LDFLAGS
  LIBS=$saved_LIBS

])
