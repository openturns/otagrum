cmake_minimum_required(VERSION 2.8.4)

option(BUILD_PYTHON "Build the python module for the library" ON)
option(USE_SPHINX "Use sphinx for documentation" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(CMAKE_CXX_STANDARD 14)


# By default, build in Release mode. Must appear before project() command
if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

project(otagrum)

string(TOLOWER ${CMAKE_PROJECT_NAME} PACKAGE_NAME)

# set module dir to find custom scripts
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


find_package (aGrUM 0.13.4 NO_MODULE REQUIRED)
if (aGrUM_FOUND)
  message(STATUS "Found aGrUM: ${AGRUM_ROOTDIR} (found version \"${AGRUM_VERSION_STRING}\")")
endif ()

find_package(OpenTURNS 1.12 NO_MODULE REQUIRED)
if (OPENTURNS_FOUND)
  message(STATUS "Found OpenTURNS: ${OPENTURNS_ROOT_DIR} (found version \"${OPENTURNS_VERSION_STRING}\")")
endif ()
include(${OPENTURNS_USE_FILE})
set(OTAGRUM_DEFINITIONS ${OPENTURNS_DEFINITIONS})
if (NOT BUILD_SHARED_LIBS)
  list(APPEND OTAGRUM_DEFINITIONS "-DOTAGRUM_STATIC")
endif ()
add_definitions(${OTAGRUM_DEFINITIONS})

set(OTAGRUM_LIBRARY_PATH lib${LIB_SUFFIX})
set(OTAGRUM_INCLUDE_PATH include)
set(OTAGRUM_SWIG_INCLUDE_PATH ${OTAGRUM_INCLUDE_PATH}/${PACKAGE_NAME}/swig)
set(OTAGRUM_DATA_PATH share)
set(OTAGRUM_EXAMPLE_PATH ${OTAGRUM_DATA_PATH}/${PACKAGE_NAME}/examples)
set(OTAGRUM_CONFIG_CMAKE_PATH lib${LIB_SUFFIX}/cmake/otagrum)
set(OTAGRUM_DOC_PATH share/${PACKAGE_NAME}/doc)

if (BUILD_PYTHON)
  # workaround for cmake bug #0013449
  if (CMAKE_VERSION VERSION_LESS 3 AND DEFINED CMAKE_FIND_ROOT_PATH)
    find_program(SWIG_EXECUTABLE NAMES swig2.0 swig)
    if (SWIG_EXECUTABLE)
      set(SWIG_USE_FILE ${CMAKE_ROOT}/Modules/UseSWIG.cmake)
      set(SWIG_FOUND TRUE)
    endif ()
  else ()
    # follow agrum swig version requirement
    find_package(SWIG 3.0.11)
  endif ()
  if (SWIG_FOUND)
    include(${SWIG_USE_FILE})
    include(TargetLinkLibrariesWithDynamicLookup)
  endif ()

  find_package(PythonInterp)
  find_package(PythonLibs)
  include(FindPythonModule)

  if (USE_SPHINX)
    find_program(SPHINX_EXECUTABLE NAMES sphinx-build DOC "Sphinx Documentation Builder (sphinx-doc.org)")
    find_program(PANDOC_EXECUTABLE NAMES pandoc DOC "a universal document converter")
    find_python_module(numpydoc)
    find_python_module(nbsphinx)
    find_python_module(IPython)
    if (SPHINX_EXECUTABLE AND NUMPYDOC_FOUND AND NBSPHINX_FOUND AND IPYTHON_FOUND AND PANDOC_EXECUTABLE)
      find_python_module(sphinx 1.3)
    endif ()
    if (NOT SPHINX_FOUND)
      message(STATUS "Disabling documentation (need sphinx/pandoc/numpydoc/nbsphinx/ipython)")
    endif ()
  endif ()

  if (PYTHONINTERP_FOUND)
    if (DEFINED PYTHON_SITE_PACKAGES)
      set(OTAGRUM_PYTHON_MODULE_PATH ${PYTHON_SITE_PACKAGES})

    else ()
      execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(plat_specific=True, prefix='${CMAKE_INSTALL_PREFIX}'))"
          OUTPUT_VARIABLE _ABS_PYTHON_MODULE_PATH
          RESULT_VARIABLE _PYTHON_pythonlib_result
          OUTPUT_STRIP_TRAILING_WHITESPACE)

      if (_PYTHON_pythonlib_result)
        message(SEND_ERROR "Could not run ${PYTHON_EXECUTABLE}")
      endif ()

      get_filename_component(_ABS_PYTHON_MODULE_PATH ${_ABS_PYTHON_MODULE_PATH} ABSOLUTE)
      file(RELATIVE_PATH OTAGRUM_PYTHON_MODULE_PATH ${CMAKE_INSTALL_PREFIX} ${_ABS_PYTHON_MODULE_PATH})
    endif ()

    if (IS_ABSOLUTE ${OTAGRUM_PYTHON_MODULE_PATH})
      set(PYTHON_MODULE_PATH ${OTAGRUM_PYTHON_MODULE_PATH})
    else ()
      set(PYTHON_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/${OTAGRUM_PYTHON_MODULE_PATH})
    endif ()

    if (CMAKE_VERSION VERSION_LESS "2.8.6")
      execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print(sys.version_info[0])"
          OUTPUT_VARIABLE PYTHON_VERSION_MAJOR
          OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif ()
    set(OTAGRUM_PYTHON${PYTHON_VERSION_MAJOR}_MODULE_PATH ${OTAGRUM_PYTHON_MODULE_PATH})
  endif (PYTHONINTERP_FOUND)
