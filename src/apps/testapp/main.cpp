// Copyright


#include <CSV.h>
#include "Profile.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <set>
#include <vector>
#include <iostream>
#include <string>

namespace bpo = boost::program_options;



// #define VERBOSE


bool noMat( false );

typedef std::vector< bool > Validity;
static Validity s_valid;


typedef std::set< unsigned int > IntSet;

static void initIntSet( IntSet& v, const unsigned int size, const unsigned int current=999999999 )
{
    v.clear();
    for( unsigned int idx = 0; idx < size; ++idx )
        if( idx != current )
            v.insert( idx );
}
static void removeInt( IntSet& v, const unsigned int idx )
{
    IntSet::iterator it( v.find( idx ) );
    if( it != v.end() )
        v.erase( it );
}
static bool hasInt( const IntSet& v, const unsigned int idx )
{
    IntSet::const_iterator it( v.find( idx ) );
    return( it != v.end() );
}
static bool checkIntSet( const IntSet& v )
{
    return( v.empty() );
}


static void label( const CSV& in, CSV& out )
{
    unsigned int sCols, sRows;
    in.getDims( sCols, sRows );
    unsigned int dCols, dRows;
    out.getDims( dCols, dRows );

    unsigned int colIdx( 0 );
    unsigned int rowIdx( 0 );
    for( unsigned int idx=0; idx < dCols; ++idx )
    {
        out.setColLabel( idx,
            in.getColLabel( colIdx ) + in.getRowLabel( rowIdx ) );
        out.setRowLabel( idx,
            in.getColLabel( colIdx ) + in.getRowLabel( rowIdx ) );
        ++colIdx;
        if( colIdx >= sCols )
        {
            colIdx = 0;
            ++rowIdx;
        }
    }

    for( unsigned int idx = sRows*sRows; idx < dRows; ++idx )
        out.setRowLabel( idx, "formatting" );

    out.setRowColLabel( "Result" );
}

static void validate( const CSV& data, const unsigned int baseRow=0 )
{
    unsigned int cols, rows;
    data.getDims( cols, rows );
    for( unsigned int idx=baseRow; idx < rows; ++idx )
    {
        bool valid( false ); // assume not valid
        for( unsigned int c=0; c < cols; ++c )
        {
            if( data( c, idx ) == -1. ) // If there's a non-excluded cell...
            {
                valid = true; // ...then this row is valid and we can stop looking.
                break;
            }
        }
        s_valid[ idx-baseRow ] = valid;
    }
}

static bool isSolved( const CSV& data, const unsigned int baseRow=0 )
{
    unsigned int cols, rows;
    data.getDims( cols, rows );

    IntSet colVec, rowVec;
    initIntSet( colVec, cols );
    initIntSet( rowVec, baseRow );

    for( unsigned int ridx=baseRow; ridx < rows; ++ridx )
    {
        for( unsigned int cidx=0; cidx < cols; ++cidx )
        {
            if( data( cidx, ridx ) != 1 )
            {
                if( !( hasInt( colVec, cidx ) ) ||
                    !( hasInt( rowVec, ridx-baseRow ) ) )
                    return( false );
                removeInt( colVec, cidx );
                removeInt( rowVec, ridx-baseRow );
            }
        }
    }

    return( checkIntSet( colVec ) && checkIntSet( rowVec ) );
}

static void idxConvert( const unsigned int dCol, const unsigned int dRow,
                const unsigned int sCols, const unsigned int sRows,
                unsigned int& csc, unsigned int& csr,
                unsigned int& rsc, unsigned int& rsr )
{
    // dCol,dRos denotes the cell in the computation matrix.
    // sCols and sRows denote the number of original matrix cols and rows.
    // Return:
    //   csc,csr -- cell in source matrix for this result column
    //   rsc,rsr -- cell in source matrix for this result row

    csc = dCol % sCols;
    csr = dCol / sCols;
    rsc = dRow % sRows;
    rsr = dRow / sRows;
}

static unsigned int idxInvert( const unsigned int sRows,
                const unsigned int c, const unsigned int r )
{
    // Given (c,r) a column and row in the original matrix of size (n,sRows),
    // return \c row, the row (or column) in the computation matrix for (c,r).
    return( r * sRows + c );
}




