
/* Copyright (c) 2006-2011, Stefan Eilemann <eile@equalizergraphics.com>
 *               2010, Cedric Stalder <cedric.stalder@gmail.com>
 *               2007, Tobias Wolf <twolf@access.unizh.ch>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of Eyescale Software GmbH nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "channel.h"

#include "initData.h"
#include "config.h"
#include "configEvent.h"
#include "pipe.h"
#include "view.h"
#include "window.h"
#include "mesh/vertexBufferState.h"
#include <co/base/bitOperation.h> // function getIndexOfLastBit

// light parameters
static GLfloat lightPosition[] = {0.0f, 0.0f, 1.0f, 0.0f};
static GLfloat lightAmbient[]  = {0.1f, 0.1f, 0.1f, 1.0f};
static GLfloat lightDiffuse[]  = {0.8f, 0.8f, 0.8f, 1.0f};
static GLfloat lightSpecular[] = {0.8f, 0.8f, 0.8f, 1.0f};

// material properties
static GLfloat materialAmbient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
static GLfloat materialDiffuse[]  = {0.8f, 0.8f, 0.8f, 1.0f};
static GLfloat materialSpecular[] = {0.5f, 0.5f, 0.5f, 1.0f};
static GLint  materialShininess   = 64;

#ifndef M_SQRT3_2
#  define M_SQRT3_2  0.86603f  /* sqrt(3)/2 */
#endif

