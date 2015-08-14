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
#  @date:   $LastChangedDate: 2009-01-16 13:58:07 +0100 (Fri, 16 Jan 2009) $
#  Id:      $Id: Makefile.am 1068 2009-01-16 12:58:07Z dutka $
#
include $(top_srcdir)/config/common.am

# When rebuilding, tell aclocal where to find additional macros
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = extensions
DIST_SUBDIRS = extensions

AM_CPPFLAGS += -I$(top_srcdir)/src -I$(top_srcdir)/python/extensions
AM_CPPFLAGS += $(OPENTURNS_CPPFLAGS) $(agrum_include_path)
AM_CPPFLAGS += -DSWIG_TYPE_TABLE=openturns

BUILT_SOURCES             =
pkgpython_PYTHON          =
pkgpyexec_LTLIBRARIES     =
MAINTAINERCLEANFILES      = $(BUILT_SOURCES) otagrum.py
EXTRA_DIST                = otagrum.pth

python_DATA = otagrum.pth

## Python module : otagrum

BUILT_SOURCES            +=  otagrum_wrap.cpp
pkgpython_PYTHON         +=  otagrum.py
pkgpyexec_LTLIBRARIES    += _otagrum.la
EXTRA_DIST               += $(OPENTURNS_SWIG_SRC) otagrum_wrap.cpp.h

_otagrum_la_LDFLAGS      = -module -no-undefined
_otagrum_la_LDFLAGS     += $(OPENTURNS_LDFLAGS) $(agrum_ldflags)
_otagrum_la_LIBADD       = ../src/libOTBayesNetAgrum.la
_otagrum_la_LIBADD      += $(OPENTURNS_LIBS) $(agrum_libs)
_otagrum_la_CPPFLAGS     = $(SWIG_PYTHON_CPPFLAGS) $(agrum_include_path)
_otagrum_la_CPPFLAGS    += $(PYTHON_CPPFLAGS)
_otagrum_la_CPPFLAGS    += $(AM_CPPFLAGS)
_otagrum_la_CXXFLAGS     = $(AM_CXXFLAGS)
_otagrum_la_SOURCES      = otagrum_wrap.cpp $(OPENTURNS_SWIG_SRC)

pkgswig_DATA = $(OPENTURNS_SWIG_SRC)

OPENTURNS_SWIG_SRC   = \
	otagrum.i \
	BayesNetAgrum.i \
	pyAgrum.i \
	aGrUM_wrap.i

OPENTURNS_WRAPPER_DEPEND =
#OPENTURNS_WRAPPER_DEPEND = \
#	BayesNetAgrum.hxx

otagrum.py : otagrum_wrap.cpp
otagrum_wrap.cpp : $(OPENTURNS_SWIG_SRC) $(OPENTURNS_WRAPPER_DEPEND)
	@echo "*** INFO: This may take a while. Be patient..."
	(rm -f otagrum.py otagrum_wrap.cpp && \
	$(SWIG) $(SWIG_PYTHON_OPT) $(AM_CPPFLAGS) -I$(otswigdir) $(SWIGFLAGS) -o $@.tmp $< && \
	mv $@.tmp $@)

