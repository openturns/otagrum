cmake_minimum_required (VERSION 2.8.4)

option ( BUILD_PYTHON                 "Build the python module for the library"                               ON )
option (USE_SPHINX                    "Use sphinx for documentation"                                          ON)
option ( LINK_PYTHON_LIBRARY          "Link python modules against python library"                            ON )
option ( BUILD_SHARED_LIBS            "Build shared libraries"                                                ON )

set (CMAKE_CXX_STANDARD 14)


# By default, build in Release mode. Must appear before project() command
if ( NOT DEFINED CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

project ( otagr CXX )

string ( TOLOWER ${CMAKE_PROJECT_NAME} PACKAGE_NAME )

# set module dir to find custom scripts
list ( APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )


find_package(aGrUM)
if (aGrUM_FOUND)
  include_directories(${AGRUM_INCLUDE_DIR})
  link_directories(${AGRUM_LIB_DIR})
  message(STATUS "Found aGrUM: ${AGRUM_ROOTDIR} (found version \"${AGRUM_VERSION_STRING}\")")
else (aGrUM_FOUND)
  message(FATAL_ERROR "Please install aGrUM")
endif (aGrUM_FOUND)

find_package (OpenTURNS 1.11 NO_MODULE REQUIRED)
if (OPENTURNS_FOUND)
  message (STATUS "Found OpenTURNS: ${OPENTURNS_ROOT_DIR} (found version \"${OPENTURNS_VERSION_STRING}\")")
endif ()
include ( ${OPENTURNS_USE_FILE} )
set ( OTAGR_DEFINITIONS ${OPENTURNS_DEFINITIONS} )
if (NOT BUILD_SHARED_LIBS)
  list ( APPEND OTAGR_DEFINITIONS "-DOTAGR_STATIC" )
endif ()
add_definitions ( ${OTAGR_DEFINITIONS} )

set ( OTAGR_LIBRARY_PATH lib${LIB_SUFFIX} )
set ( OTAGR_INCLUDE_PATH include)
set ( OTAGR_SWIG_INCLUDE_PATH ${OTAGR_INCLUDE_PATH}/${PACKAGE_NAME}/swig )
set ( OTAGR_DATA_PATH share )
set ( OTAGR_EXAMPLE_PATH ${OTAGR_DATA_PATH}/${PACKAGE_NAME}/examples )
set ( OTAGR_CONFIG_CMAKE_PATH lib${LIB_SUFFIX}/cmake/otagr )
set ( OTAGR_DOC_PATH share/${PACKAGE_NAME}/doc)

if (BUILD_PYTHON)
  # workaround for cmake bug #0013449
  if (CMAKE_VERSION VERSION_LESS 3 AND DEFINED CMAKE_FIND_ROOT_PATH)
    find_program (SWIG_EXECUTABLE NAMES swig2.0 swig)
    if (SWIG_EXECUTABLE)
      set (SWIG_USE_FILE ${CMAKE_ROOT}/Modules/UseSWIG.cmake)
      set (SWIG_FOUND TRUE)
    endif ()
  else ()
    # follow agrum swig version requirement
    find_package (SWIG 3.0.11)
  endif ()
  if (SWIG_FOUND)
    include (${SWIG_USE_FILE})
  endif ()

  find_package ( PythonInterp )
  find_package ( PythonLibs )
  include (FindPythonModule)

  if (USE_SPHINX)
    find_program (SPHINX_EXECUTABLE NAMES sphinx-build DOC "Sphinx Documentation Builder (sphinx-doc.org)")
    if (SPHINX_EXECUTABLE)
      find_python_module (numpydoc)
      if (NUMPYDOC_FOUND)
        find_python_module (sphinx 1.1)
      endif ()
    endif ()
    if (NOT SPHINX_FOUND)
      message (STATUS "Disabling documentation")
    endif ()
  endif ()

  if ( PYTHONINTERP_FOUND )
    if (DEFINED PYTHON_SITE_PACKAGES)
      set (OTAGR_PYTHON_MODULE_PATH ${PYTHON_SITE_PACKAGES})

    else ()
      execute_process ( COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(plat_specific=True, prefix='${CMAKE_INSTALL_PREFIX}'))"
                        OUTPUT_VARIABLE _ABS_PYTHON_MODULE_PATH
                        RESULT_VARIABLE _PYTHON_pythonlib_result
                        OUTPUT_STRIP_TRAILING_WHITESPACE )

      if (_PYTHON_pythonlib_result)
        message (SEND_ERROR "Could not run ${PYTHON_EXECUTABLE}")
      endif ()

      get_filename_component (_ABS_PYTHON_MODULE_PATH ${_ABS_PYTHON_MODULE_PATH} ABSOLUTE)
      file (RELATIVE_PATH OTAGR_PYTHON_MODULE_PATH ${CMAKE_INSTALL_PREFIX} ${_ABS_PYTHON_MODULE_PATH})
    endif ()

    if (IS_ABSOLUTE ${OTAGR_PYTHON_MODULE_PATH})
      set (PYTHON_MODULE_PATH ${OTAGR_PYTHON_MODULE_PATH})
    else ()
      set (PYTHON_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/${OTAGR_PYTHON_MODULE_PATH})
    endif ()

    if (CMAKE_VERSION VERSION_LESS "2.8.6")
      execute_process (COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print(sys.version_info[0])"
                       OUTPUT_VARIABLE PYTHON_VERSION_MAJOR
                       OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif ()
    set (OTAGR_PYTHON${PYTHON_VERSION_MAJOR}_MODULE_PATH ${OTAGR_PYTHON_MODULE_PATH})
  endif ( PYTHONINTERP_FOUND )
endif ()


# Define variables without OTAGR_ prefix with absolute paths
foreach (_var PYTHON_MODULE_PATH LIBRARY_PATH INCLUDE_PATH CONFIG_CMAKE_PATH)
  if (IS_ABSOLUTE "${OTAGR_${_var}}")
    set (${_var} ${OTAGR_${_var}})
  else ()
    set (${_var} ${CMAKE_INSTALL_PREFIX}/${OTAGR_${_var}})
  endif ()
  get_filename_component (${_var} "${${_var}}" ABSOLUTE)
  file (TO_NATIVE_PATH "${${_var}}" NATIVE_${_var})
  string (REPLACE "\\" "\\\\" NATIVE_${_var} ${NATIVE_${_var}})
endforeach ()

# RPATH settings
set (CMAKE_INSTALL_RPATH ${LIBRARY_PATH})
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
if(POLICY CMP0042)
  # Set MACOSX_RPATH to ON
  cmake_policy(SET CMP0042 NEW)
endif()

# Some useful macros to ease CMakeLists.txt file writing
set ( SOURCEFILES "" CACHE INTERNAL "List of source files to compile" )
macro ( ot_add_source_file FILENAME )
  set ( sf ${SOURCEFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME} )
  set ( SOURCEFILES ${sf} CACHE INTERNAL "List of source files to compile" )
endmacro ( ot_add_source_file )

set ( HEADERFILES "" CACHE INTERNAL "List of header files to install" )
macro ( ot_install_header_file FILENAME )
  set ( hf ${HEADERFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}/${FILENAME} )
  set ( HEADERFILES ${hf} CACHE INTERNAL "List of header files to install" )
endmacro ( ot_install_header_file )

set ( SWIGFILES "" CACHE INTERNAL "List of SWIG files to install" )
macro ( ot_install_swig_file FILENAME )
  set ( hf ${SWIGFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME} )
  set ( SWIGFILES ${hf} CACHE INTERNAL "List of SWIG files to install" )
endmacro ( ot_install_swig_file )

set ( INTERNAL_INCLUDE_DIRS "" CACHE INTERNAL "List of directories with header files needed for build" )
macro ( ot_add_current_dir_to_include_dirs )
  set ( inc_dirs ${INTERNAL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )
  set ( INTERNAL_INCLUDE_DIRS ${inc_dirs} CACHE INTERNAL "List of directories with header files needed for build" )
endmacro ( ot_add_current_dir_to_include_dirs )


set ( CPACK_PACKAGE_NAME          ${PACKAGE_NAME} )
set ( CPACK_PACKAGE_VERSION_MAJOR 0              )
set ( CPACK_PACKAGE_VERSION_MINOR 2              )
set ( CPACK_PACKAGE_VERSION_PATCH 0               )
set ( CPACK_SOURCE_GENERATOR      "TGZ;TBZ2"     )
set ( CPACK_BINARY_STGZ           "OFF"          )
set ( CPACK_BINARY_TBZ2           "ON"           )
set ( CPACK_BINARY_TGZ            "ON"           )
set ( CPACK_BINARY_TZ             "OFF"          )
set ( CPACK_SOURCE_IGNORE_FILES "/.git;/build;.*~;${CPACK_SOURCE_IGNORE_FILES}" )
set ( PACKAGE_NAME           ${CPACK_PACKAGE_NAME} )
set ( PACKAGE_VERSION        ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR} )
if ( CPACK_PACKAGE_VERSION_PATCH )
  set ( PACKAGE_VERSION       ${PACKAGE_VERSION}.${CPACK_PACKAGE_VERSION_PATCH} )
endif ()
set ( CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE_NAME}-${PACKAGE_VERSION} )


