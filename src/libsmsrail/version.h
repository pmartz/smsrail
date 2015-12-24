//
// This file is automatically generated. To make changes, edit
// version.h.in, then reconfigure using CMake. This will generate
// a new version.h file contaiig your changes.
//


#ifndef __SMSRAIL_CMAKE_GENERATED_VERSION_H__
#define __SMSRAIL_CMAKE_GENERATED_VERSION_H__ 1



/** \defgroup Version Version utilities */
/*@{*/

#define SMSRAIL_MAJOR_VERSION (0)
#define SMSRAIL_MINOR_VERSION (1)
#define SMSRAIL_SUB_VERSION (0)

/** \brief smsrail version number as an integer

C preprocessor integrated version number
The form is Mmmss, where:
   \li M is the major version
   \li mm is the minor version (zero-padded)
   \li ss is the sub version (zero padded)

Use this in version-specific code, for example:
\code
   #if( SMSRAIL_VERSION < 10500 )
      ... code specific to releases before v1.05
   #endif
\endcode
*/
#define SMSRAIL_VERSION ( \
        ( SMSRAIL_MAJOR_VERSION * 10000 ) + \
        ( SMSRAIL_MINOR_VERSION * 100 ) + \
        SMSRAIL_SUB_VERSION )


/** \brief smsrail version number as a const char*.

Example:
\code
smsrail version 1.01.00 (10100)
\endcode
*/
#define SMSRAIL_VERSION_STRING "smsrail version 0.01.00 (00100)"

/*@}*/


// __SMSRAIL_CMAKE_GENERATED_VERSION_H__
#endif
