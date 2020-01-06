#                                               -*- cmake -*-
#
#  Useotagrum.cmake
#
#  Use otagrum in CMake files
#
#  Copyright 2010-2020 Airbus-LIP6-Phimeca
#
#  This library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this library.  If not, see <http://www.gnu.org/licenses/>.
#
#

add_definitions     ( ${OTAGRUM_DEFINITIONS} )
include_directories ( ${OTAGRUM_INCLUDE_DIRS} )
link_directories    ( ${OTAGRUM_LIBRARY_DIRS} )

