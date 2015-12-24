// Copyright

#ifndef __SMSRAIL_WRITER_H__
#define __SMSRAIL_WRITER_H__ 1


#include <export.h>
#include <types.h>


/** \class Writer Writer.h
*/
class SMSRAIL_EXPORT Writer
{
public:
    typedef enum {
        CSV_WRITER
    } WriterType;

    Writer( const WriterType writerType );
    virtual ~Writer();

    // This is a factory design pattern
    static WriterPtr create( const WriterType writerType );

    WriterType getType() const;

protected:
    WriterType _writerType;
};


// __SMSRAIL_WRITER_H__
#endif
