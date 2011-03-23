#ifndef __STREAM_PROCESS__ATTRIBUTE_GETTER__HPP__
#define __STREAM_PROCESS__ATTRIBUTE_GETTER__HPP__

#include <stream_process/stream_data.hpp>
#include <stream_process/attribute.hpp>
#include <stream_process/attribute_type.hpp>
#include <stream_process/attribute_accessor.hpp>
#include <stream_process/data_types.hpp>

#include <boost/numeric/conversion/converter.hpp>


#if 0
#include <stream_process/point_structure.hpp>
#include <stream_process/exception.hpp>
#include <stream_process/VMMLibIncludes.h>
#include <stream_process/stream_data.hpp>
#endif

namespace stream_process
{

class attribute_getter
{
public:
    attribute_getter();
    attribute_getter( const attribute& attribute_ );

    void set_attribute( const attribute& attribute_ );
    
    template< typename T >
    T operator()( const stream_data* stream_data_ ) const;

    template< typename T >
    T get_as( const stream_data* stream_data_ ) const;

    template< typename T, size_t M >
    void get_as( vmml::vector< M, T >& vec_, const stream_data* stream_data_ ) const;

    template< typename T, size_t M, size_t N >
    void get_as( vmml::matrix< M, N, T >& mat_, const stream_data* stream_data_ ) const;
    
protected:
    template< typename Tin, typename Tout >
    Tout _convert( const stream_data* stream_data_ ) const;

    template< typename Tin, typename Tout >
    Tout _convert_value( const stream_data* stream_data_ ) const;

    template< typename Tin, typename Tout, size_t elements >
    void _convert_vector( vmml::vector< elements, Tout >& vec_,
        const stream_data* stream_data_ ) const;

    template< typename Tin, typename Tout, size_t M, size_t N >
    void _convert_matrix( vmml::matrix< M, N, Tout >& mat_, 
        const stream_data* stream_data_ ) const;

