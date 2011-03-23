#ifndef __STREAM_PROCESS__BATCH_CONVERTER__HPP__
#define __STREAM_PROCESS__BATCH_CONVERTER__HPP__

#include <cstddef>

namespace stream_process
{

template< typename sp_types_t >
class batch_converter : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES

    virtual ~batch_converter() {}
    virtual void convert( char* input_data, stream_container_type& outputs ) const = 0;

    virtual void add_offsets( size_t in_offset, size_t out_offset )
    {
        _offsets.push_back( std::pair< size_t, size_t >( in_offset, out_offset ) );
    }

    virtual void set_input_point_size_in_bytes( size_t in_size )
    {
        _input_point_size_in_bytes = in_size;
    }
    
    virtual bool empty()
    {
        return _offsets.empty();
    }
    
protected:
    size_t                  _input_point_size_in_bytes;

    std::vector< std::pair< size_t, size_t > >   _offsets;
    
}; // class batch_converter



template< typename sp_types_t >
class batch_copy_converter : public batch_converter< sp_types_t >
{
public:
    typedef batch_converter< sp_types_t >       super;

    STREAM_PROCESS_TYPES

    batch_copy_converter( size_t element_size_in_bytes )
        : _element_size_in_bytes( element_size_in_bytes )
    {}
    
    
    virtual void convert( char* input_data, stream_container_type& outputs ) const
    {
        typename stream_container_type::iterator
            it      = outputs.begin(),
            it_end  = outputs.end();
        for( ; it != it_end; ++it, input_data += super::_input_point_size_in_bytes )
        {
            std::vector< std::pair< size_t, size_t > >::const_iterator
                offset_it       = super::_offsets.begin(), 
                offset_it_end   = super::_offsets.end();
                
            for( ; offset_it != offset_it_end; ++offset_it )
            {
                const std::pair< size_t, size_t >& offsets = *offset_it;
                memcpy(
                    *it + offsets.second,
                    input_data + offsets.first,
                    _element_size_in_bytes
                );
            }
        }
    }
protected:
    size_t  _element_size_in_bytes;
    
private:
    batch_copy_converter() {};
};


template< typename sp_types_t, typename in_type, typename out_type >
class batch_converter_t : public batch_converter< sp_types_t >
{
public:
    typedef batch_converter< sp_types_t >       super;

    STREAM_PROCESS_TYPES

    virtual void convert( char* input_data, stream_container_type& outputs ) const
    {
        typename stream_container_type::iterator
            it      = outputs.begin(), 
            it_end  = outputs.end();
        for( ; it != it_end; ++it, input_data += super::_input_point_size_in_bytes )
        {
            std::vector< std::pair< size_t, size_t > >::const_iterator
                offset_it       = super::_offsets.begin(), 
                offset_it_end   = super::_offsets.end();
                
            for( ; offset_it != offset_it_end; ++offset_it )
            {
                const std::pair< size_t, size_t >& offsets = *offset_it;
                const in_type& in_data
                    = *reinterpret_cast< in_type* >( input_data + offsets.first ); 
                out_type& out_data
                    = *reinterpret_cast< out_type* >( *it + offsets.second );

                out_data = static_cast< out_type >( in_data );
            }
        }
    }

};


inline void swap_byte_sex( uint16_t& us)
{
    us = (us >> 8) |
         (us << 8);
}



inline void swap_byte_sex( uint32_t& ui)
{
    ui = (ui >> 24) |
         ((ui<<8) && 0x00FF0000) |
         ((ui>>8) && 0x0000FF00) |
         (ui << 24);
}



inline void swap_byte_sex( uint64_t& ull)
{
    ull = (ull >> 56) |
          ((ull<<40) && 0x00FF000000000000ull) |
          ((ull<<24) && 0x0000FF0000000000ull) |
          ((ull<<8) && 0x000000FF00000000ull) |
          ((ull>>8) && 0x00000000FF000000ull) |
          ((ull>>24) && 0x0000000000FF0000ull) |
          ((ull>>40) && 0x000000000000FF00ull) |
          (ull << 56);
}