# The tests can't be run if this function is absent
enable_testing ()
add_custom_target (tests COMMENT "Build tests")
add_custom_target (check COMMENT "Run pre-installation tests")
add_custom_target (installcheck COMMENT "Run post-installation tests")

add_subdirectory (lib)
add_dependencies (check cppcheck)

if (PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND AND BUILD_SHARED_LIBS)
  add_subdirectory (python)
  add_dependencies (installcheck pyinstallcheck)
endif ()


# uninstall target
configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  IMMEDIATE @ONLY
)

add_custom_target ( uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

set ( OTAGR_LIBRARY                 otagr )
set ( OTAGR_LIBRARIES ${OPENTURNS_LIBRARIES} )
list ( APPEND OTAGR_LIBRARIES ${OTAGR_LIBRARY} )
set ( OTAGR_LIBRARY_DIRS ${OPENTURNS_LIBRARY_DIRS} )
list ( APPEND OTAGR_LIBRARY_DIRS ${LIBRARY_PATH} )
set ( OTAGR_INCLUDE_DIR ${INCLUDE_PATH} )
set ( OTAGR_INCLUDE_DIRS ${OTAGR_INCLUDE_DIR} )
set ( OTAGR_ROOT_DIR       ${CMAKE_INSTALL_PREFIX} )
list ( APPEND OTAGR_INCLUDE_DIRS ${OPENTURNS_INCLUDE_DIRS} )
set ( OTAGR_VERSION_STRING ${PACKAGE_VERSION} )
set ( OTAGR_VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR} )
set ( OTAGR_VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR} )
set ( OTAGR_VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH} )
set ( OTAGR_USE_FILE      ${CONFIG_CMAKE_PATH}/Useotagr.cmake )

