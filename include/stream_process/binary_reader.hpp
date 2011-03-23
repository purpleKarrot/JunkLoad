#ifndef __STREAM_PROCESS__BINARY_READER__HPP__
#define __STREAM_PROCESS__BINARY_READER__HPP__

#include "rt_struct_member_info2.hpp"
#include "exception.hpp"
#include <cstdlib>

#include <arpa/inet.h>

namespace stream_process
{

struct binary_reader
{
    binary_reader( size_t read_size_ ) : read_size( read_size_ ) {}
    virtual ~binary_reader() {}
    
    virtual char* read( char* input, char* output ) = 0;
    size_t  read_size;
}; // class binary_reader



struct bin_reader : public binary_reader
{
    bin_reader( size_t array_size_, size_t element_size_in_bytes )
        : binary_reader( element_size_in_bytes * array_size_ )
        , array_size( array_size_ )
    {}
    
   virtual ~bin_reader() {}

    virtual char* read( char* input, char* output )
    {
        memcpy( output, input, read_size );
        return input + read_size;
    }
    size_t array_size;
};




struct endian_cv_reader_16 : public binary_reader
{
    endian_cv_reader_16( size_t array_size_ ) 
        : binary_reader( 2 * array_size_ )
        , array_size( array_size_ )
    {}
    
    virtual ~endian_cv_reader_16() {} 
        
    virtual char* read( char* input, char* output )
    {
        memcpy( output, input, read_size );
        for( size_t i = 0; i < array_size; ++i )
        {
            uint16_t& data = (uint16_t&) output[ 2 * i ];
            data = htons( data );
        }
        return input + read_size;
    }

    size_t array_size;
};




struct endian_cv_reader_32 : public binary_reader
{
    endian_cv_reader_32( size_t array_size_ ) 
        : binary_reader( 4 * array_size_ )
        , array_size( array_size_ )
    {}

    virtual ~endian_cv_reader_32() {} 

    virtual char* read( char* input, char* output )
    {
        memcpy( output, input, read_size );
        for( size_t i = 0; i < array_size; ++i )
        {
            uint32_t& data = (uint32_t&) output[ 4 * i ];
            data = htonl( data );
        }
        return input + read_size;
    }
    size_t array_size;
};

} // namespace stream_process

#endif

