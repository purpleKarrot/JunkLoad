#ifndef __STREAM_PROCESS__NEIGHBOR_STORE_STREAM_OP__HPP__
#define __STREAM_PROCESS__NEIGHBOR_STORE_STREAM_OP__HPP__


#include <stream_process/stream_process_types.hpp>

namespace stream_process
{
template< typename operator_base_t >
class neighbor_store_stream_op : public operator_base_t
{
public:
    typedef operator_base_t             super;
    typedef typename super::sp_types_t  sp_types_t;

    STREAM_PROCESS_TYPES
    
    neighbor_store_stream_op();
    
    virtual void push( slice_type* slice );

    virtual void setup_finalize();
    virtual void setup_attributes();
    virtual void setup_accessors();
    
    virtual bool needs_bounds_checking() const  { return false; }
    virtual bool is_multi_threadable() const    { return false; }
    
    virtual super* clone();
    
protected:
	attribute_accessor< vec3 >			_get_position;
	attribute_accessor< nbh_type >      _get_neighbors;
	attribute_accessor< size_t >        _get_point_index;

    attribute_accessor< sp_float_type > _get_nb_distances;
    attribute_accessor< size_t >        _get_nb_indices;
    
    size_t  _max_k;
    

}; // class neighbor_store_stream_op


template< typename operator_base_t >
neighbor_store_stream_op< operator_base_t >::
neighbor_store_stream_op()
    : _max_k( 0 )
{}



template< typename operator_base_t >
void
neighbor_store_stream_op< operator_base_t >::
setup_finalize()
{
    stream_options& opts = super::_stream_config->get_options();
    _max_k = opts.get( "number-of-neighbors" ).get_int();
}


template< typename operator_base_t >
void
neighbor_store_stream_op< operator_base_t >::
setup_attributes()
{
    point_structure& ps = super::_stream_config->get_vertex_structure();
    
    super::read( ps, "position", _get_position );
    super::read( ps, "neighbors", _get_neighbors );
    super::read( ps, "point_index", _get_point_index );
    
    super::write( ps, "nb_distances",   _get_nb_distances, true, _max_k );
    super::write( ps, "nb_indices",     _get_nb_indices, true, _max_k );
}



template< typename operator_base_t >
void
neighbor_store_stream_op< operator_base_t >::
setup_accessors()
{
    _get_position.setup();
    _get_neighbors.setup();
    _get_point_index.setup();
    _get_nb_distances.setup();
    _get_nb_indices.setup();
}




template< typename operator_base_t >
void
neighbor_store_stream_op< operator_base_t >::
push( slice_type* slice )
{
    const bool print_out = false;

    typename slice_type::iterator it = slice->begin(), it_end = slice->end();
    for( ; it != it_end; ++it )
    {
        stream_data* point  = *it;
        
        if ( print_out )
            std::cout << "point " << _get_point_index( point ) << ": ";
        
        nbh_type& nbh               = _get_neighbors( point );
        
        sp_float_type* distances    = & _get_nb_distances( point );
        size_t* indices             = & _get_nb_indices( point );
        
        typename nbh_type::iterator nit = nbh.begin(), nit_end = nbh.end();
        for( size_t index = 0; nit != nit_end; ++nit, ++index )
        {
            distances[ index ]  = (*nit).get_distance();
            indices[ index ]    = _get_point_index( (*nit).get_ptr() );
            if ( print_out )
            {
                std::cout
                    << indices[ index ] 
                    << ": " << distances[ index ]
                    << ", ";
            }
            
        }
        if ( print_out )
            std::cout << std::endl;
    }
    super::_out_buffer.push_back( slice );
}


template< typename operator_base_t >
typename neighbor_store_stream_op< operator_base_t >::super*
neighbor_store_stream_op< operator_base_t >::
clone()
{
    return new neighbor_store_stream_op( *this );
}



} // namespace stream_process

#endif