static void processStep1( const CSV& in, CSV& out )
{
    RTW_PROFILE( "Step 1" );

    unsigned int sCols, sRows;
    in.getDims( sCols, sRows );
    unsigned int dCols, dRows;
    out.getDims( dCols, dRows );

    const unsigned int baseRows( sRows * sRows );

    for( unsigned int rdx=0; rdx < baseRows; ++rdx )
    {
        for( unsigned int cdx=0; cdx < dCols; ++cdx )
        {
            unsigned int csc, csr;
            unsigned int rsc, rsr;
            idxConvert( cdx, rdx, sCols, sRows,
                csc, csr, rsc, rsr );
            if( ( csc == rsc ) || ( csr == rsr ) )
                continue;

            double cell( in( csc, csr ) + in( rsc, rsr ) );
            out( cdx, rdx ) = cell;

            double comp( in( csc, rsr ) + in( rsc, csr ) );
            double fmt( ( cell <= comp ) ? -1. : 1 );
            out( cdx, rdx+baseRows ) = fmt;
        }
    }
}

static bool processStep2( CSV& out, const unsigned int sCols, const unsigned int sRows )
{
    RTW_PROFILE( "Step 2" );

    unsigned int dCols, dRows;
    out.getDims( dCols, dRows );

    const unsigned int baseRows( sRows * sRows );

    IntSet colVec, rowVec;
    bool processed( false );
    for( unsigned int rdx=0; rdx < baseRows; ++rdx )
    {
        // Skip the current row if it's not valid.
        // Not valid means there are zero non-excluded cells.
        if( !( s_valid[ rdx ] ) )
            continue;

        unsigned int csc0, csr0;
        unsigned int rsc0, rsr0;
        idxConvert( 0, rdx, sCols, sRows,
            csc0, csr0, rsc0, rsr0 );

        initIntSet( colVec, sCols, rsc0 );
        initIntSet( rowVec, sRows, rsr0 );

        for( unsigned int cdx=0; cdx < dCols; ++cdx )
        {
            unsigned int csc, csr;
            unsigned int rsc, rsr;
            idxConvert( cdx, rdx, sCols, sRows,
                csc, csr, rsc, rsr );

            if( ( csc == rsc0 ) || ( csr == rsr0 ) )
                continue;

            if( out( cdx, rdx+baseRows ) == -1. )
            {
                removeInt( colVec, csc );
                removeInt( rowVec, csr );
            }
        }

        bool colOK( checkIntSet( colVec ) );
        bool rowOK( checkIntSet( rowVec ) );

        if( !colOK || !rowOK )
        {
            for( unsigned int cdx=0; cdx < dCols; ++cdx )
            {
                if( out( cdx, rdx+baseRows ) == -1. )
                {
                    out( cdx, rdx+baseRows ) = 1.;
                    out( rdx, cdx+baseRows ) = 1.;
                    processed = true;
                }
            }
        }
    }

    return( processed );
}

