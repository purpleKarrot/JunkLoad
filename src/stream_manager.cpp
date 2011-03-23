#include <stream_process/stream_manager.hpp>

namespace stream_process
{
} // namespace stream_process


#if 0    
    // debug
    _options.print_values();
    
    _setup_io();

    _setup_ops();

    _setup_point_structure();

    _finalize();
    
    _DEBUG_stuff();
}



void
stream_manager::_setup_io()
{
    if ( ! _stream_reader )
        _stream_reader = new stream_file_reader< stream_op_base_t >( _config );

    if ( ! _stream_writer )
        _stream_writer = new stream_file_writer( _config );
    
    // open output file for writing // FIXME -> setup_attribs
    _stream_writer->setup_output();
}



void
stream_manager::_setup_ops()
{
    assert( _stream_reader );
    assert( _stream_writer );

    // create op instances
 
    _nb_op = new nb_op( _config );
    _nb_op->add_config_options();
	
	_covar_op = new covariance_stream_op< stream_op_base_t >( _config );

    _ex_op = new example_stream_operator< stream_op_base_t >( _config );

    // call setup stage 0 / negotiate
    _stream_reader->setup_negotiate();
    _nb_op->setup_negotiate();
    _ex_op->setup_negotiate();
    _covar_op->setup_negotiate();

    // call setup stage 1 / finalize
    _stream_reader->setup_finalize();
    _ex_op->setup_finalize();
    _nb_op->setup_finalize();
    _covar_op->setup_finalize();
}


void
stream_manager::_setup_point_structure()
{
    // copy the input attributes metadata to stream point metadata
    _config.setup_input();

    // for all ops, get their attribute requests
    _stream_reader->setup_attributes();
    _nb_op->setup_attributes();
    _ex_op->setup_attributes();
    _covar_op->setup_attributes();
    
    _config.setup_point(); // must be before stream_reader->finalize_attr
    _stream_reader->finalize_attributes();

    
    _point_factory = new point_factory( _config.get_point_structure().compute_size_in_bytes() );
    _stream_reader->set_point_factory( _point_factory );
    
    std::cout
        << "stream structure: \n" 
        << _config.get_point_structure()
        << std::endl;
}


void
stream_manager::_finalize()
{
    // for each op
    _stream_writer->finalize();
    
    _nb_op->setup_accessors();
    _ex_op->setup_accessors();
    _covar_op->setup_accessors();
}

void
stream_manager::_DEBUG_stuff()
{
    _nb_det = new nb_det( _nb_op->get_shared_data() );

    std::list< data_slice_t* >  slices;
    bool x = false;
    while( _stream_reader->has_more() )
    {
        std::cout << "processing slice " << std::endl;
        data_slice_t* slice = new data_slice_t();

        _stream_reader->process( *slice );

        _nb_op->insert( *slice );

        _ex_op->process( *slice );
        
        slices.push_back( slice );
    }
    std::list< data_slice_t* >::iterator 
        it      = slices.begin(),
        it_end  = slices.end();
    for( ; it != it_end; ++it )
    {
        _nb_det->insert( * it );
        std::cout << "det " << _nb_det->detect() << std::endl;
    }

    it = slices.begin();
    it_end  = slices.end();
    for( ; it != it_end; ++it )
    {
        _nb_op->remove( **it );
    }

}

#endif
