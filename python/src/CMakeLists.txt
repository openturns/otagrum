
include_directories (${OPENTURNS_INCLUDE_DIRS})
include_directories (${OPENTURNS_SWIG_INCLUDE_DIRS})
add_definitions (${OPENTURNS_SWIG_DEFINITIONS})
include_directories (${INTERNAL_INCLUDE_DIRS})

include_directories (${AGRUM_INCLUDE_DIRS})

# allows one to pass compile flags like -O1 to reduce memory usage
set (SWIG_COMPILE_FLAGS "" CACHE STRING "C++ compiler flags used for wrapper code")

macro (ot_add_python_module MODULENAME SOURCEFILE)
  set_source_files_properties (${SOURCEFILE} PROPERTIES CPLUSPLUS ON)
  set_source_files_properties (${SOURCEFILE} PROPERTIES SWIG_MODULE_NAME ${MODULENAME})
  ot_install_swig_file (${SOURCEFILE})
  if (${ARGC} GREATER 2)
    set (SWIG_MODULE_${MODULENAME}_python_EXTRA_DEPS ${ARGN})
  endif ()

  set (swig_other_sources)
  set (docstring_sources)
  foreach (it ${ARGN})
    if (${it} MATCHES ".*\\.i$")
      ot_install_swig_file (${it})
    elseif (${it} MATCHES ".*_doc\\.i\\.in$")
      list (APPEND docstring_sources ${it})

      get_filename_component (DOCSTRING_FILE_BASENAME ${it} NAME_WE)
      ot_install_swig_file (${CMAKE_CURRENT_BINARY_DIR}/${DOCSTRING_FILE_BASENAME}.i)
    else ()
      list (APPEND swig_other_sources "${it}")
    endif ()
  endforeach ()

  # https://github.com/swig/swig/issues/1273
  option (SWIG_FORCE_DOUBLE_ESCAPE_BACKSLASH "Force double escaping of backslashes" OFF)
  mark_as_advanced (SWIG_FORCE_DOUBLE_ESCAPE_BACKSLASH)
  set (TIMES x1)
  if ((SWIG_VERSION VERSION_LESS 4) OR SWIG_FORCE_DOUBLE_ESCAPE_BACKSLASH)
    set (TIMES x2)
  endif ()

  # handle docstring generation
  add_custom_target (generate_${MODULENAME}_docstrings
                      COMMAND ${CMAKE_COMMAND} -DDOCSTRING_SOURCES="${docstring_sources}"
                      -DCURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DTIMES=${TIMES}
                      -P ${CMAKE_SOURCE_DIR}/cmake/escape_backslash.cmake
                      DEPENDS ${docstring_sources})
  list (APPEND SWIG_MODULE_${MODULENAME}_python_EXTRA_DEPS generate_${MODULENAME}_docstrings)

  # swig_add_module is deprecated
  swig_add_library (${MODULENAME}_python LANGUAGE python SOURCES ${SOURCEFILE} ${swig_other_sources})

  # UseSWIG generates now standard target names
  set (module_target ${MODULENAME}_python)

  swig_link_libraries (${MODULENAME}_python otagrum)
  target_include_directories (${MODULENAME}_python PRIVATE ${Python_INCLUDE_DIRS})

  if (TARGET Python::Module)
    target_link_libraries (${module_target} Python::Module)
  else ()
    target_link_libraries_with_dynamic_lookup (${module_target} ${Python_LIBRARIES})
  endif ()

  set_target_properties (${module_target} PROPERTIES OUTPUT_NAME ${MODULENAME})

  set_target_properties (${module_target} PROPERTIES COMPILE_FLAGS "${SWIG_COMPILE_FLAGS}")

  set_target_properties (${module_target} PROPERTIES UNITY_BUILD OFF)

  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULENAME}.py
           DESTINATION ${OTAGRUM_PYTHON_MODULE_PATH}/${PACKAGE_NAME})
  install (TARGETS ${module_target}
            LIBRARY DESTINATION ${OTAGRUM_PYTHON_MODULE_PATH}/${PACKAGE_NAME})

  list (APPEND OTAGRUM_PYTHON_MODULES ${MODULENAME})
endmacro ()



ot_add_python_module (${PACKAGE_NAME} ${PACKAGE_NAME}_module.i otagrum_agrum.i
                      ContinuousPC.i ContinuousPC_doc.i
                      ContinuousMIIC.i ContinuousMIIC_doc.i
                      TabuList.i TabuList_doc.i
                      ContinuousTTest.i ContinuousTTest_doc.i
                      CorrectedMutualInformation.i CorrectedMutualInformation_doc.i
                      NamedJunctionTree.i NamedJunctionTree_doc.i
                      NamedDAG.i NamedDAG_doc.i
                      Utils.i Utils_doc.i
                      JunctionTreeBernsteinCopula.i JunctionTreeBernsteinCopula_doc.i
                      JunctionTreeBernsteinCopulaFactory.i JunctionTreeBernsteinCopulaFactory_doc.i
                      ContinuousBayesianNetwork.i ContinuousBayesianNetwork_doc.i
                      ContinuousBayesianNetworkFactory.i ContinuousBayesianNetworkFactory_doc.i
                     )


install ( FILES __init__.py
          DESTINATION ${OTAGRUM_PYTHON_MODULE_PATH}/${PACKAGE_NAME}
        )