static bool processStep3( CSV& out, const unsigned int sCols, const unsigned int sRows )
{
    RTW_PROFILE( "Step 3" );

    unsigned int dCols, dRows;
    out.getDims( dCols, dRows );

    const unsigned int baseRows( sRows * sRows );

    IntSet item0Cols, item0Rows, item1Cols, item1Rows;
    IntSet dontCheckCols, dontCheckRows;
    bool processed( false );
    unsigned int exclusionCount( 0 );
    for( unsigned int rdx=0; rdx < baseRows; ++rdx )
    {
        // Skip the current row if it's not valid.
        // Not valid means there are zero non-excluded cells.
        if( !( s_valid[ rdx ] ) )
            continue;

        // Find first non-excluded cell in this row.
        for( unsigned int cdx=0; cdx < dCols; ++cdx )
        {
            if( out( cdx, rdx+baseRows ) != -1 )
                continue;
            // Now, (cdx,rdx) is a non-null/non-excluded cell.

            unsigned int csc0, csr0;
            unsigned int rsc0, rsr0;
            idxConvert( cdx, rdx, sCols, sRows,
                csc0, csr0, rsc0, rsr0 );

            initIntSet( item0Cols, sCols, rsc0 );
            removeInt( item0Cols, csc0 );
            item1Cols = item0Cols;
            initIntSet( item0Rows, sRows, rsr0 );
            removeInt( item0Rows, csr0 );
            item1Rows = item0Rows;
            // rowVec and colVec are now the lists of rows and columns
            // (orders and vehicles) in the original matrix that must contain
            // the non-excluded cells for (csc0,csr0) and (rsc0,rsr0).

            dontCheckCols.clear();
            dontCheckCols.insert( rsc0 );
            dontCheckCols.insert( csc0 );
            dontCheckRows.clear();
            dontCheckRows.insert( rsr0 );
            dontCheckRows.insert( csr0 );

            // Invert (csc0,csr0) and (rsc0,rsr0) into the columns of the
            // cells we need to check.
            unsigned int item0Col( idxInvert( sRows, csc0, csr0 ) );
            unsigned int item1Col( idxInvert( sRows, rsc0, rsr0 ) );

            for( unsigned int check=0; check < baseRows; ++check )
            {
                if( !( s_valid[ check ] ) )
                    continue;

                unsigned int colA, rowA, colB, rowB;
                idxConvert( 0, check, sCols, sRows,
                    colA, rowA, colB, rowB );

                if( hasInt( dontCheckCols, colB ) ||
                    hasInt( dontCheckRows, rowB ) )
                    continue;

                if( hasInt( item0Cols, colB ) || hasInt( item0Rows, rowB ) )
                {
                    if( out( item0Col, check+baseRows ) != 1 )
                    {
                        removeInt( item0Cols, colB );
                        removeInt( item0Rows, rowB );
                    }
                }
                if( hasInt( item1Cols, colB ) || hasInt( item1Rows, rowB ) )
                {
                    if( out( item1Col, check+baseRows ) != 1 )
                    {
                        removeInt( item1Cols, colB );
                        removeInt( item1Rows, rowB );
                    }
                }
            }

            if( checkIntSet( item0Cols ) && checkIntSet( item0Rows ) &&
                checkIntSet( item1Cols ) && checkIntSet( item1Rows ) )
                // Each component of the non-excluded pair was part of a
                // pair for all other columns and rows (vehicles and orders).
                // Do not exclude this entry.
                continue;

            // At least one element of the non-excluded pair was not found to
            // be part of a pair for some vehicle(column) or order(row). Exclude it.
#ifdef VERBOSE
            std::cout << "\tExcluding (" << cdx+1 << "," << rdx+1 << ") because ";
            if( item0Cols.size() > 0 )
                std::cout << "vehicle(col) " << *(item0Cols.begin())+1 << " lacks a pair for (" <<
                csc0+1 << "," << csr0+1 << ")." << std::endl;
            else if( item0Rows.size() > 0 )
                std::cout << "order(row) " << *(item0Rows.begin())+1 << " lacks a pair for (" <<
                csc0+1 << "," << csr0+1 << ")." << std::endl;
            else if( item1Cols.size() > 0 )
                std::cout << "vehicle(col) " << *(item1Cols.begin())+1 << " lacks a pair for (" <<
                rsc0+1 << "," << rsr0+1 << ")." << std::endl;
            else if( item1Rows.size() > 0 )
                std::cout << "order(row) " << *(item1Rows.begin())+1 << " lacks a pair for (" <<
                rsc0+1 << "," << rsr0+1 << ")." << std::endl;
#endif
            out( cdx, rdx+baseRows ) = 1;
            processed = true;
            ++exclusionCount;
        }
    }

    return( processed );
}


//#define NO_TEXAS

static unsigned int totalSums( 0 );
static unsigned int killedByTexas( 0 );


static double bestSum( DBL_MAX );

typedef std::pair< unsigned int, unsigned int > UIntPair;
typedef std::vector< UIntPair > UIntPairVec;
typedef std::vector< UIntPairVec > UIntPairVecVec;
static UIntPairVecVec solutions;
static UIntPairVecVec allSolutions;


// Returns true if passes (and still a potential solution),
// false if fails (and not a solution).
static bool texas1Step( const CSV& comp, const unsigned int sRows,
                       const UIntPairVec& solution,
                       const unsigned int candCol, const unsigned int candRow )
{
    unsigned int compCols, compRows;
    comp.getDims( compCols, compRows );
    const unsigned int compBaseRows( compRows/2 );

    const unsigned int testRow( idxInvert( sRows, candCol, candRow ) );

    for( unsigned int idx=0; idx < solution.size(); ++idx )
    {
        const unsigned int testCol( idxInvert( sRows, solution[ idx ].first, solution[ idx ].second ) );
        if( comp( testCol, testRow + compBaseRows ) == 1 )
            return( false );
    }

    return( true );
}

