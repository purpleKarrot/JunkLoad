
#include <stream_process/option.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <iomanip>

namespace stream_process
{

option::option()
{
    clear();
}

option&
option::setup_int( const std::string& long_name_, int default_value_ )
{
    clear();

    set_flag( OPTION_IS_DOUBLE, true );
    
    _long_name = long_name_;
    return set_float( static_cast< double >( default_value_ ), false );
}


option&
option::setup_float( const std::string& long_name_, double default_value_ )
{
    clear();

    set_flag( OPTION_IS_DOUBLE, true );
    
    _long_name = long_name_;
    return set_float( default_value_, false );
}


option&
option::setup_bool( const std::string& long_name_, bool default_value_ )
{
    clear();

    set_flag( OPTION_IS_BOOLEAN, true );
    
    _long_name = long_name_;
    return set_bool( default_value_, false );
}


option&
option::setup_string( const std::string& long_name_, const std::string& default_value_ )
{
    clear();

    set_flag( OPTION_IS_STRING, true );
    
    _long_name = long_name_;
    return set_string( default_value_, false );
}


int
option::get_int() const
{
    return static_cast< int >( get_float() );
}


double
option::get_float() const
{
    if ( _flags & OPTION_IS_DOUBLE )
        return _double_value;
    throw exception( 
        std::string( "attempt to get non-numeric option'" )
        + _long_name + "' as number failed.", 
        SPROCESS_HERE );
}


void
option::clear()
{
    _flags = 0;
    _long_name      = "uninitialized";
    _short_name     = '0';
    _string_value   = "";
    _double_value   = 0.0;
    _help_text      = "";
    _positional_number = 0;
    
}


std::string
option::get_string() const
{
    if ( _flags & OPTION_IS_STRING )
        return _string_value;
    throw exception( 
        std::string( "attempt to get non-string option'" )
        + _long_name + "' as string failed.", 
        SPROCESS_HERE );
}


bool
option::get_bool() const
{
    if ( _flags & OPTION_IS_BOOLEAN )
        return ( _flags & OPTION_BOOLEAN_VALUE );
    throw exception(
        std::string( "attempt to get non-boolean option'" )
        + _long_name + "' as boolean failed.", 
        SPROCESS_HERE );
}



option&
option::set_int( int value_, bool set_by_user )
{
    set_float( static_cast< double >( value_ ), set_by_user );
    return *this;
}



option&
option::set_float( double value_, bool set_by_user )
{
    if ( _flags & OPTION_IS_DOUBLE )
    {
        _double_value = value_;

        if ( set_by_user )
            _flags |= OPTION_WAS_SET_BY_USER;

        return *this;
    }
    throw exception( 
        std::string( "attempt to set a numeric value to non-numeric option '" )
            + _long_name + "' failed.",
        SPROCESS_HERE );
}



option&
option::set_string( const std::string& value_, bool set_by_user )
{
    if ( _flags & OPTION_IS_STRING )
    {
        _string_value = value_;

        if ( set_by_user )
            _flags |= OPTION_WAS_SET_BY_USER;

        return *this;
    }
    else if ( _flags & OPTION_IS_DOUBLE )
    {
        try
        {
            double v = boost::lexical_cast< double >( value_ );
            return set_float( v, set_by_user );
        }
        catch(...)
        {
            throw exception(
                std::string( "attempt to convert option '" ) + _long_name + 
                    "': " + value_ + "to numeric value failed.",
                SPROCESS_HERE );
        }
    }
    else if ( _flags & OPTION_IS_BOOLEAN )
    {
        bool v = false;
        if ( value_.empty() )
            v = true;
        else
        {
            std::string boolv( value_ );
            boost::algorithm::to_lower( boolv );
            if ( boolv == "true" || boolv == "1" )
                v = true;
        }
        return set_bool( v, set_by_user );
    }

    throw exception(
        std::string( "failed to set option '" ) + _long_name + "' to value " 
            + value_, 
        SPROCESS_HERE );
}



option&
option::set_bool( bool value_, bool set_by_user )
{
    if ( _flags & OPTION_IS_BOOLEAN )
    {
        set_flag( OPTION_BOOLEAN_VALUE, value_ );

        if ( set_by_user )
            _flags |= OPTION_WAS_SET_BY_USER;

        return *this;
    }
    throw exception( 
        std::string( "attempt to set a boolean value to non-boolean option " )
            + _long_name + " failed.",
        SPROCESS_HERE );
}



std::string
option::get_value_as_string() const
{
    if ( _flags & OPTION_IS_DOUBLE )
    {
        return boost::lexical_cast< std::string >( _double_value );
    }
    else if ( _flags & OPTION_IS_BOOLEAN )
    {
        if ( _flags & OPTION_BOOLEAN_VALUE )
            return "true";
        else
            return "false";
    }
    else if ( _flags & OPTION_IS_STRING )
    {
        return _string_value;
    }
    throw exception( _long_name + ": option is corrupt.", SPROCESS_HERE );
}


option&
option::set_short_name( char short_name_ )
{
    set_flag( OPTION_HAS_SHORT_NAME, true );
    _short_name      = short_name_;
    return *this;
}


std::ostream&
option::print_value( std::ostream& os ) const
{
    os << std::setw( 20 ) << _long_name << " = " << get_value_as_string();
    return os;
}



std::ostream&
option::print_usage( std::ostream& os ) const
{
    os  << " --" << _long_name;
    for( size_t index = _long_name.size(); index < 24; ++index )
    {
        os << " ";
    }

    if ( has_short_name() )
        os << " (-" << get_short_name() << ")";
    else
        os << "     ";
    
    if( is_positional() )
        os << " (" << get_positional_number() << ") ";
    else
        os << "     ";
        
    os << get_help_text() << "\n";
    return os;
}


void
option::set_positional_number( size_t num )
{
    _positional_number = num;
    _flags |= OPTION_IS_POSITIONAL;
}


bool
option::is_positional() const
{
    return ( _flags & OPTION_IS_POSITIONAL );
}


option&
option::set_is_positional( bool is_positional_ )
{
    set_flag( OPTION_IS_POSITIONAL, is_positional_ );
    return *this;
}



size_t
option::get_positional_number() const
{
    return _positional_number;
}



void
option::set_help_text( const std::string& help_text_ )
{
    _help_text = help_text_;
}



bool
option::is_int() const
{
    return _flags & OPTION_IS_DOUBLE;
}



bool
option::is_string() const
{
    return _flags & OPTION_IS_STRING;
}



bool
option::is_float() const
{
    return _flags & OPTION_IS_DOUBLE;
}



bool
option::is_boolean() const
{
    return _flags & OPTION_IS_BOOLEAN;
}


} // namespace stream_process
