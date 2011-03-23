#ifndef _Common_Typedefs_H_
#define _Common_Typedefs_H_

#include <string>
#include <list>
#include <vector>
#include <map>
#include <queue>


// vmmlib math 
#include "VMMLibIncludes.h"
#include "rt_struct.h"

namespace stream_process
{
    
typedef std::vector< std::string > stringVector;
typedef std::vector< stringVector > stringVectorVector; // :) 
typedef std::map< const std::string, size_t > stringSizeMap;
typedef std::map< const std::string, stringVector > stringVectorMap;
typedef std::list< std::string > stringList;

//class Element;
//typedef std::map< const std::string, Element* > stringElementPtrMap;
//typedef std::list< Element* > ElementPtrList;

typedef std::pair< uint32_t , rt_struct* > ExtraRef; // index (first) of element (second)
typedef std::priority_queue< ExtraRef, std::vector< ExtraRef >, std::greater< ExtraRef > > ExtraQueue;

typedef std::pair< uint32_t , uint32_t > IndexReference; // smallest referenced index (first) from element (second)
typedef std::priority_queue< IndexReference, std::vector< IndexReference >, std::greater< IndexReference > > IndexQueue;

//typedef std::list< Element > ElementList;

} // namespace stream_process

#endif
