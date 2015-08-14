#                                               -*- Autoconf -*-
#
#  ot_check_bibtex.m4
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
#  @author: $LastChangedBy: dutfoy $
#  @date:   $LastChangedDate: 2009-06-30 16:14:10 +0200 (mar. 30 juin 2009) $
#  Id:      $Id: ot_check_bibtex.m4 1206 2009-06-30 14:14:10Z dutfoy $
#
#
#  This file is intended to be include in the configure.in file
#  of Open TURNS project to check whether Bc is available on the
#  build platform.
#
# OT_CHECK_BIBTEX
# -----------
#
AC_DEFUN([OT_CHECK_BIBTEX],
[
  WITH_BIBTEX=0
  AC_ARG_VAR([BIBTEX], [path for bibtex tool])
  AC_PATH_PROG([BIBTEX], [bibtex])
  test x$BIBTEX = x || WITH_BIBTEX=1

  # Propagate test into atlocal
  AC_SUBST(WITH_BIBTEX)

  # Propagate test into Makefiles
  AM_CONDITIONAL(WITH_BIBTEX, test $WITH_BIBTEX = 1)

  # Propagate bibtex into source files
  AC_DEFINE_UNQUOTED([BIBTEX_EXECUTABLE_NAME], "${BIBTEX}", [The name of the bibtex executable])
])
