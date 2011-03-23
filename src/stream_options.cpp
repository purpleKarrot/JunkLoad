#include <stream_process/stream_options.hpp>

namespace stream_process
{

stream_options::stream_options()
{
    option o;

    o.setup_string( "input-filename", "NOT_SET" ).set_short_name( 'i' )
        .set_is_positional( true ).set_help_text( "base name of the input file/s.");

    add_option( o, "io" );    
    _mark_option_as_required( "input-filename" );

    o.setup_string( "output-filename", "NOT_SET" ).set_short_name( 'o' )
        .set_is_positional( true ).set_help_text( "base name of the output file/s." );
    add_option( o, "io" );
    
    // chain
    o.setup_string( "chain", "none" ).set_short_name( 'c' )
        .set_help_text( "name of a processing chain, either predefined or from 'spchains.cfg' in the local directory" );
    add_option( o, "processing" );

    o.setup_string( "stream-ops", "default" ).set_short_name( 'O' )
        .set_help_text( "list of stream operators used for processing" );
    add_option( o, "processing" );

    o.setup_string( "chain-ops", "default" ).set_short_name( 'C' )
        .set_help_text( "list of chain operators used for processing" );
    add_option( o, "processing" );

    o.setup_int( "threads", 8 ).set_short_name( 't' )
        .set_help_text( "number of threads, or 1 for single-threading." );
    add_option( o );

    o.setup_bool( "old-mode", false )
        .set_help_text( "run in old mt mode" );
    add_option( o, "processing" );

    o.setup_bool( "list-chains", false )
        .set_help_text( "prints a list of all hardcoded operator chains." );
    add_option( o );

    // preprocessing
    o.setup_bool( "optimal-transform", true )
        .set_help_text( "determine and apply optimal transform" );
    add_option( o, "preprocessing" );

    o.setup_bool( "pre-only", false ).set_short_name( 'p' )
        .set_help_text( "do only preprocessing." );
    add_option( o, "preprocessing" );

    // precision
    o.setup_bool( "float", false ).set_short_name( 'f' )
        .set_help_text( "single-precision only (default: mixed)" );
    add_option( o );

    o.setup_bool( "double", false ).set_short_name( 'd' )
        .set_help_text( "double-precision only (default: mixed)" );
    add_option( o );
    
    o.setup_bool( "verbose", false )
        .set_help_text( "verbose text output to console." );
    add_option( o );
    
    
    // modes 
    o.setup_bool( "process", true )
        .set_help_text( "process a file (default mode).");
    add_option( o, "mode" );

    o.setup_bool( "list", false ).set_help_text( "list the contents a data set.");
    add_option( o, "mode" );

    o.setup_bool( "diff", false )
        .set_help_text( "diff two files (show the differences between them).");
    add_option( o, "mode" );

    o.setup_bool( "find", false )
        .set_help_text( "find an attribute with a specific value in a data set.");
    add_option( o, "mode" );

    o.setup_bool( "gen", false )
        .set_help_text( "generate a test data set.");
    add_option( o, "mode" );

    o.setup_bool( "sort", false )
        .set_help_text( "sorts a data set.");
    add_option( o, "mode" );

}



void
stream_options::finalize()
{
    if ( ! get( "output-filename" ).was_set_by_user() )
    {
        std::string in_file = get( "input-filename" ).get_string();
        get( "output-filename" ).set_string( in_file + ".out", false );
    }
}

} // namespace stream_process

