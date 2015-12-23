_addExecutable( Test dummy
    main.cpp
)

add_test( NAME dummy CONFIGURATIONS Release
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
    COMMAND dummy
)
