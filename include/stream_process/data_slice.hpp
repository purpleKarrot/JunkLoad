#ifndef __STREAM_PROCESS__DATA_SLICE__HPP__
#define __STREAM_PROCESS__DATA_SLICE__HPP__

#include <stream_process/stream_data.hpp>

#include <boost/pool/pool.hpp>

#include <list>
#include <vector>
#include <iostream>
#include <iomanip>

namespace stream_process
{

template<
    typename bounds_t,
    typename container_t
    >
class multi_slice : public bounds_t
{

public:
    typedef container_t                             container_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;
    
    static const size_t IS_LAST_SLICE = 1;
    
    multi_slice( size_t number_of_streams = 1 )
        : _streams( number_of_streams )
        , _slice_flags( 0 )
        , _slice_number( 0 )
    {}

    // slice 
    size_t get_slice_number() const
	{
		return _slice_number;
	}

    void set_slice_number( size_t slice_number_ )
	{
		_slice_number = slice_number_;
	}

    inline bool operator<( const multi_slice& slice_ )
    {
        return _slice_number < slice_._slice_number;
    }

    inline bool operator>( const multi_slice& slice_ )
    {
        return _slice_number > slice_._slice_number;
    }
    
    void set_is_last()
    {
        _slice_flags |= IS_LAST_SLICE;
    }
    
    inline bool is_last() { return _slice_flags & IS_LAST_SLICE; }
    
    // streams 
    inline size_t get_number_of_streams()
    {
        return _streams.size();
    }
    
    inline container_type& get_stream( size_t stream_index ) 
    {
        assert( stream_index < _streams.size() );
        return _streams[ stream_index ];
    }

    inline const container_type& get_stream( size_t stream_index ) const
    {
        assert( stream_index < _streams.size() );
        return _streams[ stream_index ];
    }

    inline container_type& operator[]( size_t index )
    {
        return get_stream( index );
    }

    inline const container_type& operator[]( size_t index ) const
    {
        return get_stream( index );
    }


    // iterators
    
    inline iterator begin( size_t stream_index = 0 )
    {
        assert( stream_index < _streams.size() );
        return _streams[ stream_index ].begin();
    }
     
    inline iterator end( size_t stream_index = 0 )
    {
        assert( stream_index < _streams.size() );
        return _streams[ stream_index ].end();
    }

    inline const_iterator begin( size_t stream_index = 0 ) const
    {
        assert( stream_index < _streams.size() );
        return _streams[ stream_index ].begin();
    }
     
    inline const_iterator end( size_t stream_index = 0 ) const
    {
        assert( stream_index < _streams.size() );
        return _streams[ stream_index ].end();
    }
    
    
    bool empty() const
    {
        typename std::vector< container_type >::const_iterator 
            it = _streams.begin(), it_end = _streams.end();
        for( ; it != it_end; ++it )
        {
            if ( ! (*it).empty() )
                return false;
        }
        return true;
    }
    
    
    void DEBUGprint( const std::string& name_ )
    {
        std::cout << "slice " << _slice_number << " "
            << "| r1max " << std::setw( 10 ) << bounds_t::_ring_one_max
            << "|   max " << std::setw( 10 ) << bounds_t::_max
            << "|   min " << std::setw( 10 ) << bounds_t::_min
            << "| r1min " << std::setw( 10 ) << bounds_t::_ring_one_min
            << "| " << name_
            << std::endl;
    }

protected:
    std::vector< container_type >   _streams;

    size_t  _slice_number;
    size_t  _slice_flags;

};




template<
    typename bounds_t,
    typename container_t
    >
class multi_stream_slice : public bounds_t, public container_t
{
public:
	typedef bounds_t							bounds_type;
	typedef container_t							stream_type;
	
    static const size_t IS_LAST_SLICE = 1;

	multi_stream_slice() : _slice_number( 0 ), _slice_flags( 0 )
	{
		_streams.push_back( this );
	}
    
    ~multi_stream_slice()
    {
        // FIXME 
        while( _streams.size() > 1 )
        {
            container_t* stream_ = _streams.back();
            _streams.pop_back();
            delete stream_;
        }
    }
	
    inline size_t get_number_of_streams()
    {
        return _streams.size();
    }
    
	inline stream_type& get_stream( size_t index = 0 )
	{
		assert( index < _streams.size() );
		return **( _streams.begin() + index );
	}

    size_t get_slice_number() const
	{
		return _slice_number;
	}
    void set_slice_number( size_t slice_number_ )
	{
		_slice_number = slice_number_;
	}
    inline bool operator<( const multi_stream_slice& slice_ )
    {
        return _slice_number < slice_._slice_number;
    }
    inline bool operator>( const multi_stream_slice& slice_ )
    {
        return _slice_number > slice_._slice_number;
    }
    
    void set_is_last()
    {
        _slice_flags |= IS_LAST_SLICE;
    }
    
    inline bool is_last() { return ( _slice_flags & IS_LAST_SLICE ); }
    
    void add_stream( container_t* stream_ )
    {
        assert( stream_ );
        _streams.push_back( stream_ );
    }
    
    void create_additional_stream()
    {
        _streams.push_back( new container_t() );
    }
	
protected:
	std::vector< stream_type* >	_streams;

    size_t  _slice_number;
    size_t  _slice_flags;

}; // class multi_stream_slice


template<
    typename bounds_t_,
    typename container_t
    >
class data_slice : public bounds_t_, public container_t	
{
public:    
    typedef bounds_t_   bounds_t;
    
    static const size_t IS_LAST_SLICE = 1;
    
    data_slice() : _slice_number( 0 ), _slice_flags( 0 ) {}
    
    struct get_index_functor
    {
        inline size_t operator()( const data_slice& s )
        {
            return s._slice_number;
        }
        inline size_t operator()( const data_slice* s )
        {
            return s->_slice_number;
        }
    };
    
    void print() { std::cout << "slice " << _slice_number << " "; bounds_t::print(); }
    size_t get_slice_number() const { return _slice_number; }
    void set_slice_number( size_t slice_number_ ) { _slice_number = slice_number_; }
    
    inline bool operator<( const data_slice& slice_ )
    {
        return _slice_number < slice_._slice_number;
    }
    inline bool operator>( const data_slice& slice_ )
    {
        return _slice_number > slice_._slice_number;
    }
    
    void set_is_last()
    {
        _slice_flags |= IS_LAST_SLICE;
    }
    
    inline bool is_last() { return ( _slice_flags & IS_LAST_SLICE ); }
    
protected:
    size_t  _slice_number;
    size_t  _slice_flags;
    
}; // class data_slice

} // namespace stream_process

#endif

