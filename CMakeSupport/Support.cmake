
#
# _zeroPad
#
# Assumes the input string is a number. If necessary, the
# number is 0-padded to two digits.
#
macro( _zeroPad _input _output )
    if( ${${_input}} LESS 10 )
        set( ${_output} "0${${_input}}" )
    else()
        set( ${_output} "${${_input}}" )
    endif()
endmacro()


#
# _listGet
#
# If _keyword is present in ${ARGN}, _result is set to
# the the following space-delimited token. If there is no
# following token, a warning is displayed and _result is unchanged.
# If _keyword is not found, no action is taken.
macro( _listGet _keyword _result )
    set( _tempList ${ARGN} )
    list( FIND _tempList ${_keyword} _temp )
    if( ${_temp} GREATER -1 )
        math( EXPR _valueIndex "${_temp}+1" )
        list( LENGTH _tempList _listLen )
        if( ${_valueIndex} LESS ${_listLen} )
            list( GET _tempList ${_valueIndex} ${_result} )
        else()
            message( WARNING "_listGet keyword ${_keyword} present, but missing parameter." )
        endif()
    endif()
endmacro()


#
# _listFindBool
#
# Like the list(FIND...) CMake macro, but returns a boolean
# result. The _result parameter is set to TRUE if _keyword is
# present anywhere in ${ARGN}. Otherwise, _result is set to FALSE.
macro( _listFindBool _keyword _result )
    set( _tempList ${ARGN} )
    list( FIND _tempList ${_keyword} _temp )
    if( ${_temp} LESS 0 )
        set( ${_result} FALSE )
    else()
        set( ${_result} TRUE )
    endif()
endmacro()


#
# _projectName
#
# Sets the project name and creates an uppercase variant.
#   CMAKE_PROJECT_NAME is set to the given _name.
#   PROJECT_NAME is set to the given _name.
#   _projectNameUpper it set to _name comverted to uppercase.
macro( _projectName _name )
    # project() implicitly sets CMAKE_PROJECT_NAME and PROJECT_NAME
    project( ${_name} )
    # set _projectNameUpper
    string( TOUPPER ${_name} _projectNameUpper )
endmacro()


#
# _projectVersion
#
# Usage:
#   _projectVersion( <maj> <min> <sub>
#                    [ZEROPAD]
#                    [HEADER_SOURCE <headSrc>
#                    HEADER_DEST <headDest> )
#
# Usage note: Set the project name with _projectName(...)
# before invoking this macro.
#
# Sets project version variables and auto-configures a Version.h header file.
# Sets these variables:
#   ${_projectNameUpper}_MAJOR_VERSION to "${_maj}"
#   ${_projectNameUpper}_MINOR_VERSION to "${_min}"
#   ${_projectNameUpper}_SUB_VERSION to "${_sub}"
#
# This macro also sets a variable called ${_projectNameUpper}_VERSION, which
# contains a combined version number. The setting of this variable is
# controlled with the presence of ZEROPAD. If not present,
# ${_projectNameUpper}_VERSION is set to "${_maj}.${_min}.${_sub}". If
# ZEROPAD is present, _min and _sub are zero-padded to two digits. (Compare:
# "3.0.1" or "3.00.01" when ZEROPAD is specified.)
#
# This macro also sets ${_projectNameUpper}_VERSION_COMPACT in the same way
# as ${_projectNameUpper}_VERSION, but with no intervening periods ("301" or
# "30001" when ZEROPAD is specified.)
#
# TBD install?
macro( _projectVersion _maj _min _sub )
    if( NOT _projectNameUpper )
        message( WARNING "_projectVersion: _projectNameUpper not set. You must invoke _projectName(...) before _projectVersion(...)." )
    endif()

    set( ${_projectNameUpper}_MAJOR_VERSION ${_maj}
        CACHE INTERNAL "Project major vertion number" FORCE )
    set( ${_projectNameUpper}_MINOR_VERSION ${_min}
        CACHE INTERNAL "Project minor vertion number" FORCE )
    set( ${_projectNameUpper}_SUB_VERSION ${_sub}
        CACHE INTERNAL "Project sub vertion number" FORCE )

    set( _minorPad ${_min} )
    set( _subPad ${_sub} )
    _listFindBool( ZEROPAD _pad ${ARGN} )
    if( ${_pad} )
        _zeroPad( _minorPad _minorPad )
        _zeroPad( _subPad _subPad )
    endif()

    set( ${_projectNameUpper}_VERSION "${_maj}.${_minorPad}.${_subPad}" )
    set( ${_projectNameUpper}_VERSION_COMPACT "${_maj}${_minorPad}${_subPad}" )

    _listGet( HEADER_SOURCE _headerSource ${ARGN} )
    _listGet( HEADER_DEST _headerDest ${ARGN} )
    if( _headerSource AND _headerDest )
        configure_file( ${_headerSource} ${_headerDest} )
    endif()
