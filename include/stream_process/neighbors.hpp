#ifndef __STREAM_PROCESS__NEIGHBORS__HPP__
#define __STREAM_PROCESS__NEIGHBORS__HPP__

#include <stream_process/neighbor_info.hpp>
#include <boost/pool/pool.hpp>
#include <stream_process/pair_functors.h>
#include <stream_process/exception.hpp>

#include <queue>
#include <functional>
#include <algorithm>
#include <iostream>

namespace stream_process
{

template< typename sp_types_t >
class neighbors : public sp_types_t
{
public:
    typedef typename sp_types_t::sp_float_type  sp_float_type;

    static const uint8_t    NB_ARRAY_IS_UNSORTED    = 1;
    static const uint8_t    NB_ARRAY_IS_HEAP        = 2;
    static const uint8_t    NB_ARRAY_IS_SORTED      = 4;

    typedef     neighbor_info< sp_types_t >     value_type;
    typedef     value_type*                     iterator;
    typedef     const value_type*               const_iterator;
    typedef     pair_compare_first_adapter< sp_float_type,
                    stream_data*, std::less< sp_float_type > > pc_t;
    
    struct globals
    {
        globals( size_t default_capacity_ )
            : default_capacity( default_capacity_ )
            , nb_info_pool( sizeof( value_type ) * default_capacity )
        {}
            
        size_t          default_capacity;
        boost::pool<>   nb_info_pool;

    }; // struct neighbors<>::globals
    
    neighbors( globals& globals );
    ~neighbors();
    
    inline bool push_if( sp_float_type distance, stream_data* point ); 
    
    inline value_type&          operator[]( size_t index );
    inline const value_type&    operator[]( size_t index ) const;
    
    inline size_t   size() const;       // k - number of neighbors
    inline size_t   capacity() const;   // max_k - max number of neighbors
    
    inline bool is_full() const;
    inline bool empty() const;
    
    inline iterator begin();
    inline iterator end();

    inline iterator rbegin();
    inline iterator rend();
    
    inline const_iterator   begin() const;
    inline const_iterator   end() const;
    
    inline const value_type&    get_farthest() const;
    
    inline void sort();

    inline bool is_heap() const;
    inline bool is_sorted() const;
    
    inline size_t get_neighbor_index( const value_type* nb ) const;
    
    friend std::ostream& operator<<( std::ostream& os,
        const neighbors< sp_types_t >& nbs )
    {
        const_iterator it = nbs.begin(), it_end = nbs.end();
        for( size_t index = 0; it != it_end; ++it, ++index )
        {
            os  << "neighbor " << index
                << " with distance " << (*it).get_distance()
                << " and ptr " << (*it).get_ptr()
                << "\n";
        }
        os << std::endl;
        return os;
    }
    
