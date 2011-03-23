#ifndef __STREAM_PROCESS__OP_CONTAINER__HPP__
#define __STREAM_PROCESS__OP_CONTAINER__HPP__

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>
#include <iomanip>
#include <cassert>

namespace stream_process
{

template< class operator_t >
class op_container
{
public:
    typedef typename std::vector< operator_t* >::iterator         iterator;
    typedef typename std::vector< operator_t* >::const_iterator   const_iterator;

    op_container();

    void setup_stage_0();
    void setup_stage_1();
    void setup_stage_2();

    void prepare_processing();
    void prepare_shutdown();

    operator_t* get_operator( const std::string& name );
    operator_t* get_operator( size_t index );

    void push_back( operator_t* op );

    std::string get_operator_timings_string() const;

    // iterators
    inline iterator begin();
    inline iterator end();
    inline const_iterator begin() const;
    inline const_iterator end() const;
    
    inline bool empty();

protected:
    std::vector< operator_t* >   _operators;
    std::map< std::string, size_t > _operators_by_name;

}; // class op_container



template< class operator_t >
op_container< operator_t >::op_container()
{}



template< class operator_t >
inline typename op_container< operator_t >::iterator
op_container< operator_t >::begin()
{
    return _operators.begin();
}



template< class operator_t >
inline typename op_container< operator_t >::iterator
op_container< operator_t >::end()
{
    return _operators.end();
}



template< class operator_t >
inline typename op_container< operator_t >::const_iterator
op_container< operator_t >::begin() const
{
    return _operators.begin();
}



template< class operator_t >
inline typename op_container< operator_t >::const_iterator
op_container< operator_t >::end() const
{
    return _operators.end();
}



template< class operator_t >
operator_t*
op_container< operator_t >::get_operator( const std::string& name )
{
    std::map< std::string, size_t >::iterator it = 
        _operators_by_name.find( name );
    if ( it == _operators_by_name.end() )
        return 0;
    assert( (*it).second  < _operators.size() );
    return _operators[ (*it).second ];
}



template< class operator_t >
operator_t*
op_container< operator_t >::get_operator( size_t index )
{
    assert( index  < _operators.size() );
    return _operators[ index ];
}



template< class operator_t >
void
op_container< operator_t >::setup_stage_0()
{
	for_each( begin(), end(),std::mem_fun( &operator_t::setup_stage_0 ) );
}



template< class operator_t >
void
op_container< operator_t >::setup_stage_1()
{
	for_each( begin(), end(), std::mem_fun( &operator_t::setup_stage_1 ) );
}



template< class operator_t >
void
op_container< operator_t >::setup_stage_2()
{
	for_each( begin(), end(), std::mem_fun( &operator_t::setup_stage_2 ) );
}



template< class operator_t >
void
op_container< operator_t >::prepare_processing()
{
	for_each( begin(), end(), std::mem_fun( &operator_t::prepare_processing ) );
}



template< class operator_t >
void
op_container< operator_t >::prepare_shutdown()
{
	for_each( begin(), end(), std::mem_fun( &operator_t::prepare_shutdown ) );
}



template< class operator_t >
std::string 
op_container< operator_t >::get_operator_timings_string() const
{
    std::stringstream stream;
    for( const_iterator it = begin(), it_end = end(); it != it_end; ++it )
    {
        stream << std::setw( 20 ) << (*it)->get_timer_string() << "s\n";
    }
    return stream.str();
}



template< class operator_t >
void 
op_container< operator_t >::push_back( operator_t* operator_ )
{
    assert( operator_ );
    _operators_by_name[ operator_->get_name() ] = _operators.size();
    _operators.push_back( operator_ );
}


template< class operator_t >
bool
op_container< operator_t >::empty()
{
    return _operators.empty();
}


} // namespace stream_process

#endif

