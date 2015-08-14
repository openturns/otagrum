#                                               -*- Makefile -*-
#
#  Makefile.am
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
#  @date:   $LastChangedDate: 2010-05-07 22:40:30 +0200 (ven. 07 mai 2010) $
#  Id:      $Id: Makefile.am 1282 2010-05-07 20:40:30Z dutfoy $
#
include $(top_srcdir)/config/common.am

EXTRA_DIST = \
	Math_Notations.sty \
	OTAgrumDocumentation.sty \
	OTAgrum_ExamplesGuide.tex \
	OTAgrumDocumentation.tex \
	OTAgrum_ReferenceGuide.tex \
	OTAgrum_summary.tex \
	OTAgrum_title.tex \
	OTAgrum_UseCasesGuide.tex \
	OTAgrum_UserManual.tex


EXTRA_DIST += \
	logoOTAgrum.png \
	PlantGrowth.png \
	Height_PDF.png \
	Height_CDF.png \
	Height_PDF_WhenDim.png \
	Height_CDF_WhenDim.png \
	Height_PDF_WhenWet.png \
	Height_CDF_WhenWet.png


EXTRA_DIST += \
          script_Plant_Growth.py

OTAgrumDocumentation.pdf : $(EXTRA_DIST) $(top_builddir)/doc/GenericInformation.tex

pdf-local: OTAgrumDocumentation.pdf
html-am: OTAgrumDocumentation.html

doc_DATA  = OTAgrumDocumentation.pdf

