
configure_file( "Version.h.in"
    "${PROJECT_SOURCE_DIR}/src/libsmsrail/Version.h"
    @ONLY )

_addLibrary( libsmsrail
    types.h
    Export.h
    Version.cpp
    Version.h
    CSV.cpp
    CSV.h
    CSVWriter.cpp
    CSVWriter.h
    Writer.cpp
    Writer.h
)
