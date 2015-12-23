// Copyright

#ifndef __SMSRAIL_EXPORT_H__
#define __SMSRAIL_EXPORT_H__ 1


#if defined( _MSC_VER ) || defined( __CYGWIN__ ) || defined( __MINGW32__ ) || defined( __BCPLUSPLUS__ ) || defined( __MWERKS__ )
    #if defined( SMSRAIL_STATIC )
        #define SMSRAIL_EXPORT
    #elif defined( SMSRAIL_LIBRARY )
        #define SMSRAIL_EXPORT __declspec( dllexport )
    #else
        #define SMSRAIL_EXPORT __declspec( dllimport )
    #endif
#else
    #define SMSRAIL_EXPORT
#endif


// __SMSRAIL_EXPORT_H__
#endif
