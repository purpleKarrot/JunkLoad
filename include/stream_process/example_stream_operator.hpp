#ifndef __STREAM_PROCESS__EXAMPLE_STREAM_OPERATOR__HPP__
#define __STREAM_PROCESS__EXAMPLE_STREAM_OPERATOR__HPP__

#include <stream_process/stream_operator.hpp>
#include <stream_process/attribute_accessor.hpp>

namespace stream_process
{

template< typename operator_base_t >
class example_stream_operator : public operator_base_t
{
public:
    typedef operator_base_t                 super;
    typedef typename super::sp_types_t      sp_types_t;

    STREAM_PROCESS_TYPES   
    
    example_stream_operator();
    
    virtual void push( slice_type* data_slice_ );

    virtual void setup_attributes();
    virtual void setup_accessors();
    
    virtual operator_base_t* clone();
    
protected:
    attribute_accessor< vec3 >			get_position;
	attribute_accessor< sp_float_type > get_counter;
    
    size_t								_counter;
    
}; // class example_stream_operator



template< typename operator_base_t >
example_stream_operator< operator_base_t >::
example_stream_operator()
    : operator_base_t()
    , _counter( 0 )
{}



template< typename operator_base_t >
void
example_stream_operator< operator_base_t >::
setup_attributes()
{
    point_structure& ps = super::_stream_config->get_vertex_structure();
	super::read( ps, "position", get_position );
	super::write( ps, "counter", get_counter, false );
}



template< typename operator_base_t >
void
example_stream_operator< operator_base_t >::
setup_accessors()
{
	get_position.setup();
	get_counter.setup();
}


template< typename operator_base_t >
void
example_stream_operator< operator_base_t >::
push( slice_type* data_slice_ )
{
    typename slice_type::iterator 
        it      = data_slice_->begin(),
        it_end  = data_slice_->end();
    for( ; it != it_end; ++it )
    {
        const vec3& position    = get_position( *it );
        sp_float_type& counter  = get_counter( *it );

        if ( _counter % 10000 == 0 )
        {
            std::cout << "position " << position << std::endl;
            std::cout << "counter " << counter << std::endl;
        }
        counter = _counter++;
    }
    super::_out_buffer.push_back( data_slice_ );
}


template< typename operator_base_t >
typename example_stream_operator< operator_base_t >::super*
example_stream_operator< operator_base_t >::
clone()
{
    return new example_stream_operator( *this );
}

} // namespace stream_process

#endif

