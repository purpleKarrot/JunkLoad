#ifndef __STREAM_PROCESS__VERTEX_INDEX_TRACKER__HPP__
#define __STREAM_PROCESS__VERTEX_INDEX_TRACKER__HPP__

#include <stream_process/stream_data.hpp>
#include <stream_process/exception.hpp>

#include <boost/thread.hpp>

#include <deque>
#include <limits>

namespace stream_process
{

class vertex_index_tracker
{
public:
    struct index_ref
    {
        index_ref( size_t index_, stream_data* ptr_ )
            : index( index_ )
            , out_index( std::numeric_limits< size_t >::max() )
            , ptr( ptr_ )
        {}

        size_t          index;
        size_t          out_index;
        stream_data*    ptr;
    };
    
    vertex_index_tracker() : _index_offset( 0 ), _out_index_bound( 0 ) {}
    
    size_t get_max_index() const
    {
        return _index_offset + _references.size();
    }

    inline index_ref& get( size_t index )
    {
        assert( index >= _index_offset 
            && index < _index_offset + _references.size() );
            
        return _references[ _index_offset + index ];
    }
    
    inline stream_data* get_ptr( size_t index )
    {

        assert( index >= _index_offset
            && index < _index_offset + _references.size() );

        return get( index ).ptr;
    }
    
    // single vertex 
    void add( size_t index, stream_data* pointer_ )
    {
        assert( index == _index_offset + _references.size() );
        assert( pointer_ );

        _references.push_back( index_ref( index, pointer_ ) );
    }
    
    void overwrite( size_t index, size_t new_index, stream_data* pointer_ )
    {
        assert( index < _index_offset + _references.size() );
        assert( pointer_ );
        
        index_ref& ir = _references[ _index_offset + index ];
        ir.ptr      = pointer_;
        ir.index    = new_index;
    }
    
    void remove( size_t index )
    {
        _references[ index ].ptr = 0;
    }

    void remove_old( size_t index )
    {
        std::deque< index_ref >::const_iterator
            it = _references.begin(), it_end = _references.end();
        for( ;it != it_end && (*it).index < index; ++it )
        {
            _references.pop_front();
            ++_index_offset;
        }
    }

    void set_out_index( size_t index, size_t out_index_ )
    {
        index_ref& ir = get( index );
        assert( ir.index == index );
        assert( ir.out_index == std::numeric_limits< size_t >::max() );

        ir.out_index = out_index_;

        if ( index >= _out_index_bound )
            _out_index_bound = index + 1;

    }
    
    size_t get_out_index( size_t index )
    {
        if ( index > _out_index_bound )
        {
            throw exception( "out_index not finalized.", SPROCESS_HERE );
        }
    
        return get( index ).out_index;
    }
    
    size_t get_out_index_bound() const
    {
        return _out_index_bound;
    }
    
    // FIXME better locking
    boost::mutex& get_mutex()
    {
        return _mutex;
    }
    
protected:

    size_t                  _index_offset;
    std::deque< index_ref > _references;
    
    size_t                  _out_index_bound;
            
    boost::mutex            _mutex;
    
}; // class vertex_index_tracker

} // namespace stream_process

#endif

