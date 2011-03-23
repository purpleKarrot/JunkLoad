#ifndef __STREAM_PROCESS__OPTION__HPP__
#define __STREAM_PROCESS__OPTION__HPP__

#include <stream_process/exception.hpp>
#include <stream_process/data_types.hpp>

#include <string>
#include <boost/any.hpp>

namespace stream_process
{

class option
{
public:
    static const size_t     OPTION_WAS_SET_BY_USER  = 1;
    static const size_t     OPTION_HAS_SHORT_NAME   = 1 << 1;
    static const size_t     OPTION_IS_POSITIONAL    = 1 << 2;

    static const size_t     OPTION_IS_DOUBLE        = 1 << 4;
    static const size_t     OPTION_IS_BOOLEAN       = 1 << 5;
    static const size_t     OPTION_IS_STRING        = 1 << 6;
    static const size_t     OPTION_BOOLEAN_VALUE    = 1 << 7;
    
    option();

    option& setup_int( const std::string& long_name, int default_value );
    option& setup_float( const std::string& long_name, double default_value_ );
    option& setup_bool( const std::string& long_name, bool default_value_ );
    option& setup_string( const std::string& long_name, const std::string& default_value_ );
   
    inline bool has_short_name() const;
    option& set_short_name( char short_name );
    
    inline const std::string&   get_long_name() const;
    inline const char           get_short_name() const;
    inline const std::string&   get_help_text() const;
    
    inline bool was_set_by_user() const;
    
    inline void set_flag( size_t flag_, bool value_ );
    inline bool get_flag( size_t flag_ ) const;

    option& set_int( int value_, bool set_by_user = true );
    option& set_float( double value, bool set_by_user = true );
    option& set_string( const std::string& value_, bool set_by_user = true );
    option& set_bool( bool value_, bool set_by_user = true );
    
    int         get_int() const;
    double      get_float() const;
    std::string get_string() const;
    bool        get_bool() const;
   
    void set_from_string( const std::string& value_, bool set_by_user = true );
    std::string get_value_as_string() const;
    
    void set_help_text( const std::string& help_text_ );
    
    bool    is_positional() const;
    option& set_is_positional( bool is_positional_ );
    
    void    set_positional_number( size_t num );
    size_t  get_positional_number() const;
    
    void    clear();

    std::ostream& print_value( std::ostream& os ) const;
    friend std::ostream& operator<<( std::ostream& os, 
        const option& option_ ){ return option_.print_value( os ); };
        
    std::ostream& print_usage( std::ostream& os ) const;
    
    bool    is_int() const;
    bool    is_string() const;
    bool    is_float() const;
    bool    is_boolean() const;
    
protected:
    size_t          _flags;
    char            _short_name;
    std::string     _long_name;

    std::string     _string_value;
    double          _double_value;
    
    std::string     _help_text;
    size_t          _positional_number;
    
}; // class option



inline bool
option::has_short_name() const
{
    return _flags & OPTION_HAS_SHORT_NAME;
}


inline const std::string&
option::get_long_name() const
{
    return  _long_name;
}



inline const char
option::get_short_name() const
{
    return  _short_name;
}


inline const std::string&
option::get_help_text() const
{
    return  _help_text;
}



inline bool
option::was_set_by_user() const
{
    return _flags & OPTION_WAS_SET_BY_USER;
}


inline void
option::set_flag( size_t flag_, bool value_ )
{
   if ( ( bool ) ( _flags & flag_ ) != value_ )
        _flags ^= flag_;
}



inline bool
option::get_flag( size_t flag_ ) const
{
    return _flags & flag_;
}


} // namespace stream_process

#endif
