// Copyright

#ifndef __SMSRAIL_TYPES_H__
#define __SMSRAIL_TYPES_H__ 1


#include <gmtl/gmtl.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <memory>


typedef gmtl::Point2d p2d;
typedef std::vector< p2d > p2dSimpleVec;
typedef gmtl::AABoxd aabox;

typedef std::vector< double > DoubleSimpleVec;
typedef std::vector< std::string > StringSimpleVec;
typedef std::vector< unsigned int > UIntSimpleVec;
typedef std::set< std::string > StringSet;

class BGR;
typedef std::shared_ptr< BGR > BGRPtr;
class Intersection;
typedef std::shared_ptr< Intersection > IntersectionPtr;
class Loader;
typedef std::shared_ptr< Loader > LoaderPtr;
class Road;
typedef std::shared_ptr< Road > RoadPtr;
class Spatial;
typedef std::shared_ptr< Spatial > SpatialPtr;
class Writer;
typedef std::shared_ptr< Writer > WriterPtr;


// __SMSRAIL_TYPES_H__
#endif
