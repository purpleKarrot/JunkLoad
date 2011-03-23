#ifndef __VMML__OPTIONS__H__
#define __VMML__OPTIONS__H__

#include "exception.hpp"

#include <boost/program_options.hpp>

#include <map>


namespace stream_process
{
namespace boost_po = ::boost::program_options;

typedef ::boost::program_options::variables_map var_map;
class options
{
public:
    options();
    // ctor for external use 
    options( bool no_options_setup );
    virtual ~options() {};
    
    void setup( int argc, const char* argv[] );
    void post_setup();
    virtual bool check() const;
    
    void print_options() const;
    virtual void print_usage() const;
    
    void add_op_desc( const std::string& op_name, 
        boost_po::options_description* desc );

    //void get_operator_chain( std::vector< std::string >& op_chain );
    
    bool has_key( const std::string& key ) const;
    bool has_option( const std::string& key ) const;

    const std::string& get_value_string( const std::string& key ) const;

    template< typename T >
    T& get_value( const std::string& key );

    template< typename T >
    const T& get_value( const std::string& key ) const;

    template< typename T >
    T get_value_copy( const std::string& key );

    template< typename T >
    void add_option( const std::string& key, const T& value, 
        const std::string& description = "" );

    //void load_op_chain_config();
    
    //const std::vector< std::vector< std::string > >& 
    //    get_op_configs_from_cfg_file() const;
    void print_all_keys() const;
    
    void debug_determine_type( const std::string& key );
    
protected:
    friend class chain_manager;
    
    
    int _argc_store;
    char** _argv_store;
    
    // generic options
    boost_po::options_description _generic_options;    
    // op chain 
    boost_po::options_description _all_options;    
    
    std::map< std::string, boost_po::options_description* > _op_descs;

    // data format options 
    boost_po::options_description _data_format;    
    // positional options
    boost_po::positional_options_description _positional_desc;   
     
    boost_po::variables_map _options_map;
    
    //std::vector< std::vector< std::string > > _op_cfgs_from_file;
    
}; // class options



template< typename T >
T&
options::get_value( const std::string& key )
{
    var_map::iterator it = _options_map.find( key );
    if( it != _options_map.end() )
    {
        return (*it).second.as< T >();
    }
    throw exception( std::string("no option value stored for key ") + key, 
        SPROCESS_HERE );
}



template< typename T >
const T&
options::get_value( const std::string& key ) const
{
    var_map::const_iterator it = _options_map.find( key );
    if( it != _options_map.end() )
    {
        return (*it).second.as< T >();
    }
    throw exception( std::string("no option value stored for key ") + key, 
        SPROCESS_HERE );
}



template< typename T >
T
options::get_value_copy( const std::string& key )
{
    var_map::iterator it = _options_map.find( key );
    if( it != _options_map.end() )
    {
        return (*it).second.as< T >();
    }
    throw exception( std::string("no option value stored for key ") + key, 
        SPROCESS_HERE );
}



template< typename T >
void
options::add_option( const std::string& key_, const T& initial_value_,
    const std::string& description_ )
{
    _generic_options.add_options()
        ( key_.c_str(), boost_po::value< T >()->default_value( initial_value_ ),
            description_.c_str() )
        ;
}


} // namespace stream_process

#endif
