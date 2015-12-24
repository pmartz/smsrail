//Copyright


#include "Version.h"
#include <string>
#include <sstream>

namespace libsmsrail {


unsigned int
getVersionNumber()
{
    return( SMSRAIL_VERSION );
}


static std::string s_smsrail_version( "" );

std::string
getVersionString()
{
    if( s_smsrail_version.empty() )
    {
        std::ostringstream oStr;
        oStr << std::string( "smsrail version " ) <<
            SMSRAIL_MAJOR_VERSION << "." <<
            SMSRAIL_MINOR_VERSION << "." <<
            SMSRAIL_SUB_VERSION << " (" <<
            getVersionNumber() << ").";
        s_smsrail_version = oStr.str();
    }
    return( s_smsrail_version );
}


// namespace libsmsrail
}