static void recurseBruteForce( CSV& result, const CSV& computation,
        const unsigned int currentRow, IntSet& processedCols,
        UIntPairVec& solution )
{
    unsigned int rCols, rRows;
    result.getDims( rCols, rRows );
    const unsigned int baseRows( rRows/2 );

    // Have we found a solution?
    if( ( currentRow >= baseRows ) &&
        ( processedCols.size() >= rCols ) )
    {
        ++totalSums;
        allSolutions.push_back( solution );

        double sum( 0. );
        for( unsigned int idx=0; idx < rCols; ++idx )
            sum += result( solution[ idx ].first, solution[ idx ].second );
        if( sum <= bestSum )
        {
            if( sum < bestSum )
            {
                bestSum = sum;
                solutions.clear();
            }
            solutions.push_back( solution );
        }
        return;
    }

    // Find candidate columns in this row.
    UIntSimpleVec candidateCols;
    for( unsigned int idx=0; idx < rCols; ++idx )
    {
        if( ( processedCols.find( idx ) == processedCols.end() ) &&
            ( result( idx, currentRow + baseRows ) != 1 ) )
        {
#ifndef NO_TEXAS
            if( !( texas1Step( computation, baseRows, solution, idx, currentRow ) ) )
            {
                ++killedByTexas;
                //std::cout << "\tTexas 1-step eliminated a solution." << std::endl;
                continue;
            }
#endif
            candidateCols.push_back( idx );
        }
    }
    if( candidateCols.empty() )
        return;

    UIntPair cell;
    cell.second = currentRow;
    for( unsigned int idx=0; idx < candidateCols.size(); ++idx )
    {
        const unsigned int col( candidateCols[ idx ] );
        processedCols.insert( col );
        cell.first = col;
        solution.push_back( cell );

        recurseBruteForce( result, computation,
            currentRow+1, processedCols,
            solution );

        solution.pop_back();
        processedCols.erase( processedCols.find( col ) );
    }
}

static void bruteForce( CSV& result, const CSV& computation, const std::string& outFile=std::string("") )
{
    {
        RTW_PROFILE ( "Texas" );

        unsigned int currentRow( 0 );
        IntSet processedCols;
        UIntPairVec solution;

        recurseBruteForce( result, computation,
            currentRow, processedCols, solution );
    }

    if( outFile.empty() )
        return;

    unsigned int cols, rows;
    result.getDims( cols, rows );
    rows /= 2;

    CSV entryValues( cols, rows );
    for( unsigned int idx=0; idx < allSolutions.size(); ++idx )
    {
        const UIntPairVec& cand( allSolutions[ idx ] );
        for( unsigned int pIdx=0; pIdx < cand.size(); ++pIdx )
        {
            const UIntPair p( cand[ pIdx ] );
            entryValues( p.first, p.second ) += 1;
        }
    }
    entryValues.write( outFile + "-entryvalues.csv" );

    double newSum( 0 );
    UIntPairVecVec newSolutions;
    for( unsigned int idx=0; idx < allSolutions.size(); ++idx )
    {
        double sum( 0. );
        const UIntPairVec& cand( allSolutions[ idx ] );
        for( unsigned int pIdx=0; pIdx < cand.size(); ++pIdx )
        {
            const UIntPair p( cand[ pIdx ] );
            sum += entryValues( p.first, p.second );
        }
        if( sum >= newSum )
        {
            if( sum > newSum )
            {
                newSum = sum;
                newSolutions.clear();
            }
            newSolutions.push_back( cand );
        }
    }

    std::cout << "Best entry value sum: " << newSum << std::endl;
    std::cout << "Solutions with best entry value sum: " << newSolutions.size() << std::endl;
    for( unsigned int idx=0; idx < newSolutions.size(); ++idx )
    {
        std::cout << "\t";
        const UIntPairVec& cand( allSolutions[ idx ] );
        for( unsigned int pIdx=0; pIdx < cand.size(); ++pIdx )
        {
            const UIntPair p( cand[ pIdx ] );
            std::cout << "(" << p.first+1 << "," << p.second+1 << ") ";
        }
        std::cout << std::endl;
    }
}