    void print();
    
    
protected:
    globals&        _globals;
    value_type*     _array;
    uint8_t         _size;      // number of neighbors
    uint8_t         _capacity;
    uint8_t         _flags;
    
    
}; // class neighbors


template< typename sp_types_t >
neighbors< sp_types_t >::
neighbors( globals& globals_ )
    : _globals( globals_ )
    , _array( 0 )
    , _size( 0 )
    , _capacity( _globals.default_capacity )
    , _flags( 1 )
{
    _array = reinterpret_cast< value_type* >( _globals.nb_info_pool.malloc() );
}


template< typename sp_types_t >
neighbors< sp_types_t >::
~neighbors()
{
    _globals.nb_info_pool.free( _array );
}



template< typename sp_types_t >
inline size_t
neighbors< sp_types_t >::
size() const
{
    return _size;
}



template< typename sp_types_t >
inline typename neighbors< sp_types_t >::iterator
neighbors< sp_types_t >::
begin()
{
    return _array;
}



template< typename sp_types_t >
inline typename neighbors< sp_types_t >::iterator
neighbors< sp_types_t >::
end()
{
    return _array + _size;
}



template< typename sp_types_t >
inline typename neighbors< sp_types_t >::iterator
neighbors< sp_types_t >::
rbegin()
{
    return _array + ( _size - 1 );
}



template< typename sp_types_t >
inline typename neighbors< sp_types_t >::iterator
neighbors< sp_types_t >::
rend()
{
    return _array - 1;
}



template< typename sp_types_t >
inline typename neighbors< sp_types_t >::const_iterator
neighbors< sp_types_t >::
begin() const
{
    return _array;
}



template< typename sp_types_t >
inline typename neighbors< sp_types_t >::const_iterator
neighbors< sp_types_t >::
end() const
{
    return _array + _size;
}


template< typename sp_types_t >
inline neighbor_info<  sp_types_t >&
neighbors< sp_types_t >::
operator[]( size_t index )
{
    assert( _array );
    assert( index < _size );
    return _array[ index ];
}


template< typename sp_types_t >
inline const neighbor_info< sp_types_t >&
neighbors< sp_types_t >::
operator[]( size_t index ) const
{
    assert( _array );
    assert( index < _size );
    return _array[ index ];
}



template< typename sp_types_t >
inline bool
neighbors< sp_types_t >::
push_if( sp_float_type distance, stream_data* point )
{
    if ( _size < _capacity )
    {
        _array[ _size++ ].set( distance, point );

        // once we have enough ( == _capacity ) nbs, we need a heap to
        // efficiently replace far away neighbors with ones closer to the
        // query point.
        if ( _size == _capacity )
        {
            assert( _flags & NB_ARRAY_IS_UNSORTED );

            std::make_heap( begin(), end() );

            if ( _flags & NB_ARRAY_IS_SORTED )
                _flags ^= NB_ARRAY_IS_SORTED;
            _flags |= NB_ARRAY_IS_HEAP;
            
        }
        return true;
    }
    else if ( is_heap() )
    {
        if ( distance < _array[ 0 ].get_distance() )
        {
            std::pop_heap( begin(), end() );
            _array[ _size - 1 ].set( distance, point ); // set the new nb data
            std::push_heap( begin(), end() );
            
            return true;
        }
        return false;
    }
    else if ( is_sorted() )
    {
        value_type& kth_nb = _array[ _size - 1 ];
        if ( distance < kth_nb.get_distance() )
        {
            kth_nb.set( distance, point );
            // we have to re-sort
            std::sort( begin(), end() );
            return true;
        }
        return false;
    }

    assert( 0 );
    return false;
}



template< typename sp_types_t >
inline bool
neighbors< sp_types_t >::
is_sorted() const
{
    return ( _flags & NB_ARRAY_IS_SORTED );
}



template< typename sp_types_t >
inline bool
neighbors< sp_types_t >::
is_heap() const
{
    return ( _flags & NB_ARRAY_IS_HEAP );
}


template< typename sp_types_t >
inline void
neighbors< sp_types_t >::
sort()
{
    std::sort_heap( begin(), end() );
    _flags |= NB_ARRAY_IS_SORTED;
    if ( _flags & NB_ARRAY_IS_HEAP )
        _flags ^= NB_ARRAY_IS_HEAP;
}



template< typename sp_types_t >
inline bool
neighbors< sp_types_t >::
is_full() const
{
    return _size == _capacity;
}


template< typename sp_types_t >
inline bool
neighbors< sp_types_t >::
empty() const
{
    return _size == 0;
}



template< typename sp_types_t >
inline const neighbor_info< sp_types_t >&
neighbors< sp_types_t >::
get_farthest() const
{
    assert( is_full() );

    if ( is_heap() )
        return _array[ 0 ];

    else if ( is_sorted() )
        return _array[ _size - 1 ];

    else if ( _size != 0 ) // unsorted, we have to search
    {
        const_iterator farthest = begin(), it = begin(), it_end = end();
        for( ++it; it != it_end; ++it )
        {
            const value_type& far = *farthest;
            const value_type& cur = *it;
            if ( cur.get_distance() > far.get_distance() )
                farthest = it;
        }
        return *farthest;
    }
    else
    {
        throw exception( "attempt to get kth neighbor of empty neighborhood.", 
            SPROCESS_HERE );
    }
}



template< typename sp_types_t >
inline size_t
neighbors< sp_types_t >::
get_neighbor_index( const value_type* nb ) const
{
    size_t index = nb - _array;
    assert( index < _size );
    return index;
}



template< typename sp_types_t >
void
neighbors< sp_types_t >::
print()
{
    std::ostream& os = std::cout;
    const_iterator it = begin(), it_end = end();
    for( size_t index = 0; it != it_end; ++it, ++index )
    {
        os  << "neighbor " << index
            << " with distance " << (*it).get_distance()
            << " and ptr " << (*it).get_ptr()
            << "\n";
    }
    os << std::endl;
}


} // namespace stream_process

#endif

