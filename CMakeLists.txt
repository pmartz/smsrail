#
# smsrail - Skew Matrix model railroad testbed
#

cmake_minimum_required( VERSION 2.8.5 )
project( smsrail )

set( SMSRAIL_VERION 0.00.00 )
set( SMSRAIL_MAJOR_VERSION 0 )
set( SMSRAIL_MINOR_VERSION 0 )
set( SMSRAIL_SUB_VERSION 0 )


set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH} )
include( CMakeMacros )


# Installation directories
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )


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
# Optional dependencies.


#
# Optional Doxygen, for documentation.
# add a custom target to build documentation.
option( SMSRAIL_DOCUMENTATION "Enable to create build target for Doxygen documentation." OFF )
mark_as_advanced( CLEAR SMSRAIL_DOCUMENTATION )
if( SMSRAIL_DOCUMENTATION )
    find_package( Doxygen REQUIRED )
    set( HAVE_DOT "NO" )
    if( DOXYGEN_DOT_PATH )
        set( HAVE_DOT "YES" )
    endif( DOXYGEN_DOT_PATH )

    configure_file( ${PROJECT_SOURCE_DIR}/doc/doxyfile.cmake
        ${PROJECT_BINARY_DIR}/doc/doxyfile
    )
    add_custom_target( Documentation ${DOXYGEN_EXECUTABLE}
        ${PROJECT_BINARY_DIR}/doc/doxyfile
    )
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
    "${PROJECT_SOURCE_DIR}/src"
)

set( _projectLibraries
    libsmsrail
)

set( _dependencyIncludes
    ${Boost_INCLUDE_DIR}
)

set( _dependencyLibraries
    ${Boost_LIBRARIES}
)


add_subdirectory( src )



#
# Install pdb files for Debug and RelWithDebInfo builds
if( MSVC )
    install(
        DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\${CMAKE_INSTALL_CONFIG_NAME}/
        DESTINATION lib
        USE_SOURCE_PERMISSIONS
        COMPONENT smsrail-dev
        FILES_MATCHING PATTERN "*.pdb"
    )
endif()
