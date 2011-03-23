#ifndef __STREAM_PROCESS__THREADING_FUNCTOR__HPP__
#define __STREAM_PROCESS__THREADING_FUNCTOR__HPP__


#include "exception.hpp"


namespace stream_process
{

/**
*
*   boost::thread will make a copy of the functor-instance passed as 
*   ctor-parameter using the copy-constructor.However, some classes might not 
*   be copy-constructable. Using this class avoids this problem.
*
*/

template< typename T >
class threading_functor
{
public:
    threading_functor( T* wrapped_instance );
    void operator()();

protected:
    T* _wrapped_instance;
private:
    threading_functor() {};

}; // class threading_functor



template< typename T >
threading_functor< T >::threading_functor( T* wrapped_instance )
    : _wrapped_instance( wrapped_instance )
{
    if ( _wrapped_instance == 0 )
    {
        throw exception( "passed nullptr to threading_functor ctor.", 
            SPROCESS_HERE );
    }
}


template< typename T >
void
threading_functor< T >::operator()()
{
    // forward the call to the wrapped instance
    (*_wrapped_instance)();
}


} // namespace stream_process

#endif