    const attribute*    _attribute;

}; // class attribute_getter


template< typename T >
T
attribute_getter::get_as( const stream_data* stream_data_ ) const
{
    return this->operator()< T >( stream_data_ );
}


template< typename T >
T
attribute_getter::operator()( const stream_data* stream_data_ ) const
{
    if ( _attribute == 0 )
    {
        throw exception( "Attempt to use attribute_getter with attribute nullpointer.", 
            SPROCESS_HERE );
    }

    data_type_id id_ = get_data_type_id_from_type< T >()();
    if ( id_ == _attribute->get_data_type_id() )
    {
        return attribute_accessor< T >::get_with_offset(
            _attribute->get_offset(), stream_data_ );
    }
    
    // we need to convert the data
    switch( _attribute->get_data_type_id() )
    {
        case SP_FLOAT_32:
            return _convert< float, T >( stream_data_ );
            break;
        case SP_FLOAT_64:
            return _convert< double, T >( stream_data_ );
            break;
        case SP_INT_8:
            return _convert< int8_t, T >( stream_data_ );
            break;
        case SP_UINT_8:
            return _convert< uint8_t, T >( stream_data_ );
            break;
        case SP_INT_16:
            return _convert< int16_t, T >( stream_data_ );
            break;
        case SP_UINT_16:
            return _convert< uint16_t, T >( stream_data_ );
            break;
        case SP_INT_32:
            return _convert< int32_t, T >( stream_data_ );
            break;
        case SP_UINT_32:
            return _convert< uint32_t, T >( stream_data_ );
            break;
        case SP_INT_64:
            return _convert< int64_t, T >( stream_data_ );
            break;
        case SP_UINT_64:
            return _convert< uint64_t, T >( stream_data_ );
            break;
        default:
            throw exception(
                "invalid type conversion request.", 
                SPROCESS_HERE
            );
            break;
    }
}



template< typename Tin, typename Tout >
Tout
attribute_getter::_convert( const stream_data* stream_data_ ) const
{
    return static_cast< Tout >( 
        attribute_accessor< Tin >::get_with_offset(
            _attribute->get_offset(), stream_data_ 
            )
        );
}



template< typename T, size_t M >
void
attribute_getter::get_as( vmml::vector< M, T >& vec_, const stream_data* stream_data_ ) const
{
    if ( _attribute == 0 )
    {
        throw exception( "Attempt to use attribute_getter with attribute nullpointer.", 
            SPROCESS_HERE );
    }
    if ( M != _attribute->get_number_of_elements() )
    {
        throw exception( "Attempt to get attribute data with mismatched types.", 
            SPROCESS_HERE );
    }

    data_type_id id_ = get_data_type_id_from_type< T >()();
    if ( id_ == _attribute->get_data_type_id() )
    {
        vec_ = attribute_accessor< vmml::vector< M, T > >::get_with_offset(
            _attribute->get_offset(), stream_data_ );
        return;
    }
    
    // we need to convert the data
    switch( _attribute->get_data_type_id() )
    {
        case SP_FLOAT_32:
            return _convert_vector< float, T, M >( vec_, stream_data_ );
            break;
        case SP_FLOAT_64:
            return _convert_vector< double, T, M >( vec_, stream_data_ );
            break;
        case SP_INT_8:
            return _convert_vector< int8_t, T, M >( vec_, stream_data_ );
            break;
        case SP_UINT_8:
            return _convert_vector< uint8_t, T, M >( vec_, stream_data_ );
            break;
        case SP_INT_16:
            return _convert_vector< int16_t, T, M >( vec_, stream_data_ );
            break;
        case SP_UINT_16:
            return _convert_vector< uint16_t, T, M >( vec_, stream_data_ );
            break;
        case SP_INT_32:
            return _convert_vector< int32_t, T, M >( vec_, stream_data_ );
            break;
        case SP_UINT_32:
            return _convert_vector< uint32_t, T, M >( vec_, stream_data_ );
            break;
        case SP_INT_64:
            return _convert_vector< int64_t, T, M >( vec_, stream_data_ );
            break;
        case SP_UINT_64:
            return _convert_vector< uint64_t, T, M >( vec_, stream_data_ );
            break;
        default:
            throw exception(
                "invalid type conversion request.", 
                SPROCESS_HERE
            );
            break;
    }


}



template< typename T, size_t M, size_t N >
void
attribute_getter::get_as( vmml::matrix< M, N, T >& mat_,
    const stream_data* stream_data_ ) const
{
    if ( _attribute == 0 )
    {
        throw exception( "Attempt to use attribute_getter with attribute nullpointer.", 
            SPROCESS_HERE );
    }
    if ( M * N != _attribute->get_number_of_elements() )
    {
        throw exception( "Attempt to get attribute data with mismatched types.", 
            SPROCESS_HERE );
    }

    data_type_id id_ = get_data_type_id_from_type< T >()();
    if ( id_ == _attribute->get_data_type_id() )
    {
        return attribute_accessor< vmml::matrix< M, N, T > >::get_with_offset(
            _attribute->get_offset(), stream_data_ );
    }
    
    // we need to convert the data
    switch( _attribute->get_data_type_id() )
    {
        case SP_FLOAT_32:
            return _convert_matrix< float, T, M, N >( mat_, stream_data_ );
            break;
        case SP_FLOAT_64:
            return _convert_matrix< double, T, M, N >( mat_, stream_data_ );
            break;
        case SP_INT_8:
            return _convert_matrix< int8_t, T, M, N >( mat_, stream_data_ );
            break;
        case SP_UINT_8:
            return _convert_matrix< uint8_t, T, M, N >( mat_, stream_data_ );
            break;
        case SP_INT_16:
            return _convert_matrix< int16_t, T, M, N >( mat_, stream_data_ );
            break;
        case SP_UINT_16:
            return _convert_matrix< uint16_t, T, M, N >( mat_, stream_data_ );
            break;
        case SP_INT_32:
            return _convert_matrix< int32_t, T, M, N >( mat_, stream_data_ );
            break;
        case SP_UINT_32:
            return _convert_matrix< uint32_t, T, M, N >( mat_, stream_data_ );
            break;
        case SP_INT_64:
            return _convert_matrix< int64_t, T, M, N >( mat_, stream_data_ );
            break;
        case SP_UINT_64:
            return _convert_matrix< uint64_t, T, M, N >( mat_, stream_data_ );
            break;
        default:
            throw exception(
                "invalid type conversion request.", 
                SPROCESS_HERE
            );
            break;
    }
}




template< typename Tin, typename Tout, size_t elements >
void
attribute_getter::_convert_vector( vmml::vector< elements, Tout >& vec_,
    const stream_data* stream_data_ ) const
{
    typedef vmml::vector< elements, Tin >   in_vector;
    typedef vmml::vector< elements, Tout >  out_vector;
    
    const in_vector& in_ = attribute_accessor< in_vector >::get_with_offset(
        _attribute->get_offset(), stream_data_ );
    
    boost::numeric::converter< Tin, Tout > cv;
    for( size_t index = 0; index < elements; ++index )
    {
        vec_.array[ index ] = cv( in_.array[ index ] );
    }
}



template< typename Tin, typename Tout, size_t M, size_t N >
void
attribute_getter::_convert_matrix( vmml::matrix< M, N, Tout >& mat_, 
    const stream_data* stream_data_ ) const
{
    typedef vmml::matrix< M, N, Tin >   in_matrix;
    typedef vmml::matrix< M, N, Tout >  out_matrix;
    
    const in_matrix& in_ = attribute_accessor< in_matrix >::get_with_offset(
        _attribute->get_offset(), stream_data_ );
    
    boost::numeric::converter< Tin, Tout > cv;
    const size_t s = M * N;
    for( size_t index = 0; index < s; ++index )
    {
        mat_.array[ index ] = cv( in_.array[ index ] );
    }
}


} // namespace stream_process

#endif
