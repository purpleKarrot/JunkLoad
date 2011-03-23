#ifndef __STREAM_PROCESS__STREAM_STATS__HPP__
#define __STREAM_PROCESS__STREAM_STATS__HPP__

namespace stream_process
{

class stream_stats
{
public:
    stream_stats() : _current( 0 ), _max( 0 ), _total( 0 ) {}
    
    inline void operator+=( const size_t count_ )
    {
        _current    += count_;
        _total      += count_;
        
        if ( _current > _max )
            _max = _current;
    }

    inline void operator-=( const size_t count_ )
    {
        _current    -= count_;
    }
    
    inline void operator++( int i )
    {
        ++_current;
        ++_total;

        if ( _current > _max )
            _max = _current;
    }
    
    inline void operator++()
    {
        ++_current;
        ++_total;

        if ( _current > _max )
            _max = _current;
    }
    
    inline void operator--( int i )
    {
        --_current;
    }

    inline void operator--()
    {
        --_current;
    }
    
    size_t current() const  { return _current; }
    size_t max() const      { return _max; }
    size_t total() const    { return _total; }
    
protected:
    size_t _current;
    size_t _max;
    size_t _total;

protected:

}; // class stream_stats

} // namespace stream_process

#endif

