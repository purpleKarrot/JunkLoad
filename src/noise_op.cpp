/*
 *  noise_op.cpp
 *  StreamProcessing
 *
 * created by jonas bösch
 *
 */

#include "noise_op.h"

#include <cmath>


namespace stream_process
{

noise_op::noise_op()
    : stream_op()
    , _point( 0 )
    , _scale( pow(2.0, 30.0) - 1.0 )
    , _position( "position" )
{
    set_name( "noise" );

    _max_change = 0.0005f;   
}

noise_op::~noise_op()
{}

void noise_op::_addNoise()
{
    vec3f& pos = _position.get( _point );
    //cout << "noise_op::_addNoise - pre-noise: " << pos <<std::endl;
    #ifdef SPROCESS_OLD_VMMLIB
    pos.x += - _max_change[0] + _max_change[0] * ( ( (float)random() ) / _scale );
    pos.y += - _max_change[1] + _max_change[1] * ( ( (float)random() )/ _scale );
    pos.z += - _max_change[2] + _max_change[2] * ( ( (float)random() ) / _scale );
    #else
    pos.x() += - _max_change[0] + _max_change[0] * ( ( (float)random() ) / _scale );
    pos.y() += - _max_change[1] + _max_change[1] * ( ( (float)random() )/ _scale );
    pos.z() += - _max_change[2] + _max_change[2] * ( ( (float)random() ) / _scale );
    #endif
    //cout << "noise_op::_addNoise - post-noise: " << pos <<std::endl;
}

void noise_op::pull_push()
{  
  // pull elements from previous operator
  while ( ( _point = prev->front() ) ) 
  {
    prev->pop_front();
    _addNoise();
    _fifo.push_back( _point );
  }
}

stream_point* noise_op::front()
{
    return ( _fifo.empty() ) ? 0 : _fifo.front();
}

void noise_op::pop_front()
{
    if ( ! _fifo.empty() )
        _fifo.pop_front();
}


void 
noise_op::setup_stage_0()
{}



void 
noise_op::setup_stage_1()
{
    #if 0
    // FIXME
    if ( config.size() == 4 )
    {
        _max_change.x = static_cast< double > ( atof( config[1].c_str() ) );   
        _max_change.y = static_cast< double > ( atof( config[2].c_str() ) );   
        _max_change.z = static_cast< double > ( atof( config[3].c_str() ) );   
    }
    else 
        _max_change = static_cast< double > ( atof( config[1].c_str() ) );
    #endif
   std::cout << "    max change: " << _max_change <<std::endl;
}



void 
noise_op::setup_stage_2() 
{}


size_t 
noise_op::size() const
{ 
    return _fifo.size(); 
}


} // namespace stream_process
