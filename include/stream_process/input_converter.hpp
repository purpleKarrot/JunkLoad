#ifndef __STREAM_PROCESS__INPUT_CONVERTER__HPP__
#define __STREAM_PROCESS__INPUT_CONVERTER__HPP__

#include <stream_process/point_structure.hpp>
#include <stream_process/exception.hpp>

#include <stream_process/data_types.hpp>
#include <stream_process/batch_converter.hpp>
#include <stream_process/io_shared_data.hpp>

#include <stream_process/foreach.hpp>

#include <cstddef>
#include <map>
#include <vector>

namespace stream_process
{


template< typename sp_types_t >
class input_converter : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
    
    typedef batch_converter< sp_types >             batch_cv_type;
    typedef io_shared_data< sp_types >              io_shared_data_type;
    
    input_converter();

    void setup( point_structure& input_, point_structure& stream_, 
        bool endian_convert = false );
    
    void convert( char* input_, stream_container_type& output_ );
    
    void convert( char* input_, char* output, size_t elements );
    
    void set_io_shared_data( io_shared_data_type* io );
    
protected:
    batch_cv_type* get_converter( data_type_id src, data_type_id target, 
        bool requires_endian_cv );

    template< typename T >
    batch_cv_type* get_converter_t( data_type_id target );

    template< typename T >
    batch_cv_type* get_endian_16_converter_t( data_type_id target );

    template< typename T >
    batch_cv_type* get_endian_32_converter_t( data_type_id target );

    template< typename T >
    batch_cv_type* get_endian_64_converter_t( data_type_id target );
    
    std::vector< batch_cv_type* > _converters;

    typedef std::map<
        std::pair< data_type_id, data_type_id >,
        batch_cv_type*
        > batch_cv_map;

    batch_cv_map            _converters_by_types;
    io_shared_data_type*    _io_shared_data;

}; // class input_converter



template< typename sp_types_t >
input_converter< sp_types_t >::
input_converter()
{}


template< typename sp_types_t >
void
input_converter< sp_types_t >::
convert( char* input_, stream_container_type& output_ )
{
    foreach( const batch_cv_type* cv, _converters )
    {
        cv->convert( input_, output_ );
    }        
}





template< typename sp_types_t >
void
input_converter< sp_types_t >::
setup( point_structure& input_, point_structure& stream_,
    bool endian_cv )
{
    const size_t in_point_size_in_bytes = input_.compute_size_in_bytes();

    point_structure::const_iterator 
        it      = input_.begin(),
        it_end  = input_.end();
    for( ; it != it_end; ++it )
    {
        // get the attribute for input and stream point
        attribute& in_attr  = **it;
        attribute& attr     = stream_.get_attribute( in_attr.get_name() );
        
        data_type_id in_id      = in_attr.get_data_type_id();
        data_type_id stream_id  = attr.get_data_type_id();
        
        batch_cv_type*  batch_cv = 0;

        // search for a suitable converter, create if not existing yet.
        typename batch_cv_map::key_type key_( in_id, stream_id );
        typename batch_cv_map::iterator it = _converters_by_types.find( key_ );
        if ( it == _converters_by_types.end() )
        {
            batch_cv = get_converter( in_id, stream_id, endian_cv );
            if ( ! batch_cv )
            {
                batch_cv = new batch_copy_converter< sp_types >( in_attr.get_element_size_in_bytes() );
                if ( in_id != SP_UNKNOWN_DATA_TYPE )
                    _converters_by_types[ key_ ] = batch_cv;
            }
            else
            {
                _converters_by_types[ key_ ] = batch_cv;
            }
            assert( batch_cv );

            batch_cv->set_input_point_size_in_bytes( in_point_size_in_bytes );
            _converters.push_back( batch_cv );
        }
        else
        {
            batch_cv = it->second;
        }
        
        // add offsets to batch_cv
        size_t in_offset                = in_attr.get_offset();
        const size_t in_elem_size       = in_attr.get_element_size_in_bytes();

        size_t stream_offset            = attr.get_offset();
        const size_t stream_elem_size   = attr.get_element_size_in_bytes();
        const size_t number_of_elements = in_attr.get_number_of_elements();

        for( size_t index = 0; index < number_of_elements; ++index )
        {
            batch_cv->add_offsets( in_offset, stream_offset );
            in_offset       += in_elem_size;
            stream_offset   += stream_elem_size;
        }
    }

}


