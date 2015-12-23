
macro( _addLibrary TRGTNAME )
    # Check to see if we are forcing a static library.
    set( _optionsPlusFiles ${ARGN} )
    list( GET _optionsPlusFiles 0 _option )
    if( _option STREQUAL "FORCE_STATIC" )
        # Remove the FORCE_STATIC option, leaving only file names.
        list( REMOVE_AT _optionsPlusFiles 0 )
    endif()

    if( BUILD_SHARED_LIBS AND NOT ( _option STREQUAL "FORCE_STATIC" ) )
        add_library( ${TRGTNAME} SHARED ${_optionsPlusFiles} )
    else()
        add_library( ${TRGTNAME} STATIC ${_optionsPlusFiles} )
    endif()

    include_directories(
        ${_projectIncludes}
        ${_dependencyIncludes}
    )
    add_definitions( -DSMSRAIL_LIBRARY )

    target_link_libraries( ${TRGTNAME}
        ${_dependencyLibraries}
    )

    set_target_properties( ${TRGTNAME} PROPERTIES PROJECT_LABEL "Lib ${TRGTNAME}" )

    set( _libName ${TRGTNAME} )
    include( ModuleInstall REQUIRED )
    install(
        DIRECTORY .
        DESTINATION include/${TRGTNAME}
        USE_SOURCE_PERMISSIONS
        FILES_MATCHING 
        PATTERN "*.h"
        PATTERN ".txt" EXCLUDE
        PATTERN ".cpp" EXCLUDE
        PATTERN ".cxx" EXCLUDE
        PATTERN ".cc" EXCLUDE
    )
endmacro()


macro( _addExecutable CATAGORY EXENAME )
    add_executable( ${EXENAME} ${ARGN} )

    include_directories(
        ${_projectIncludes}
        ${_dependencyIncludes}
    )

    target_link_libraries( ${EXENAME}
        ${_projectLibraries}
        ${_dependencyLibraries}
    )

    install(
        TARGETS ${EXENAME}
        RUNTIME DESTINATION bin COMPONENT smsrail
    )

    set_target_properties( ${EXENAME} PROPERTIES PROJECT_LABEL "${CATAGORY} ${EXENAME}" )
    set_property( TARGET ${EXENAME} PROPERTY DEBUG_OUTPUT_NAME "${EXENAME}${CMAKE_DEBUG_POSTFIX}" )
endmacro()