namespace eqPly
{

Channel::Channel( eq::Window* parent )
        : eq::Channel( parent )
        , _model(0)
        , _frameRestart( 0 )
{
}

bool Channel::configInit( const eq::uint128_t& initID )
{
    if( !eq::Channel::configInit( initID ))
        return false;

    setNearFar( 0.1f, 10.0f );
    _model = 0;
    return true;
}

bool Channel::configExit()
{
    for( size_t i = 0; i < eq::NUM_EYES; ++i )
    {
        delete _accum[ i ].buffer;
        _accum[ i ].buffer = 0;
    }

    return eq::Channel::configExit();
}

void Channel::frameClear( const eq::uint128_t& frameID )
{
    if( stopRendering( ))
        return;

    _initJitter();
    const FrameData& frameData = _getFrameData();
    const int32_t eyeIndex = co::base::getIndexOfLastBit( getEye() );
    if( _isDone() && !_accum[ eyeIndex ].transfer )
        return;

    applyBuffer();
    applyViewport();

    const eq::View* view = getView();
    if( view && frameData.getCurrentViewID() == view->getID( ))
        glClearColor( .4f, .4f, .4f, 1.0f );
#ifndef NDEBUG
    else if( getenv( "EQ_TAINT_CHANNELS" ))
    {
        const eq::Vector3ub color = getUniqueColor();
        glClearColor( color.r()/255.0f,
                      color.g()/255.0f,
                      color.b()/255.0f, 1.0f );
    }
#endif // NDEBUG
    else
        glClearColor( 0.f, 0.f, 0.f, 1.0f );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Channel::frameDraw( const eq::uint128_t& frameID )
{
    if( stopRendering( ))
        return;

    _initJitter();
    if( _isDone( ))
        return;

    const Model* model = _getModel();
    if( model )
        _updateNearFar();

    // Setup OpenGL state
    eq::Channel::frameDraw( frameID );

    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );
    glLightfv( GL_LIGHT0, GL_AMBIENT,  lightAmbient  );
    glLightfv( GL_LIGHT0, GL_DIFFUSE,  lightDiffuse  );
    glLightfv( GL_LIGHT0, GL_SPECULAR, lightSpecular );

    glMaterialfv( GL_FRONT, GL_AMBIENT,   materialAmbient );
    glMaterialfv( GL_FRONT, GL_DIFFUSE,   materialDiffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR,  materialSpecular );
    glMateriali(  GL_FRONT, GL_SHININESS, materialShininess );

    const FrameData& frameData = _getFrameData();
    glPolygonMode( GL_FRONT_AND_BACK, 
                   frameData.useWireframe() ? GL_LINE : GL_FILL );

    const eq::Vector3f& position = frameData.getCameraPosition();

    glMultMatrixf( frameData.getCameraRotation().array );
    glTranslatef( position.x(), position.y(), position.z() );
    glMultMatrixf( frameData.getModelRotation().array );

    if( frameData.getColorMode() == COLOR_DEMO )
    {
        const eq::Vector3ub color = getUniqueColor();
        glColor3ub( color.r(), color.g(), color.b() );
    }
    else
        glColor3f( .75f, .75f, .75f );

    if( model )
        _drawModel( model );
    else
    {
        glNormal3f( 0.f, -1.f, 0.f );
        glBegin( GL_TRIANGLE_STRIP );
            glVertex3f(  .25f, 0.f,  .25f );
            glVertex3f( -.25f, 0.f,  .25f );
            glVertex3f(  .25f, 0.f, -.25f );
            glVertex3f( -.25f, 0.f, -.25f );
        glEnd();
    }

    Accum& accum = _accum[ co::base::getIndexOfLastBit( getEye()) ];
    accum.stepsDone = EQ_MAX( accum.stepsDone, 
                              getSubPixel().size * getPeriod( ));
    accum.transfer = true;
}

void Channel::frameAssemble( const eq::uint128_t& frameID )
{
    if( stopRendering( ))
        return;

    if( _isDone( ))
        return;

    Accum& accum = _accum[ co::base::getIndexOfLastBit( getEye()) ];

    if( getPixelViewport() != _currentPVP )
    {
        accum.transfer = true;

        if( accum.buffer && !accum.buffer->usesFBO( ))
        {
            EQWARN << "Current viewport different from view viewport, ";
            EQWARN << "idle anti-aliasing not implemented." << std::endl;
            accum.step = 0;
        }

        eq::Channel::frameAssemble( frameID );
        return;
    }
    // else
    
    bool subPixelALL = true;
    const eq::Frames& frames = getInputFrames();

    for( eq::Frames::const_iterator i = frames.begin(); i != frames.end(); ++i )
    {
        eq::Frame* frame = *i;
        const eq::SubPixel& curSubPixel = frame->getSubPixel();

        if( curSubPixel != eq::SubPixel::ALL )
            subPixelALL = false;

        accum.stepsDone = EQ_MAX( accum.stepsDone, 
                                  frame->getSubPixel().size*frame->getPeriod());
    }

    accum.transfer = subPixelALL;

    applyBuffer();
    applyViewport();
    setupAssemblyState();

    eq::Compositor::assembleFrames( getInputFrames(), this, accum.buffer );

    resetAssemblyState();
}

void Channel::frameReadback( const eq::uint128_t& frameID )
{
    if( stopRendering( ))
        return;

    if( _isDone( ))
        return;

    // OPT: Drop alpha channel from all frames during network transport
    const FrameData& frameData = _getFrameData();
    const eq::Frames& frames = getOutputFrames();
    for( eq::Frames::const_iterator i = frames.begin(); i != frames.end(); ++i )
    {
        eq::Frame* frame = *i;
        frame->setAlphaUsage( false );
        
        if( frameData.isIdle( ))
            frame->setQuality( eq::Frame::BUFFER_COLOR, 1.f );
        else
            frame->setQuality( eq::Frame::BUFFER_COLOR, frameData.getQuality());
    }

    eq::Channel::frameReadback( frameID );
}

void Channel::frameStart( const eq::uint128_t& frameID,
                          const uint32_t frameNumber )
{
    if( stopRendering( ))
        return;

    for( size_t i = 0; i < eq::NUM_EYES; ++i )
        _accum[ i ].stepsDone = 0;

    eq::Channel::frameStart( frameID, frameNumber );
}

void Channel::frameViewStart( const eq::uint128_t& frameID )
{
    if( stopRendering( ))
        return;

    _currentPVP = getPixelViewport();
    _initJitter();
    eq::Channel::frameViewStart( frameID );
}

void Channel::frameFinish( const eq::uint128_t& frameID,
                           const uint32_t frameNumber )
{
    if( stopRendering( ))
        return;

    for( size_t i = 0; i < eq::NUM_EYES; ++i )
    {
        Accum& accum = _accum[ i ];
        if( accum.step > 0 )
        {
            if( static_cast< int32_t >( accum.stepsDone ) > accum.step )
                accum.step = 0;
            else
                accum.step -= accum.stepsDone;
        }
    }

    eq::Channel::frameFinish( frameID, frameNumber );
}

void Channel::frameViewFinish( const eq::uint128_t& frameID )
{
    if( stopRendering( ))
        return;

    applyBuffer();

    const FrameData& frameData = _getFrameData();
    Accum& accum = _accum[ co::base::getIndexOfLastBit( getEye()) ];

    if( accum.buffer )
    {
        const eq::PixelViewport& pvp = getPixelViewport();
        const bool isResized = accum.buffer->resize( pvp.w, pvp.h );

        if( isResized )
        {
            const View* view = static_cast< const View* >( getView( ));
            accum.buffer->clear();
            accum.step = view->getIdleSteps();
            accum.stepsDone = 0;
        }
        else if( frameData.isIdle( ))
        {
            setupAssemblyState();

            if( !_isDone() && accum.transfer )
                accum.buffer->accum();
            accum.buffer->display();

            resetAssemblyState();
        }
    }

    applyViewport();

    if( frameData.useStatistics())
        drawStatistics();

    ConfigEvent event;
    event.data.originator = getID();
    event.data.type = ConfigEvent::IDLE_AA_LEFT;

    if( frameData.isIdle( ))
    {
        int32_t maxSteps = 0;
        for( size_t i = 0; i < eq::NUM_EYES; ++i )
            maxSteps = EQ_MAX( maxSteps, _accum[i].step );

        event.steps = maxSteps;
    }
    else
    {
        const View* view = static_cast< const View* >( getView( ));
        if( view )
            event.steps = view->getIdleSteps();
        else
            event.steps = 0;
    }

    // if _jitterStep == 0 and no user redraw event happened, the app will exit
    // FSAA idle mode and block on the next redraw event.
    eq::Config* config = getConfig();
    config->sendEvent( event );
}

bool Channel::useOrtho() const
{
    const FrameData& frameData = _getFrameData();
    return frameData.useOrtho();
}

const FrameData& Channel::_getFrameData() const
{
    const Pipe* pipe = static_cast<const Pipe*>( getPipe( ));
    return pipe->getFrameData();
}

bool Channel::_isDone() const
{
    const FrameData& frameData = _getFrameData();
    if( !frameData.isIdle( ))
        return false;

    const eq::SubPixel& subpixel = getSubPixel();
    const Accum& accum = _accum[ co::base::getIndexOfLastBit( getEye()) ];
    return static_cast< int32_t >( subpixel.index ) >= accum.step;
}

void Channel::_initJitter()
{
    if( !_initAccum( ))
        return;

    const FrameData& frameData = _getFrameData();
    if( frameData.isIdle( ))
        return;

    const View* view = static_cast< const View* >( getView( ));
    if( !view )
        return;

    const uint32_t totalSteps = view->getIdleSteps();

    if( totalSteps == 0 )
        return;

    // ready for the next FSAA
    Accum& accum = _accum[ co::base::getIndexOfLastBit( getEye()) ];

    if( accum.buffer )
        accum.buffer->clear();
    accum.step = totalSteps;
}

bool Channel::_initAccum()
{
    View* view = static_cast< View* >( getNativeView( ));
    if( !view ) // Only alloc accum for dest
        return true;

    const eq::Eye eye = getEye();
    Accum& accum = _accum[ co::base::getIndexOfLastBit( eye ) ];

    if( accum.buffer ) // already done
        return true;

    if( accum.step == -1 ) // accum init failed last time
        return false;

    // Check unsupported cases
    if( !eq::util::Accum::usesFBO( glewGetContext( )))
    {
        for( size_t i = 0; i < eq::NUM_EYES; ++i )
        {
            if( _accum[ i ].buffer )
            {
                EQWARN << "glAccum-based accumulation does not support "
                       << "stereo, disabling idle anti-aliasing."
                       << std::endl;
                for( size_t j = 0; j < eq::NUM_EYES; ++j )
                {
                    delete _accum[ j ].buffer;
                    _accum[ j ].buffer = 0;
                    _accum[ j ].step = -1;
                }

                view->setIdleSteps( 0 );
                return false;
            }
        }
    }

    // set up accumulation buffer
    accum.buffer = new eq::util::Accum( glewGetContext( ));
    const eq::PixelViewport& pvp = getPixelViewport();
    EQASSERT( pvp.isValid( ));

    if( !accum.buffer->init( pvp, getWindow()->getColorFormat( )) ||
        accum.buffer->getMaxSteps() < 256 )
    {
        EQWARN <<"Accumulation buffer initialization failed, "
               << "idle AA not available." << std::endl;
        delete accum.buffer;
        accum.buffer = 0;
        accum.step = -1;
        return false;
    }

    // else
    EQVERB << "Initialized "
           << (accum.buffer->usesFBO() ? "FBO accum" : "glAccum")
           << " buffer for " << getName() << " " << getEye() 
           << std::endl;

    view->setIdleSteps( accum.buffer ? 256 : 0 );
    return true;
}

bool Channel::stopRendering() const
{ 
    return getPipe()->getCurrentFrame() < _frameRestart; 
}

eq::Vector2f Channel::getJitter() const
{
    const FrameData& frameData = _getFrameData();
    const Accum& accum = _accum[ co::base::getIndexOfLastBit( getEye()) ];

    if( !frameData.isIdle() || accum.step <= 0 )
        return eq::Channel::getJitter();

    const View* view = static_cast< const View* >( getView( ));
    if( !view || view->getIdleSteps() != 256 )
        return eq::Vector2f::ZERO;

    eq::Vector2i jitterStep = _getJitterStep();
    if( jitterStep == eq::Vector2i::ZERO )
        return eq::Vector2f::ZERO;

    const eq::PixelViewport& pvp = getPixelViewport();
    const float pvp_w = float( pvp.w );
    const float pvp_h = float( pvp.h );
    const float frustum_w = float(( getFrustum().get_width( )));
    const float frustum_h = float(( getFrustum().get_height( )));

    const float pixel_w = frustum_w / pvp_w;
    const float pixel_h = frustum_h / pvp_h;

    const float sampleSize = 16.f; // sqrt( 256 )
    const float subpixel_w = pixel_w / sampleSize;
    const float subpixel_h = pixel_h / sampleSize;

    // Sample value randomly computed within the subpixel
    co::base::RNG rng;
    float value_i = rng.get< float >() * subpixel_w
                    + float( jitterStep.x( )) * subpixel_w;

    float value_j = rng.get< float >() * subpixel_h
                    + float( jitterStep.y( )) * subpixel_h;

    const eq::Pixel& pixel = getPixel();
    value_i /= float( pixel.w );
    value_j /= float( pixel.h );

    return eq::Vector2f( value_i, value_j );
}

namespace
{
static const uint32_t _primes[100] = {
    739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829,
    839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941,
    947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 1019, 1021, 1031, 1033,
    1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097, 1103, 1109,
    1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193, 1201, 1213,
    1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291,
    1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399,
    1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451 };
}

eq::Vector2i Channel::_getJitterStep() const
{
    const eq::SubPixel& subPixel = getSubPixel();
    const uint32_t channelID = subPixel.index;
    const View* view = static_cast< const View* >( getView( ));
    if( !view )
        return eq::Vector2i::ZERO;

    const uint32_t totalSteps = view->getIdleSteps();
    if( totalSteps != 256 )
        return eq::Vector2i::ZERO;

    const Accum& accum = _accum[ co::base::getIndexOfLastBit( getEye()) ];
    const uint32_t subset = totalSteps / getSubPixel().size;
    const uint32_t idx = 
        ( accum.step * _primes[ channelID ] ) % subset + ( channelID * subset );

    const uint32_t sampleSize = 16;
    const int dx = idx % sampleSize;
    const int dy = idx / sampleSize;

    return eq::Vector2i( dx, dy );
}

const Model* Channel::_getModel()
{
	if (!_model)
		_model = static_cast<Config*> (getConfig())->getModel(co::base::UUID::ZERO);

	return _model;
}

void Channel::_drawModel( const Model* model )
{
    Window*            window    = static_cast< Window* >( getWindow( ));
    mesh::VertexBufferState& state     = window->getState();
    const FrameData&   frameData = _getFrameData();
    const eq::Range&   range     = getRange();

    if( frameData.getColorMode() == COLOR_MODEL && model->hasColors( ))
        state.setColors( true );
    else
        state.setColors( false );
    state.setChannel( this );

    const eq::Pipe* pipe = getPipe();
    const GLuint program = state.getProgram( pipe );
    if( program != mesh::VertexBufferState::INVALID )
        glUseProgram( program );
    
    model->beginRendering( state );

    // start with root node
    std::vector< const mesh::VertexBufferBase* > candidates;
    candidates.push_back( model );

    while( !candidates.empty() )
    {
        if( stopRendering( ))
            return;

        const mesh::VertexBufferBase* treeNode = candidates.back();
        candidates.pop_back();
            
        // completely out of range check
        if( treeNode->getRange()[0] >= range.end || 
            treeNode->getRange()[1] < range.start )
            continue;
            
        // bounding sphere view frustum culling
        const vmml::Visibility visibility = vmml::VISIBILITY_FULL;

        switch( visibility )
        {
            case vmml::VISIBILITY_FULL:
                // if fully visible and fully in range, render it
                if( range == eq::Range::ALL || 
                    ( treeNode->getRange()[0] >= range.start && 
                      treeNode->getRange()[1] < range.end ))
                {
                    treeNode->render( state );
                    break;
                }
                // partial range, fall through to partial visibility

            case vmml::VISIBILITY_PARTIAL:
            {
                const mesh::VertexBufferBase* left  = treeNode->getLeft();
                const mesh::VertexBufferBase* right = treeNode->getRight();
            
                if( !left && !right )
                {
                    if( treeNode->getRange()[0] >= range.start )
                    {
                        treeNode->render( state );
                    }
                    // else drop, to be drawn by 'previous' channel
                }
                else
                {
                    if( left )
                        candidates.push_back( left );
                    if( right )
                        candidates.push_back( right );
                }
                break;
            }
            default:
                // do nothing
                break;
        }
    }
    
    model->endRendering( state );
    state.setChannel( 0 );

    glUseProgram( 0 );
}

void Channel::_updateNearFar()
{
    // compute dynamic near/far plane of whole model
    const FrameData& frameData = _getFrameData();

    const eq::Matrix4f& rotation     = frameData.getCameraRotation();
    const eq::Matrix4f headTransform = getHeadTransform() * rotation;

    eq::Matrix4f modelInv;
    compute_inverse( headTransform, modelInv );

    const eq::Vector3f zero  = modelInv * eq::Vector3f::ZERO;
    eq::Vector3f       front = modelInv * eq::Vector3f( 0.0f, 0.0f, -1.0f );

    front -= zero;
    front.normalize();
    front *= 2;

    const eq::Vector3f center = frameData.getCameraPosition().get_sub_vector< 3 >();
    const eq::Vector3f nearPoint  = headTransform * ( center - front );
    const eq::Vector3f farPoint   = headTransform * ( center + front );

    if( useOrtho( ))
    {
        EQASSERTINFO( fabs( farPoint.z() - nearPoint.z() ) >
                      std::numeric_limits< float >::epsilon(),
                      nearPoint << " == " << farPoint );
        setNearFar( -nearPoint.z(), -farPoint.z() );
    }
    else
    {
        // estimate minimal value of near plane based on frustum size
        const eq::Frustumf& frustum = getFrustum();
        const float width  = fabs( frustum.right() - frustum.left() );
        const float height = fabs( frustum.top() - frustum.bottom() );
        const float size   = EQ_MIN( width, height );
        const float minNear = frustum.near_plane() / size * .001f;

        const float zNear = EQ_MAX( minNear, -nearPoint.z() );
        const float zFar  = EQ_MAX( zNear * 2.f, -farPoint.z() );

        setNearFar( zNear, zFar );
    }
}

}
