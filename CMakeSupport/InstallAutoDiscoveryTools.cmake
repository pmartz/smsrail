# Adds all project libraries to the <package>LibraryDepends.cmake
# file so that downstream projects can find the libraries in
# a build tree.
if( EXISTS "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}LibraryDepends.cmake" )
    export( TARGETS ${_projectLibraries} APPEND
        FILE "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}LibraryDepends.cmake")
else()
    export( TARGETS ${_projectLibraries}
        FILE "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}LibraryDepends.cmake")
endif()

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export( PACKAGE ${CMAKE_PROJECT_NAME} )

# Create a BuildTreeSettings.cmake file for the use from the build tree
configure_file( ${_supportDir}/PackageBuildTreeSettings.cmake.in
    "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}BuildTreeSettings.cmake" @ONLY )
configure_file( ${_supportDir}/PackageConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake" @ONLY )
configure_file( ${_supportDir}/UsePackage.cmake.in
    "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/Use${CMAKE_PROJECT_NAME}.cmake" @ONLY )

include( CMakePackageConfigHelpers )
# Create the <pkgname>Config.cmake and <pkgname>ConfigVersion files
configure_package_config_file( 
    ${_supportDir}/PackageConfig.cmake.in 
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PATH_VARS ${VES_CONFIGURE_VARS} )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}Config.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/Use${CMAKE_PROJECT_NAME}.cmake
         DESTINATION ${CMAKE_INSTALL_LIBDIR}
         COMPONENT development )

# Install the CMake find script
configure_file( ${_supportDir}/FindPackage.cmake.in
    "${PROJECT_BINARY_DIR}/Find${CMAKE_PROJECT_NAME}.cmake" @ONLY)
install( FILES "${PROJECT_BINARY_DIR}/Find${CMAKE_PROJECT_NAME}.cmake"
    DESTINATION "share/${CMAKE_PROJECT_NAME}/extra"
    COMPONENT development )
    # RENAME Find${CMAKE_PROJECT_NAME}.cmake )

# Install the export set for use with the install-tree
install( EXPORT ${CMAKE_PROJECT_NAME}-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT development )

# TBD move this to the fpc cmake code
# setup the fpc variables for the fpc file creation
set( VES_FPC_PREFIX "\${fp_file_cwd}/../.." )
if( CMAKE_LIBRARY_ARCHITECTURE )
    set( VES_FPC_PREFIX "\${fp_file_cwd}/../../.." )
endif()