if (CMAKE_VERSION VERSION_LESS 2.8.8)
  # CMakePackageConfigHelpers is not available
  set (PACKAGE_INIT "
# Generated by a CMake version without CMakePackageConfigHelpers.
macro(set_and_check _var _value)
  set(\${_var} \"\${_value}\")
endmacro(set_and_check _var _value)
"     )
  foreach (_var
      OTAGR_USE_FILE
      OTAGR_ROOT_DIR
      OTAGR_INCLUDE_DIR
      OTAGR_INCLUDE_DIRS
      OTAGR_LIBRARY_DIRS )
    set(PACKAGE_${_var} ${${_var}})
  endforeach ()

configure_file ( ${CMAKE_SOURCE_DIR}/cmake/otagrConfig.cmake.in
                 ${CMAKE_BINARY_DIR}/otagrConfig.cmake
                 @ONLY ESCAPE_QUOTES
               )

install ( FILES cmake/Useotagr.cmake
                ${CMAKE_BINARY_DIR}/otagrConfig.cmake
          DESTINATION ${OTAGR_CONFIG_CMAKE_PATH}
        )

else ()

  include(CMakePackageConfigHelpers)
  configure_package_config_file (
    cmake/otagrConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/otagrConfig.cmake
    INSTALL_DESTINATION ${OTAGR_CONFIG_CMAKE_PATH}
    PATH_VARS
      OTAGR_USE_FILE
      OTAGR_ROOT_DIR
      OTAGR_INCLUDE_DIR
      OTAGR_INCLUDE_DIRS
      OTAGR_LIBRARY_DIRS
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
                                )
  write_basic_package_version_file( otagrConfigVersion.cmake VERSION ${OTAGR_VERSION_STRING} COMPATIBILITY AnyNewerVersion )

  install ( FILES cmake/Useotagr.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/otagrConfig.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/otagrConfigVersion.cmake
            DESTINATION ${OTAGR_CONFIG_CMAKE_PATH}
          )
endif ()

include (CPack)