endif ()

# Guess agrum python module path
set(AGRUM_PYTHON_MODULE_PATH ${AGRUM_ROOTDIR}/${OTAGRUM_PYTHON_MODULE_PATH})

# Define variables without OTAGRUM_ prefix with absolute paths
foreach (_var PYTHON_MODULE_PATH LIBRARY_PATH INCLUDE_PATH CONFIG_CMAKE_PATH)
  if (IS_ABSOLUTE "${OTAGRUM_${_var}}")
    set(${_var} ${OTAGRUM_${_var}})
  else ()
    set(${_var} ${CMAKE_INSTALL_PREFIX}/${OTAGRUM_${_var}})
  endif ()
  get_filename_component(${_var} "${${_var}}" ABSOLUTE)
  file(TO_NATIVE_PATH "${${_var}}" NATIVE_${_var})
  string(REPLACE "\\" "\\\\" NATIVE_${_var} ${NATIVE_${_var}})
endforeach ()

# RPATH settings
set(CMAKE_INSTALL_RPATH ${LIBRARY_PATH})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
if (POLICY CMP0042)
  # Set MACOSX_RPATH to ON
  cmake_policy(SET CMP0042 NEW)
endif ()
if (POLICY CMP0078)
  # Set MACOSX_RPATH to ON
  cmake_policy(SET CMP0078 NEW)
endif()
if (POLICY CMP0086)
  cmake_policy (SET CMP0086 NEW)
endif()

# Some useful macros to ease CMakeLists.txt file writing
set(SOURCEFILES "" CACHE INTERNAL "List of source files to compile")
macro(ot_add_source_file FILENAME)
  set(sf ${SOURCEFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME})
  set(SOURCEFILES ${sf} CACHE INTERNAL "List of source files to compile")
endmacro(ot_add_source_file)

set(HEADERFILES "" CACHE INTERNAL "List of header files to install")
macro(ot_install_header_file FILENAME)
  set(hf ${HEADERFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}/${FILENAME})
  set(HEADERFILES ${hf} CACHE INTERNAL "List of header files to install")
endmacro(ot_install_header_file)

set(SWIGFILES "" CACHE INTERNAL "List of SWIG files to install")
macro(ot_install_swig_file FILENAME)
  set(hf ${SWIGFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME})
  set(SWIGFILES ${hf} CACHE INTERNAL "List of SWIG files to install")
endmacro(ot_install_swig_file)

set(INTERNAL_INCLUDE_DIRS "" CACHE INTERNAL "List of directories with header files needed for build")
macro(ot_add_current_dir_to_include_dirs)
  set(inc_dirs ${INTERNAL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
  set(INTERNAL_INCLUDE_DIRS ${inc_dirs} CACHE INTERNAL "List of directories with header files needed for build")
endmacro(ot_add_current_dir_to_include_dirs)


set(CPACK_PACKAGE_NAME ${PACKAGE_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 2)
set(CPACK_PACKAGE_VERSION_PATCH)
set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
set(CPACK_BINARY_STGZ OFF CACHE BOOL "STGZ")
set(CPACK_BINARY_TBZ2 ON CACHE BOOL "TBZ2")
set(CPACK_BINARY_TGZ ON CACHE BOOL "TGZ")
set(CPACK_BINARY_TZ OFF CACHE BOOL "TZ")
set(CPACK_SOURCE_IGNORE_FILES "/.git;/build;.*~;${CPACK_SOURCE_IGNORE_FILES}")
set(PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR})
if (CPACK_PACKAGE_VERSION_PATCH)
  set(PACKAGE_VERSION ${PACKAGE_VERSION}.${CPACK_PACKAGE_VERSION_PATCH})
endif ()
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE_NAME}-${PACKAGE_VERSION})


