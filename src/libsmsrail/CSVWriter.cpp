// Copyright


#include "CSVWriter.h"
#include <fstream>


CSVWriter::CSVWriter()
    : Writer( CSV_WRITER )
{
}
CSVWriter::~CSVWriter()
{
}

bool CSVWriter::write( const IntersectionVec& inter, const std::string& fileName )
{
    std::ofstream ofstr( fileName.c_str() );
    if( ofstr.bad() )
        return( false );

    unsigned int npid( 0 );

    ofstr << "TYPE,UNIQUEID,REF INTERSECTION,X,Y,ROAD0,ROAD1,ROAD2,ROAD3,ROAD4" << std::endl;
    for( unsigned int idx=0; idx < inter.size(); ++idx )
    {
        const IntersectionPtr& i( inter[ idx ] );

        ofstr << "INTER,";     // type
        ofstr << idx << ",";   // unique ID
        ofstr << ",";          // ref intersection
        ofstr << i->getPoint()[ 0 ] << ",";   // x
        ofstr << i->getPoint()[ 1 ] << ",";   // y

        const StringSet& roadNames( i->getRoadNames() );
        StringSet::const_iterator itr;
        unsigned int roadCount( 0 );
        if( roadNames.size() > 5 )
        {
            std::cerr << "CSVWriter: Warning: Too many roads (" << roadNames.size() << ") at an intersection." << std::endl;
        }
        for( itr = roadNames.begin(); roadCount < 5, itr != roadNames.end(); ++itr )
        {
            ofstr << *itr;
            if( roadCount++ < 4 )
                ofstr << ",";
        }
        while( roadCount++ < 4 )
            ofstr << ",";
        ofstr << std::endl;


        //
        // Write the node points for this intersection

        const Intersection::NodePointInfoSimpleVec& npvec( i->getNodePoints() );
        for( unsigned int tdx=0; tdx < npvec.size(); ++tdx )
        {
            const Intersection::NodePointInfo& npi( npvec[ tdx ] );

            ofstr << "INTER-NP,";    // type
            ofstr << npid++ << ",";  // unique ID
            ofstr << idx << ",";     // ref intersection
            ofstr << npi._point[0] << ",";   // x
            ofstr << npi._point[1] << ",";   // y
            ofstr << npi._road->getName() <<  ",";   // road0
            ofstr << ",,,";         // roads 1-4
            ofstr << std::endl;
        }
    }

    return( true );
}
