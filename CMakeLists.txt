#
# smsrail - Skew Matrix model railroad testbed
#

cmake_minimum_required( VERSION 2.8.5 )
set( _supportDir ${PROJECT_SOURCE_DIR}/CMakeSupport )
set( CMAKE_MODULE_PATH ${_supportDir} ${CMAKE_MODULE_PATH} )
include( Support )

_projectName( smsrail )

_projectVersion( 0 1 0 ZEROPAD
    HEADER_SOURCE ${PROJECT_SOURCE_DIR}/src/libsmsrail/version.h.in
    HEADER_DEST ${PROJECT_SOURCE_DIR}/src/libsmsrail/version.h )

    
# Build and installation directories
_outputDirs()
_installDirs()


if( WIN32 AND MSVC )
    # Global warning disable.
    # C4251: <membervar> : class <classX> needs to have dll-interface to be used by clients of class <classY>
    # C4275: non dll-interface class <classX> used as base for dll-interface class <classY>
    foreach( warning 4251 4275 )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${warning}" )
    endforeach()

    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )

    set( CMAKE_DEBUG_POSTFIX d )
else()
    # Enable C++0x in gcc.
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )
endif()


#
# Required Boost
set( _boostComponents thread program_options date_time chrono system )
find_package( Boost 1.55.0 REQUIRED ${_boostComponents} )
if( BOOST_ALL_DYN_LINK )
    add_definitions( -DBOOST_ALL_DYN_LINK )
    set( USE_BOOST_DEFINITION "add_definitions( -DBOOST_ALL_DYN_LINK )" )
endif()


#
# Optional Doxygen, for documentation.
# add a custom target to build documentation.
_doxygen(
    DOXYFILE_SOURCE ${PROJECT_SOURCE_DIR}/doc/doxyfile.in )


#
# Installation of data files is optional and off by default.
option( SMSRAIL_INSTALL_DATA "Enable to add the data directory to the install target" OFF )
if( SMSRAIL_INSTALL_DATA )
    add_subdirectory( data )
endif()


#
# By default, build shared libraries.
set( BUILD_SHARED_LIBS ON CACHE BOOL "Build shared or static libs" )
    
if( WIN32 )
    # Common debug postfix for Windows.
    set( CMAKE_DEBUG_POSTFIX d )
endif()


enable_testing()


set( _projectIncludes
    "${PROJECT_SOURCE_DIR}/src/libsmsrail"
)
set( _projectLibraries
    smsrail
)

set( _dependencyIncludes
    ${Boost_INCLUDE_DIR}
)
set( _dependencyLibraries
    ${Boost_LIBRARIES}
)


add_subdirectory( src )



# 
# Install management
#
# Install .pdb files for Windows builds
_windowsInstallPDB()


#
# Create and install all of the auto find tools
# =============================================

# Add all targets to the build-tree export set
include( InstallAutoDiscoveryTools )

#
# Setup the fpc auto detection files
if(WIN32)
    set(${CMAKE_PROJECT_NAME}_FPC_LIBRARIES "/libpath:\"\${libdir}\" ${CMAKE_PROJECT_NAME}.lib")
    set(${CMAKE_PROJECT_NAME}_FPC_INCLUDES "/I\"\${includedir}\"")
else(WIN32)
    set(${CMAKE_PROJECT_NAME}_FPC_INCLUDES "-I\${includedir}")
    set(${CMAKE_PROJECT_NAME}_FPC_LIBRARIES "-L\${libdir} -l${CMAKE_PROJECT_NAME}")
endif(WIN32)
set(latticefx_VERSION "${LATTICEFX_VERSION}")

configure_file( ${_supportDir}/fpc.in
  "${PROJECT_BINARY_DIR}/lib/flagpoll/${CMAKE_PROJECT_NAME}.fpc" @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/flagpoll/${CMAKE_PROJECT_NAME}.fpc
        DESTINATION lib/flagpoll )

