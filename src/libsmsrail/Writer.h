// Copyright

#ifndef __SMSRAIL_WRITER_H__
#define __SMSRAIL_WRITER_H__ 1


#include <Export.h>
#include <Road.h>
#include <Intersection.h>


/** \class Writer Writer.h
*/
class SMSRAIL_EXPORT Writer
{
public:
    typedef enum {
        OSG_WRITER,
        CSV_WRITER
    } WriterType;

    Writer( const WriterType writerType );
    virtual ~Writer();

    // This is a factory design pattern
    static WriterPtr create( const WriterType writerType );

    WriterType getType() const;

    virtual bool write( const RoadVec& roads, const std::string& fileName )
    {
        return( false );
    }

    virtual bool write( const IntersectionVec& roads, const std::string& fileName )
    {
        return( false );
    }

protected:
    WriterType _writerType;
};


// __SMSRAIL_WRITER_H__
#endif
