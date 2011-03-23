#include "mapped_point_data.hpp"
#include "point_header.hpp"
#include "string_utils.h"

namespace stream_process
{

mapped_point_data::mapped_point_data()
    : _point_info( 0 )
    , _point_data( 0 )
    , _clean_up_info_and_data_objects( true )
    , _filename( "unknown_filename" )
{}


mapped_point_data::mapped_point_data( const std::string& points_file_name, 
		const std::string& header_file_name )
    : _point_info( new point_info() )
    , _point_data( new boost::iostreams::mapped_file() )
    , _clean_up_info_and_data_objects( true )
    , _filename( points_file_name )
{
    _read_header( header_file_name );
    _mmap_data( points_file_name );
}



mapped_point_data::mapped_point_data( point_info& point_info_, 
        boost::iostreams::mapped_file& point_data, const std::string& filename )
    : _point_info( &point_info_ )
    , _point_data( &point_data )
    , _clean_up_info_and_data_objects( false )
    , _filename( filename )
{}



mapped_point_data::~mapped_point_data()
{
    write_header();
    
    if ( _clean_up_info_and_data_objects )
    {
        delete _point_info;

        _point_data->close();
        delete _point_data;
    }
    
    
}




stream_point*
mapped_point_data::get_point( size_t index )
{
    if ( index == 0 )
        return reinterpret_cast< stream_point* >( _point_data->data() ); 
        
    if ( index >= _point_info->get_point_count() )
    {
        throw exception( "requested point ptr with invalid index", 
            SPROCESS_HERE );
    }
    size_t point_size = _point_info->get_size_in_bytes();
    char* point = _point_data->data() + index * point_size;
    return reinterpret_cast< stream_point* >( point ); 
}



const stream_point*
mapped_point_data::get_point( size_t index ) const
{
    if ( index == 0 )
        return reinterpret_cast< const stream_point* >( _point_data->data() ); 
        
    if ( index >= _point_info->get_point_count() )
    {
        throw exception( "requested point ptr with invalid index", 
            SPROCESS_HERE );
    }
    size_t point_size = _point_info->get_size_in_bytes();
    const char* point = _point_data->data() + index * point_size;
    return reinterpret_cast< const stream_point* >( point ); 
}



void
mapped_point_data::setup_point_info( const std::string& header_filename, 
    point_info* point_info_ )
{
    _point_info = point_info_ == 0 ? new point_info() : point_info_;
    _read_header( header_filename );
}


void
mapped_point_data::setup_empty_point_info()
{
    _point_info = new point_info();
}


void
mapped_point_data::setup_mmap( const std::string& data_filename )
{
    _filename = data_filename;
    _point_data = new boost::iostreams::mapped_file();
    _mmap_data( _filename );
}


// PRE: point_info is set up properly, specifically it must contain
// size_in_bytes per point, and point_count
void
mapped_point_data::setup_empty_mmap( const std::string& filename )
{
    if ( ! _point_info )
    {
        throw exception( "tried setting up mmap before setting up point_info",
            SPROCESS_HERE );
    }

   _filename = filename;

    if ( _point_data )
    {
        delete _point_data;
    }
    
    boost::iostreams::mapped_file_params parameters;
    parameters.path             = _filename;
    parameters.mode             = std::ios_base::in | std::ios_base::out;
    parameters.new_file_size    = _point_info->get_point_set_size_in_bytes();
    
    _point_data = new boost::iostreams::mapped_file( parameters );
	if ( ! _point_data->is_open() )
	{
		throw exception( "mapping point data failed. ", SPROCESS_HERE );
	}
}



void
mapped_point_data::_read_header( const std::string& filename )
{
	point_header header( *_point_info );
	header.read( filename );
}



void
mapped_point_data::_mmap_data( const std::string& filename )
{
	// mmap data file
	_point_data->open( filename );
	if ( ! _point_data->is_open() )
	{
		throw exception( "mapping point data failed. ", SPROCESS_HERE );
	}
}



point_info&
mapped_point_data::get_point_info()
{
    return *_point_info;
}



const point_info&
mapped_point_data::get_point_info() const
{
    return *_point_info;
}


char*
mapped_point_data::get_data_ptr()
{
    return _point_data->data();
}



const char*
mapped_point_data::get_data_ptr() const
{
    return _point_data->data();
}



const std::string&
mapped_point_data::get_filename() const
{
    return _filename;
}


void
mapped_point_data::write_header( const std::string& header_filename_ )
{
    point_header header( *_point_info );

    std::string header_filename = header_filename_;

    if ( header_filename_.empty() )
    {
        try 
        {
            header_filename = string_utils::replace_file_suffix( _filename, ".ph" );
        }
        catch( std::exception& e )
        {
            header_filename = _filename + ".ph";
        }
    }

    LOGDEBUG << "writing header to " << header_filename << std::endl;
    header.write( header_filename );
}



void
mapped_point_data::recompute_aabb()
{
    vec3f min_ = std::numeric_limits< float >::max();
    vec3f max_ = -std::numeric_limits< float >::max();

    rt_struct_member< vec3f > position = 
        _point_info->get_accessor_for_attribute< vec3f >( "position" );
    
    size_t point_count          = _point_info->get_point_count();
    size_t point_size_in_bytes  = _point_info->get_point_size_in_bytes();
    
    char* point                 = get_data_ptr();
    char* points_end            = point + point_count * point_size_in_bytes;
    
    stream_point* stream_point_;
    //for( size_t point_index = 0; point_index < point_count; 
    //    ++point_index, point += point_size_in_bytes )
    for( ; point != points_end; point += point_size_in_bytes )
    {
        stream_point_ = reinterpret_cast< stream_point* >( point );
        const vec3f& pos = stream_point_->get( position );
        
        #ifdef SPROCESS_OLD_VMMLIB
        if ( pos.x > max_.x )
            max_.x = pos.x;
        if ( pos.y > max_.y )
            max_.y = pos.y;
        if ( pos.z > max_.z )
            max_.z = pos.z;
            
        if ( pos.x < min_.x )
            min_.x = pos.x;
        if ( pos.y < min_.y )
            min_.y = pos.y;
        if ( pos.z < min_.z )
            min_.z = pos.z;
        #else
        if ( pos.x() > max_.x() )
            max_.x() = pos.x();
        if ( pos.y() > max_.y() )
            max_.y() = pos.y();
        if ( pos.z() > max_.z() )
            max_.z() = pos.z();
            
        if ( pos.x() < min_.x() )
            min_.x() = pos.x();
        if ( pos.y() < min_.y() )
            min_.y() = pos.y();
        if ( pos.z() < min_.z() )
            min_.z() = pos.z();
        #endif
    }
    
    _point_info->set_min( min_ );
    _point_info->set_max( max_ );
    
    #if 1
    std::cout << "model aabb:\n"
        << "  min: " << min_ 
        << "\n  max: " << max_ 
        << std::endl;
    #endif
}





} // namespace stream_process

