#ifndef __VMML_RT_STRUCT_H__
#define __VMML_RT_STRUCT_H__

#include <iostream>

/**
 * @brief a dynamic data class 
 *
 * an instance of this object can have an arbitrary number of arbitrary 
 * data members. the only limitation atm: the object MUST NOT depend on its 
 * ctor/dtor, since the constructor and destructor will NOT be called.
 * this has some drawbacks, but it allows to efficiently allocate large
 * pools of dynamic objects. 
 * 
 * construction of dynamic data instances must be done using  
 * an instance of the dynamic data factory
 *
 * @author Jonas Boesch
 *
 */
  
namespace stream_process
{

template< typename T > class rt_struct_member;

//typedef char rt_struct;
struct rt_struct
{
    template< typename T >
    T& get( const rt_struct_member< T >& member )
    {
        //std::cout << member._name << " " << member._offset << std::endl;
        return reinterpret_cast< T& >( this[ member._offset ] );
    }


    template< typename T >
    const T& get( const rt_struct_member< T >& member ) const
    {
        //std::cout << member._name << " " << member._offset << std::endl;
        return reinterpret_cast< const T& >( this[ member._offset ] );
    }


    template< typename T >
    T* get_ptr( const rt_struct_member< T >& member )
    {
        return &reinterpret_cast< T& >( this[ member._offset ] );
    }


    template< typename T >
    const T* get_ptr( const rt_struct_member< T >& member ) const
    {
        return &reinterpret_cast< T& >( this[ member._offset ] );
    }


    template< typename T >
    void set( const rt_struct_member< T >& member, const T& value )
    {
        T& new_value = reinterpret_cast< T& >( this[ member._offset ] );
        new_value = value; // memcpy ? 
    }

private:
    rt_struct(){};
    rt_struct( const rt_struct& other ){};

}; // struct rt_struct


} // namespace stream_process

#endif
