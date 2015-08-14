#                                               -*- Makefile -*-
#
#  Makefile.am
#
#  (C) Copyright 2005-2007 EDF-EADS-Phimeca
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
#  @date:   $LastChangedDate: 2008-12-18 14:14:36 +0100 (Thu, 18 Dec 2008) $
#  Id:      $Id: Makefile.am 1046 2008-12-18 13:14:36Z dutka $
#
include $(top_srcdir)/config/common.am

# When rebuilding, tell aclocal where to find additional macros
ACLOCAL_AMFLAGS = -I m4

SUFFIXES = .at
AUTOTEST = autom4te --language=autotest
.at:
	$(AUTOTEST) -I $(top_srcdir)/config -I $(srcdir) $@.at -o $@.tmp
	mv $@.tmp $@

CLEANFILES += *.log check_testsuite
DISTCLEANFILES = atconfig
clean-local:
	$(RM) -rf *.dir

check_PROGRAMS  = t_BayesNetAgrum_std
t_BayesNetAgrum_std_SOURCES  = t_BayesNetAgrum_std.cxx
t_BayesNetAgrum_std_CPPFLAGS = -I$(top_srcdir)/src $(OPENTURNS_CPPFLAGS) $(agrum_include_path)
t_BayesNetAgrum_std_LDADD = $(top_builddir)/src/libOTBayesNetAgrum.la $(OPENTURNS_LDFLAGS) $(agrum_ldflags)

CHECKTESTSUITE = $(srcdir)/check_testsuite
$(CHECKTESTSUITE): $(srcdir)/check_testsuite.at
EXTRA_DIST     = check_testsuite.at check_testsuite
check-local: atconfig atlocal $(CHECKTESTSUITE)
	$(SHELL) $(CHECKTESTSUITE)

