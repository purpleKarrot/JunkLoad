#ifndef __STREAM_PROCESS__FACE_ELEMENT_READER__HPP__
#define __STREAM_PROCESS__FACE_ELEMENT_READER__HPP__

#include <stream_process/element_reader.hpp>
#include <stream_process/vertex_indexer.hpp>

namespace stream_process
{
template< typename sp_types_t >
class face_element_reader : public element_reader< sp_types_t >
{
public:
    STREAM_PROCESS_TYPES
    
    typedef element_reader< sp_types_t >    super;
    typedef vertex_indexer< sp_types_t >    vertex_indexer_type;

    face_element_reader( 
        vertex_indexer_type& vertex_indexer, 
        data_element& data_element_,
        const std::string& filename_base );

protected:
    vertex_indexer_type&    _vertex_indexer;

}; // class face_element_reader


template< typename sp_types_t >
face_element_reader< sp_types_t >::
face_element_reader( vertex_indexer_type& vertex_indexer_,
    data_element& data_element_, const std::string& filename_base )
    : super( data_element_, filename_base )
    , _vertex_indexer( vertex_indexer_ )
{}


} // namespace stream_process

#endif