static void intermediateDump( CSV& final, const CSV& result )
{
    unsigned int dCols, dRows;
    final.getDims( dCols, dRows );

    // Re-allocate space to hold formatting codes.
    final.resize( dCols, dRows*2 );
    // Add "formatting" as row label for new rows
    for( unsigned int idx = dRows; idx < dRows * 2; ++idx )
        final.setRowLabel( idx, "formatting" );

    unsigned int wCol( 0 );
    unsigned int wRow( dRows );
    for( unsigned int idx = dRows*dRows; idx < dRows*dRows*2; ++idx )
    {
        bool red( true );
        for( unsigned int c=0; c < dCols*dCols; ++c )
        {
            if( result( c, idx ) == -1 )
            {
                red = false;
                break;
            }
        }
        if( red )
            final( wCol, wRow ) = 1;
        ++wCol;
        if( wCol >= dCols )
        {
            wCol = 0;
            ++wRow;
        }
    }
}

static void writeSolution( CSV& final, const std::string& baseName )
{
    if( noMat ) return;

    for( unsigned int s=0; s < solutions.size(); ++s )
    {
        const UIntPairVec& solN( solutions[ s ] );

        unsigned int rCols, rRows;
        final.getDims( rCols, rRows );
        const unsigned int baseRows( rRows / 2 );

        // Add color coding to result matrix.
        for( unsigned int idx=0; idx < solN.size(); ++idx )
        {
            const unsigned int c( solN[ idx ].first );
            const unsigned int r( solN[ idx ].second );
            for( unsigned int cdx=0; cdx < rCols; ++cdx )
                final( cdx, r + baseRows ) = ( cdx == c ) ? 0 : 1;
        }

        std::ostringstream ostr;
        ostr << "-sol-" << s << ".csv";
        final.write( baseName + ostr.str() );
    }
}



