// Copyright


#include "Writer.h"
#ifdef OSG_FOUND
#  include "OSGWriter.h"
#endif
#include "CSVWriter.h"


Writer::Writer( const WriterType writerType )
    : _writerType( writerType )
{
}
Writer::~Writer()
{
}

WriterPtr Writer::create( const WriterType writerType )
{
    switch( writerType )
    {
    case OSG_WRITER:
        {
#ifdef OSG_FOUND
            OSGWriterPtr writer( new OSGWriter() );
            return( writer );
#else
            std::cerr << "Writer::create(): OSG_WRITER disabled; no OSG support." << std::endl;
            return( WriterPtr( (Writer*)NULL ) );
#endif
        }
        break;

    case CSV_WRITER:
        {
            CSVWriterPtr writer( new CSVWriter() );
            return( writer );
        }
        break;

    default:
        std::cerr << "Writer::create(): Invalid WriterType." << std::endl;
        return( WriterPtr( (Writer*)NULL ) );
        break;
    }
}
Writer::WriterType Writer::getType() const
{
    return( _writerType );
}