# The tests can't be run if this function is absent
enable_testing()
add_custom_target(tests COMMENT "Build tests")
add_custom_target(check COMMENT "Run pre-installation tests")
add_custom_target(installcheck COMMENT "Run post-installation tests")

add_subdirectory(lib)
add_dependencies(check cppcheck)

if (PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND)
  add_subdirectory(python)
  add_dependencies(installcheck pyinstallcheck)
endif ()


# uninstall target
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    IMMEDIATE @ONLY
)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )

set ( OTAGRUM_LIBRARY                 otagrum )
set ( OTAGRUM_LIBRARIES ${OPENTURNS_LIBRARIES} )
list ( APPEND OTAGRUM_LIBRARIES ${OTAGRUM_LIBRARY} )
set ( OTAGRUM_LIBRARY_DIRS ${OPENTURNS_LIBRARY_DIRS} )
list ( APPEND OTAGRUM_LIBRARY_DIRS ${LIBRARY_PATH} )
set ( OTAGRUM_INCLUDE_DIR ${INCLUDE_PATH} )
set ( OTAGRUM_INCLUDE_DIRS ${OTAGRUM_INCLUDE_DIR} )
set ( OTAGRUM_ROOT_DIR       ${CMAKE_INSTALL_PREFIX} )
list ( APPEND OTAGRUM_INCLUDE_DIRS ${OPENTURNS_INCLUDE_DIRS} )
set ( OTAGRUM_VERSION_STRING ${PACKAGE_VERSION} )
set ( OTAGRUM_VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR} )
set ( OTAGRUM_VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR} )
set ( OTAGRUM_VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH} )
set ( OTAGRUM_USE_FILE      ${CONFIG_CMAKE_PATH}/Useotagrum.cmake )

if (CMAKE_VERSION VERSION_LESS 2.8.8)
  # CMakePackageConfigHelpers is not available
  set(PACKAGE_INIT "
# Generated by a CMake version without CMakePackageConfigHelpers.
macro(set_and_check _var _value)
  set(\${_var} \"\${_value}\")
endmacro(set_and_check _var _value)
")
  foreach (_var
      OTAGRUM_USE_FILE
      OTAGRUM_ROOT_DIR
      OTAGRUM_INCLUDE_DIR
      OTAGRUM_INCLUDE_DIRS
      OTAGRUM_LIBRARY_DIRS)
    set(PACKAGE_${_var} ${${_var}})
  endforeach ()

configure_file ( ${CMAKE_SOURCE_DIR}/cmake/otagrumConfig.cmake.in
                 ${CMAKE_BINARY_DIR}/otagrumConfig.cmake
                 @ONLY ESCAPE_QUOTES
               )

install ( FILES cmake/Useotagrum.cmake
                ${CMAKE_BINARY_DIR}/otagrumConfig.cmake
          DESTINATION ${OTAGRUM_CONFIG_CMAKE_PATH}
        )

else ()

  include(CMakePackageConfigHelpers)
  configure_package_config_file (
    cmake/otagrumConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/otagrumConfig.cmake
    INSTALL_DESTINATION ${OTAGRUM_CONFIG_CMAKE_PATH}
    PATH_VARS
      OTAGRUM_USE_FILE
      OTAGRUM_ROOT_DIR
      OTAGRUM_INCLUDE_DIR
      OTAGRUM_INCLUDE_DIRS
      OTAGRUM_LIBRARY_DIRS
      NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )
  write_basic_package_version_file( otagrumConfigVersion.cmake VERSION ${OTAGRUM_VERSION_STRING} COMPATIBILITY AnyNewerVersion )

  install ( FILES cmake/Useotagrum.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/otagrumConfig.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/otagrumConfigVersion.cmake
            DESTINATION ${OTAGRUM_CONFIG_CMAKE_PATH}
          )
endif ()

include(CPack)