template< typename sp_types_t, typename in_type, typename out_type >
class endian_16_batch_converter_t : public batch_converter< sp_types_t >
{
public:
    typedef batch_converter< sp_types_t >       super;

    STREAM_PROCESS_TYPES

    virtual void convert( char* input_data, stream_container_type& outputs ) const
    {
        typename stream_container_type::iterator
            it      = outputs.begin(),
            it_end  = outputs.end();
        for( ; it != it_end; ++it, input_data += super::_input_point_size_in_bytes )
        {
            std::vector< std::pair< size_t, size_t > >::const_iterator
                offset_it       = super::_offsets.begin(), 
                offset_it_end   = super::_offsets.end();
                
            for( ; offset_it != offset_it_end; ++offset_it )
            {
                const std::pair< size_t, size_t >& offsets = *offset_it;
                uint16_t src_data
                    = *reinterpret_cast< uint16_t* >( input_data + offsets.first );

                swap_byte_sex( src_data );
                
                const in_type& endian_converted 
                    = *reinterpret_cast< in_type* >( &src_data );

                out_type& out_data
                    = *reinterpret_cast< out_type* >( *it + offsets.second );

                out_data = static_cast< out_type >( endian_converted );
            }
        }
    }

};



template< typename sp_types_t, typename in_type, typename out_type >
class endian_32_batch_converter_t : public batch_converter< sp_types_t >
{
public:
    typedef batch_converter< sp_types_t >       super;

    STREAM_PROCESS_TYPES
    
    virtual void convert( char* input_data, stream_container_type& outputs ) const
    {
        typename stream_container_type::iterator
            it      = outputs.begin(),
            it_end  = outputs.end();
        for( ; it != it_end; ++it, input_data += super::_input_point_size_in_bytes )
        {
            std::vector< std::pair< size_t, size_t > >::const_iterator
                offset_it       = super::_offsets.begin(), 
                offset_it_end   = super::_offsets.end();
                
            for( ; offset_it != offset_it_end; ++offset_it )
            {
                const std::pair< size_t, size_t >& offsets = *offset_it;

                uint32_t src_data
                    = *reinterpret_cast< uint32_t* >( input_data + offsets.first );

                swap_byte_sex( src_data );
                
                const in_type& endian_converted 
                    = *reinterpret_cast< in_type* >( &src_data );


                out_type& out_data
                    = *reinterpret_cast< out_type* >( *it + offsets.second );

                out_data = static_cast< out_type >( endian_converted );
            }
        }
    }

};



template< typename sp_types_t, typename in_type, typename out_type >
class endian_64_batch_converter_t : public batch_converter< sp_types_t >
{
public:
    typedef batch_converter< sp_types_t >       super;

    STREAM_PROCESS_TYPES

    virtual void convert( char* input_data, stream_container_type& outputs ) const
    {
        typename stream_container_type::iterator
            it      = outputs.begin(), 
            it_end  = outputs.end();
        for( ; it != it_end; ++it, input_data += super::_input_point_size_in_bytes )
        {
            std::vector< std::pair< size_t, size_t > >::const_iterator
                offset_it       = super::_offsets.begin(), 
                offset_it_end   = super::_offsets.end();
                
            for( ; offset_it != offset_it_end; ++offset_it )
            {
                const std::pair< size_t, size_t >& offsets = *offset_it;

                uint64_t src_data
                    = *reinterpret_cast< uint64_t* >( input_data + offsets.first );

                swap_byte_sex( src_data );
                
                const in_type& endian_converted 
                    = *reinterpret_cast< in_type* >( &src_data );

                out_type& out_data
                    = *reinterpret_cast< out_type* >( *it + offsets.second );

                out_data = static_cast< out_type >( endian_converted );
            }
        }
    }

};



} // namespace stream_process

#endif