template< typename sp_types_t >
typename input_converter< sp_types_t >::batch_cv_type*
input_converter< sp_types_t >::
get_converter( data_type_id src, data_type_id target,
    bool requires_endian_cv )
{
    if ( requires_endian_cv )
    {
        switch( src )
        {
            case SP_FLOAT_32:
                return get_endian_32_converter_t< float >( target );
            case SP_FLOAT_64:
                return get_endian_64_converter_t< double >( target );
            case SP_UINT_8:
                return get_converter_t< uint8_t >( target );
            case SP_INT_8:
                return get_converter_t< int8_t >( target );
            case SP_UINT_16:
                return get_endian_16_converter_t< uint16_t >( target );
            case SP_INT_16:
                return get_endian_16_converter_t< int16_t >( target );
            case SP_UINT_32:
                return get_endian_32_converter_t< uint32_t >( target );
            case SP_INT_32:
                return get_endian_32_converter_t< int32_t >( target );
            case SP_UINT_64:
                return get_endian_64_converter_t< uint64_t >( target );
            case SP_INT_64:
                return get_endian_64_converter_t< int64_t >( target );
            default:
                return 0;
        }
    }
    else
    {
        switch( src )
        {
            case SP_FLOAT_32:
                return get_converter_t< float >( target );
            case SP_FLOAT_64:
                return get_converter_t< double >( target );
            case SP_UINT_8:
                return get_converter_t< uint8_t >( target );
            case SP_INT_8:
                return get_converter_t< int8_t >( target );
            case SP_UINT_16:
                return get_converter_t< uint16_t >( target );
            case SP_INT_16:
                return get_converter_t< int16_t >( target );
            case SP_UINT_32:
                return get_converter_t< uint32_t >( target );
            case SP_INT_32:
                return get_converter_t< int32_t >( target );
            case SP_UINT_64:
                return get_converter_t< uint64_t >( target );
            case SP_INT_64:
                return get_converter_t< int64_t >( target );
            default:
                return 0;
        }
    }

}



template< typename sp_types_t >
template< typename T >
typename input_converter< sp_types_t >::batch_cv_type*
input_converter< sp_types_t >::
get_converter_t( data_type_id target )
{
    switch( target )
    {
        case SP_FLOAT_32:
            return new batch_converter_t< sp_types_t, T, float >();
        case SP_FLOAT_64:
            return new batch_converter_t< sp_types_t, T, double >();
        case SP_UINT_8:
            return new batch_converter_t< sp_types_t, T, uint8_t >();
        case SP_INT_8:
            return new batch_converter_t< sp_types_t, T, int8_t >();
        case SP_UINT_16:
            return new batch_converter_t< sp_types_t, T, uint16_t >();
        case SP_INT_16:
            return new batch_converter_t< sp_types_t, T, int16_t >();
        case SP_UINT_32:
            return new batch_converter_t< sp_types_t, T, uint32_t >();
        case SP_INT_32:
            return new batch_converter_t< sp_types_t, T, int32_t >();
        case SP_UINT_64:
            return new batch_converter_t< sp_types_t, T, uint64_t >();
        case SP_INT_64:
            return new batch_converter_t< sp_types_t, T, int64_t >();
        default:
            return 0;
    }
}



template< typename sp_types_t >
template< typename T >
typename input_converter< sp_types_t >::batch_cv_type*
input_converter< sp_types_t >::
get_endian_16_converter_t( data_type_id target )
{
    switch( target )
    {
        case SP_UINT_16:
            return new endian_16_batch_converter_t< sp_types_t, T, uint16_t >();
        case SP_INT_16:
            return new endian_16_batch_converter_t< sp_types_t, T, int16_t >();
        default:
            throw exception(
                "attempt to endian convert between differently-sized values.", 
                SPROCESS_HERE 
                );
    }
}




template< typename sp_types_t >
template< typename T >
typename input_converter< sp_types_t >::batch_cv_type*
input_converter< sp_types_t >::
get_endian_32_converter_t( data_type_id target )
{
    switch( target )
    {
        case SP_FLOAT_32:
            return new endian_32_batch_converter_t< sp_types_t, T, float >();
        case SP_UINT_32:
            return new endian_32_batch_converter_t< sp_types_t, T, uint32_t >();
        case SP_INT_32:
            return new endian_32_batch_converter_t< sp_types_t, T, int32_t >();
        default:
            throw exception(
                "attempt to endian convert between differently-sized values.", 
                SPROCESS_HERE 
                );
    }
}


template< typename sp_types_t >
template< typename T >
typename input_converter< sp_types_t >::batch_cv_type*
input_converter< sp_types_t >::
get_endian_64_converter_t( data_type_id target )
{
    switch( target )
    {
        case SP_FLOAT_64:
            return new endian_64_batch_converter_t< sp_types_t, T, double >();
        case SP_UINT_64:
            return new endian_64_batch_converter_t< sp_types_t, T, uint64_t >();
        case SP_INT_64:
            return new endian_64_batch_converter_t< sp_types_t, T, int64_t >();
        default:
            throw exception(
                "attempt to endian convert between differently-sized values.", 
                SPROCESS_HERE 
                );
    }
}



template< typename sp_types_t >
void
input_converter< sp_types_t >::
set_io_shared_data( io_shared_data_type* io )
{
    _io_shared_data = io;
}


} // namespace stream_process

#endif