int main( int argc, char** argv )
{
    bpo::options_description desc( "Options" );
    desc.add_options()
        ( "help,h", "Help text" )
        ( "in,i", bpo::value< std::string >(), "Input matrix .csv file." )
        ( "out,o", bpo::value< std::string >(), "Optional path to where output files will be written." )
        ( "nomat", "Do not write matrices." )
    ;

    bpo::variables_map vm;
    bpo::store( bpo::parse_command_line( argc, argv, desc ), vm );
    bpo::notify( vm );

    if( vm.count( "help" ) > 0 )
    {
        std::cout << desc << std::endl;
        return( 0 );
    }

    noMat = vm.count( "nomat" ) > 0;


    std::string inFile, outPath;
    if( vm.count( "in" ) > 0 )
        inFile = vm[ "in" ].as< std::string >();
    if( vm.count( "out" ) > 0 )
        outPath = vm[ "out" ].as< std::string >();

    if( inFile.empty() )
    {
        std::cerr << "Must specify --in/-i <csvFile> [--out/-o <csvFile>]" << std::endl;
        std::cerr << desc << std::endl;
        return( 1 );
    }


    std::cout << "input file: " << inFile << std::endl;
    std::cout << "output path: " << outPath << std::endl;

    // Set up output base file name.
    // Full file name is: "<base><n>.csv"
    // Base is: "<outPath>{\|/]result"
    // Examples:
    //    <...path...>/result0.csv
    //    <...path...>/result1.csv
    const bool forwardSlash( outPath.find_last_of( '/' ) != outPath.npos );
    const bool backSlash( outPath.find_last_of( '\\' ) != outPath.npos );
    std::string outFile( outPath );
    if( forwardSlash ) outFile += "/";
    else if( backSlash ) outFile += "\\";
    outFile += "result";


    // Load the input.
    CSV inCSV( inFile );
    if( !( inCSV.valid() ) )
    {
        std::cerr << "Failed to load " << inFile << std::endl;
        return( 1 );
    }


    // Prepare the first result matrix.
    unsigned int cols, rows;
    inCSV.getDims( cols, rows );

    const unsigned int rCols( cols*cols );
    const unsigned int rRows( rows*rows );
    CSV resultCSV( rCols, rRows*2 );


    // Set labels for output CSV.
    label( inCSV, resultCSV );

    // Init valid status. Initially 'true'.
    // 'false' indicates all pairings for a row are excluded.
    s_valid.resize( rRows, true );


    //
    // First processing step.
    processStep1( inCSV, resultCSV );
    if( !noMat )
        resultCSV.write( outFile + "-1.csv" );

    validate( resultCSV, rRows );

    //
    // Create intermediate output CSV.
    {
        CSV final( inCSV );
        intermediateDump( final, resultCSV );
        if( !noMat )
            final.write( outFile + "-1-final.csv" );

        if( isSolved( final, rows ) )
        {
            std::cout << "Solved after step 1." << std::endl;
            return( 0 );
        }
    }


    CSV workingCSV( resultCSV );


    unsigned int loop( 0 );
    const unsigned int maxLoop( 5 );
    bool continueLoop;
    do {
        std::ostringstream ostr;
        ostr << "Loop " << loop;
        RTW_PROFILE( ostr.str().c_str() );

        continueLoop = false;

        //
        // Second (iterative) processing step.
        unsigned int limit( 15 );
        unsigned int count( 0 );

        bool processed;
        do {
            std::cout << "Loop " << loop << " step 2, iteration " << count << std::endl;

            processed = processStep2( workingCSV, cols, rows );
            if( processed )
            {
                std::ostringstream ostr;
                ostr << outFile << "-loop" << loop << "-2-" << count << ".csv";
                if( !noMat )
                    workingCSV.write( ostr.str() );
                continueLoop = true;
            }

            CSV final( inCSV );
            intermediateDump( final, workingCSV );

            if( isSolved( final, rows ) )
                break;
        } while( processed && ( count++ < limit ) );

        validate( workingCSV, rRows );

        //
        // Create intermediate output CSV.
        {
            CSV final( inCSV );
            intermediateDump( final, workingCSV );
            {
                std::ostringstream ostr;
                ostr << outFile << "-loop" << loop << "-2-final.csv";
                if( !noMat )
                    final.write( ostr.str() );
            }

            if( isSolved( final, rows ) )
            {
                std::cout << "Solved after loop " << loop << " step 2." << std::endl;
                return( 0 );
            }
        }


        //
        // Third (interactive) processing step.
        count = 0;
        do {
            std::cout << "Loop " << loop << " step 3, iteration " << count << std::endl;

            processed = processStep3( workingCSV, cols, rows );
            if( processed )
            {
                std::ostringstream ostr;
                ostr << outFile << "-loop" << loop << "-3-" << count << ".csv";
                if( !noMat )
                    workingCSV.write( ostr.str() );
                continueLoop = true;
            }

            CSV final( inCSV );
            intermediateDump( final, workingCSV );

            if( isSolved( final, rows ) )
                break;
        } while( processed && ( count++ < limit ) );

        validate( workingCSV, rRows );

        //
        // Create final output CSV.
        {
            CSV final( inCSV );
            intermediateDump( final, workingCSV );
            {
                std::ostringstream ostr;
                ostr << outFile << "-loop" << loop << "-3-final.csv";
                if( !noMat )
                    final.write( ostr.str() );
            }

            if( isSolved( final, rows ) )
            {
                std::cout << "Solved after loop " << loop << " step 3." << std::endl;
                return( 0 );
            }
        }

    } while( continueLoop &&
        ( loop++ < maxLoop ) );


    //
    // Final brute-force evaluation of remaining possible solutions.
    std::cout << "Texas 1-step eval" << std::endl;
    CSV result( inCSV );
    intermediateDump( result, workingCSV );
#ifdef PERFORM_ENTRY_OCCURRANCE_TEST
    bruteForce( result, workingCSV, outFile );
#else
    bruteForce( result, workingCSV );
#endif

    std::cout << std::endl;
    std::cout << "Solution sum: " << bestSum << std::endl;
    std::cout << "Number of solutions: " << solutions.size() << std::endl;
    if( !noMat )
        writeSolution( result, outFile );

#ifdef NO_TEXAS
    std::cout << "Total sums without Texas: " << totalSums << std::endl;
#else
    std::cout << "Total sums with Texas: " << totalSums << std::endl;
    std::cout << "Texas eliminated " << killedByTexas << " unfruitful evaluations." << std::endl;
#endif

    ProfileManager::instance()->dumpAll();


    return( 0 );
}