if (SPHINX_FOUND)

  # create a build site-package from which sphinx can import
  set (OTAGRUM_PYTHON_BUILD_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}) #/site-packages)
  execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory ${OTAGRUM_PYTHON_BUILD_MODULE_PATH}/otagrum)
  foreach (module ${OTAGRUM_PYTHON_MODULES})
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/${module}.py ${OTAGRUM_PYTHON_BUILD_MODULE_PATH}/otagrum/${module}.py)
    # FIXME: get_target_property (suffix ${SWIG_MODULE_${module}_REAL_NAME} SUFFIX)
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/_${module}.so ${OTAGRUM_PYTHON_BUILD_MODULE_PATH}/otagrum/_${module}.so)
  endforeach ()
  foreach (file __init__.py)
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${OTAGRUM_PYTHON_BUILD_MODULE_PATH}/otagrum/${file})
  endforeach ()

  # sphinx configuration
  set (doc_formats)
  list (APPEND doc_formats html)
  list (APPEND doc_formats htmlhelp)
  list (APPEND doc_formats latex)
  list (APPEND doc_formats doctest)
  list (APPEND doc_formats qthelp)
  list (APPEND doc_formats devhelp)

  set (SPHINX_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sphinx_build)
  set (SPHINX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../doc)

  file (MAKE_DIRECTORY ${SPHINX_BINARY_DIR}/_static) # currently empty

  configure_file (${SPHINX_SOURCE_DIR}/conf.py.in ${SPHINX_BINARY_DIR}/conf.py @ONLY)

  # get sphinx sources
  file (GLOB_RECURSE sphinx_sources
    ${SPHINX_SOURCE_DIR}/*.rst
    ${SPHINX_SOURCE_DIR}/*.rst_t
    ${SPHINX_SOURCE_DIR}/*.css_t
    ${SPHINX_SOURCE_DIR}/*.conf
    ${SPHINX_SOURCE_DIR}/*.py
    ${SPHINX_SOURCE_DIR}/*.png
    ${SPHINX_SOURCE_DIR}/*.html
    ${SPHINX_SOURCE_DIR}/*.css
    ${SPHINX_SOURCE_DIR}/*.jpg
    ${SPHINX_SOURCE_DIR}/*.ico
    ${SPHINX_SOURCE_DIR}/*.sty
    ${SPHINX_SOURCE_DIR}/*.txt
    ${SPHINX_SOURCE_DIR}/*.in
    ${SPHINX_SOURCE_DIR}/*.inv
  )

  # copy sphinx sources
  set (sphinx_clones)
  foreach (file ${sphinx_sources})
    if (EXISTS ${file})
      file ( RELATIVE_PATH rel_file ${SPHINX_SOURCE_DIR} ${file} )
      get_filename_component (rel_path ${rel_file} PATH)
      file (MAKE_DIRECTORY ${SPHINX_BINARY_DIR}/${rel_path})
      set (sphinx_clone ${SPHINX_BINARY_DIR}/${rel_file})
      add_custom_command (OUTPUT ${sphinx_clone}
        COMMAND ${CMAKE_COMMAND} -E copy ${file} ${sphinx_clone}
        DEPENDS ${file}
      )
      list (APPEND sphinx_clones ${sphinx_clone})
    endif ()
  endforeach ()

  set (SPHINX_FLAGS "-W -T" CACHE STRING "sphinx flags")
  separate_arguments (SPHINX_FLAGS)

  # generate sphinx rule for each format
  foreach (format ${doc_formats})
    add_custom_target ( sphinx_${format}
                        COMMAND ${SPHINX_EXECUTABLE}
                                  -b ${format}
                                  -d ${SPHINX_BINARY_DIR}/.doctrees
                                  ${SPHINX_FLAGS}
                                  ${SPHINX_BINARY_DIR}
                                  ${SPHINX_BINARY_DIR}/${format}
                        DEPENDS ${sphinx_clones}
                      )
    foreach (MODULENAME ${OTAGRUM_PYTHON_MODULES})
      set (module_target ${MODULENAME}_python)
      add_dependencies (sphinx_${format} ${module_target})
    endforeach ()
  endforeach ()

  # build html by default
  set_target_properties (sphinx_html PROPERTIES EXCLUDE_FROM_ALL OFF)

  add_custom_command (OUTPUT ${SPHINX_BINARY_DIR}/latex/OpenTURNS.pdf
                      WORKING_DIRECTORY ${SPHINX_BINARY_DIR}/latex
                      COMMAND make all-pdf
                    )
  add_custom_target (sphinx_pdf DEPENDS ${SPHINX_BINARY_DIR}/latex/OpenTURNS.pdf)
  add_dependencies (sphinx_pdf sphinx_latex)

  install (DIRECTORY ${SPHINX_BINARY_DIR}/html DESTINATION ${OTAGRUM_DOC_PATH})
endif ()

# wheel files
configure_file (METADATA.in METADATA @ONLY)
configure_file (WHEEL.in WHEEL @ONLY)
install (DIRECTORY DESTINATION ${OTAGRUM_PYTHON_MODULE_PATH}/${PACKAGE_NAME}-${PACKAGE_VERSION}.dist-info)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/METADATA ${CMAKE_CURRENT_BINARY_DIR}/WHEEL
         DESTINATION ${OTAGRUM_PYTHON_MODULE_PATH}/${PACKAGE_NAME}-${PACKAGE_VERSION}.dist-info)
