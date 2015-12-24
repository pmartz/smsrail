// Copyright

#ifndef __SMSRAIL_CSV_WRITER_H__
#define __SMSRAIL_CSV_WRITER_H__ 1


#include <export.h>
#include <Writer.h>
#include <memory>


/** \class CSVWriter CSVWriter.h
*/
class SMSRAIL_EXPORT CSVWriter : public Writer
{
public:
    CSVWriter();
    virtual ~CSVWriter();

protected:
};

typedef std::shared_ptr< CSVWriter > CSVWriterPtr;


// __SMSRAIL_CSV_WRITER_H__
#endif
