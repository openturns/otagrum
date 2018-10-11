#include_directories("${CMAKE_SOURCE_DIR}/external/aGrUM/src")

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( Utils.cxx )
ot_add_source_file ( MixedHistogramUserDefined.cxx )
ot_add_source_file ( JunctionTree.cxx )
ot_add_source_file ( StratifiedCache.cxx)
ot_add_source_file ( ContinuousTTest.cxx)
ot_add_source_file ( IndicesManip.cxx)
ot_add_source_file (ContinuousPC.cxx)

ot_install_header_file ( MixedHistogramUserDefined.hxx )
ot_install_header_file ( Utils.hxx )
ot_install_header_file ( JunctionTree.hxx )
ot_install_header_file ( StratifiedCache.hxx)
ot_install_header_file ( ContinuousTTest.hxx)
ot_install_header_file ( IndicesManip.hxx)
ot_install_header_file ( ContinuousPC.hxx)

include_directories ( ${INTERNAL_INCLUDE_DIRS} )

add_library ( otagrum ${SOURCEFILES} )
if ( BUILD_SHARED_LIBS )
  set_target_properties ( otagrum PROPERTIES COMPILE_DEFINITIONS "OTAGRUM_DLL_EXPORTS" )
endif ()

if ( NOT DEFINED LIB_VERSION )
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION )
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( otagrum PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( otagrum PROPERTIES SOVERSION ${LIB_SOVERSION} )


if(CMAKE_BUILD_TYPE MATCHES Debug)
  target_link_libraries (otagrum ${OPENTURNS_LIBRARY} agrum-dbg)
  message(STATUS "linking against agrum-dbg")
else(CMAKE_BUILD_TYPE MATCHES Debug)
  target_link_libraries (otagrum ${OPENTURNS_LIBRARY} agrum)
  message(STATUS "linking against agrum")
endif(CMAKE_BUILD_TYPE MATCHES Debug)

# Add targets to the build-tree export set
export (TARGETS otagrum FILE ${PROJECT_BINARY_DIR}/otagrum-Targets.cmake)

# Install the export set for use with the install-tree
install(EXPORT otagrum-Targets
        DESTINATION "${OTAGRUM_CONFIG_CMAKE_PATH}"
        COMPONENT Development)

install(TARGETS otagrum
        EXPORT  otagrum-Targets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${OTAGRUM_LIBRARY_PATH}
        ARCHIVE DESTINATION ${OTAGRUM_LIBRARY_PATH}
)