endmacro()


#
# _outputToBinDir
#
# Sets the CMake output directory variables so that executables,
# libraries, and archives are all stored in a common bin directory.
# This makes it easy to find all executable code built by the project.
macro( _outputToBinDir )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
endmacro()


#
# _doxygen
#
# Parameters:
#   DOXYFILE_SOURCE <file>
#     If present, <file> is processed by configure_file() and the
#     output is used as the Doxygen configuration file. If not
#     present, <sourcedir>/doc/doxyfile.in is used if it exists.
#     Otherwise, a warning is displayed.
macro( _doxygen )
    if( NOT _projectNameUpper )
        message( WARNING "_doxygen: _projectNameUpper not set. You must invoke _projectName(...) before _doxygen(...)." )
    endif()

    # Do nothing if building documentation is disabled.
    if( NOT ${_projectNameUpper}_DOCUMENTATION )
        return()
    endif()

    find_package( Doxygen )

    if( DOXYGEN_FOUND )
        set( HAVE_DOT "NO" )
        if( DOXYGEN_DOT_PATH )
            set( HAVE_DOT "YES" )
        endif()

        set( _doxyfileSource "" )
        _listGet( DOXYFILE_SOURCE _doxyfileSource ${ARGN} )
        if( _doxyfileSource STREQUAL "" )
            if( EXISTS ${PROJECT_SOURCE_DIR}/doc/doxyfile.in )
                set( _doxyfileSource ${PROJECT_SOURCE_DIR}/doc/doxyfile.in )
            else()
                message( WARNING "_doxygen: Can't find doxyfile source." )
            endif()
        endif()

        configure_file( ${_doxyfileSource}
            ${PROJECT_BINARY_DIR}/doc/doxyfile
        )
        add_custom_target( Documentation
            ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/doc/doxyfile
        )

        if( EXISTS ${PROJECT_SOURCE_DIR}/doc/images/CMakeLists.txt )
            add_subdirectory( doc/images )
        endif()
    endif()
endmacro()




#
# _addLibrary
#
# If FORCE_STATIC is present, _addLibrary creates an archive
# rather than a library (a .lib rather than a .so).
#
macro( _addLibrary _libName )
    # Check to see if we are forcing a static library.
    set( _optionsPlusFiles ${ARGN} )
    list( GET _optionsPlusFiles 0 _option )
    if( _option STREQUAL "FORCE_STATIC" )
        # Remove the FORCE_STATIC option, leaving only file names.
        list( REMOVE_AT _optionsPlusFiles 0 )
    endif()

    if( BUILD_SHARED_LIBS AND NOT ( _option STREQUAL "FORCE_STATIC" ) )
        add_library( ${_libName} SHARED ${_optionsPlusFiles} )
    else()
        add_library( ${_libName} STATIC ${_optionsPlusFiles} )
    endif()

    include_directories(
        ${_projectIncludes}
        ${_dependencyIncludes}
    )
    add_definitions( -DSMSRAIL_LIBRARY )

    target_link_libraries( ${_libName}
        ${_dependencyLibraries}
    )

    set_target_properties( ${_libName} PROPERTIES PROJECT_LABEL "Lib ${_libName}" )

    set( _libName ${_libName} )
    include( ModuleInstall REQUIRED )
    install(
        DIRECTORY .
        DESTINATION include/${_libName}
        USE_SOURCE_PERMISSIONS
        FILES_MATCHING 
        PATTERN "*.h"
        PATTERN ".txt" EXCLUDE
        PATTERN ".cpp" EXCLUDE
        PATTERN ".cxx" EXCLUDE
        PATTERN ".cc" EXCLUDE
    )
endmacro()


macro( _addExecutable CATAGORY _exeName )
    add_executable( ${_exeName} ${ARGN} )

    include_directories(
        ${_projectIncludes}
        ${_dependencyIncludes}
    )

    target_link_libraries( ${_exeName}
        ${_projectLibraries}
        ${_dependencyLibraries}
    )

    install(
        TARGETS ${_exeName}
        RUNTIME DESTINATION bin COMPONENT smsrail
    )

    set_target_properties( ${_exeName} PROPERTIES PROJECT_LABEL "${CATAGORY} ${_exeName}" )
    set_property( TARGET ${_exeName} PROPERTY DEBUG_OUTPUT_NAME "${_exeName}${CMAKE_DEBUG_POSTFIX}" )
endmacro()


#
# Install pdb files for Debug and RelWithDebInfo builds
macro( _windowsInstallPDB )
    if( MSVC )
        install(
            DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\${CMAKE_INSTALL_CONFIG_NAME}/
            DESTINATION lib
            USE_SOURCE_PERMISSIONS
            COMPONENT ${CMAKE_PROJECT_NAME}-dev
            FILES_MATCHING PATTERN "*.pdb"
        )
    endif()
endmacro()

