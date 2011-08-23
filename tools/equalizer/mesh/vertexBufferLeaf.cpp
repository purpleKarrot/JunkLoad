/* Copyright (c) 2007, Tobias Wolf <twolf@access.unizh.ch>
 * Copyright (c) 2008-2009, Stefan Eilemann <eile@equalizergraphics.com>
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
  
    
    Implementation of the VertexBufferLeaf class.
*/


#include "vertexBufferLeaf.h"
#include "vertexBufferData.h"
#include "vertexBufferState.h"
#include "vertexData.h"
#include <map>

using namespace std;

namespace mesh
{

/*  Finish partial setup - sort, reindex and merge into global data.  */
void VertexBufferLeaf::setupTree( VertexData& data, const Index start,
                                  const Index length, const Axis axis,
                                  const size_t depth,
                                  VertexBufferData& globalData )
{
    data.sort( start, length, axis );
    _vertexStart = globalData.vertices.size();
    _vertexLength = 0;
    _indexStart = globalData.indices.size();
    _indexLength = 0;
    
    const bool hasColors = ( data.colors.size() > 0 ); 
    
    // stores the new indices (relative to _start)
    map< Index, ShortIndex > newIndex;
    
    for( Index t = 0; t < length; ++t )
    {
        for( Index v = 0; v < 3; ++v )
        {
            Index i = data.triangles[start + t][v];
            if( newIndex.find( i ) == newIndex.end() )
            {
                newIndex[i] = _vertexLength++;
                // assert number of vertices does not exceed SmallIndex range
                MESHASSERT( _vertexLength );
                globalData.vertices.push_back( data.vertices[i] );
                if( hasColors )
                    globalData.colors.push_back( data.colors[i] );
                globalData.normals.push_back( data.normals[i] );
            }
            globalData.indices.push_back( newIndex[i] );
            ++_indexLength; 
        }
    }
    
#ifndef NDEBUG
    MESHINFO << "VertexBufferLeaf::setupTree"
             << "( " << _indexStart << ", " << _indexLength << "; start " 
             << _vertexStart << ", " << _vertexLength << " vertices)." << endl;
#endif
}


/*  Compute the range of this child.  */
void VertexBufferLeaf::updateRange()
{
    _range[0] = 1.0f * _indexStart / _globalData.indices.size();
    _range[1] = _range[0] + 1.0f * _indexLength / _globalData.indices.size();
    
#ifndef NDEBUG
    MESHINFO << "updateRange" << "( " << _range[0] << ", " << _range[1]
             << " )." << endl;
#endif
}

#define glewGetContext state.glewGetContext

/*  Set up rendering of the leaf nodes.  */
void VertexBufferLeaf::setupRendering( VertexBufferState& state,
                                       GLuint* data ) const
{
        const char* charThis = reinterpret_cast< const char* >( this );
        
        if( data[VERTEX_OBJECT] == state.INVALID )
            data[VERTEX_OBJECT] = state.newBufferObject( charThis + 0 );
        glBindBuffer( GL_ARRAY_BUFFER, data[VERTEX_OBJECT] );
        glBufferData( GL_ARRAY_BUFFER, _vertexLength * sizeof( Vertex ),
                        &_globalData.vertices[_vertexStart], GL_STATIC_DRAW );
        
        if( data[NORMAL_OBJECT] == state.INVALID )
            data[NORMAL_OBJECT] = state.newBufferObject( charThis + 1 );
        glBindBuffer( GL_ARRAY_BUFFER, data[NORMAL_OBJECT] );
        glBufferData( GL_ARRAY_BUFFER, _vertexLength * sizeof( Normal ),
                        &_globalData.normals[_vertexStart], GL_STATIC_DRAW );
        
        if( data[COLOR_OBJECT] == state.INVALID )
            data[COLOR_OBJECT] = state.newBufferObject( charThis + 2 );
        if( state.useColors() )
        {
            glBindBuffer( GL_ARRAY_BUFFER, data[COLOR_OBJECT] );
            glBufferData( GL_ARRAY_BUFFER, _vertexLength * sizeof( Color ),
                            &_globalData.colors[_vertexStart], GL_STATIC_DRAW );
        }
        
        if( data[INDEX_OBJECT] == state.INVALID )
            data[INDEX_OBJECT] = state.newBufferObject( charThis + 3 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, data[INDEX_OBJECT] );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
                        _indexLength * sizeof( ShortIndex ),
                        &_globalData.indices[_indexStart], GL_STATIC_DRAW );
}


/*  Render the leaf.  */
void VertexBufferLeaf::render( VertexBufferState& state ) const
{
    if ( state.stopRendering( ))
        return;

    renderBufferObject( state );
}


/*  Render the leaf with buffer objects.  */
void VertexBufferLeaf::renderBufferObject( VertexBufferState& state ) const
{
    GLuint buffers[4];
    for( int i = 0; i < 4; ++i )
        buffers[i] = 
            state.getBufferObject( reinterpret_cast< const char* >(this) + i );
    if( buffers[VERTEX_OBJECT] == state.INVALID || 
        buffers[NORMAL_OBJECT] == state.INVALID || 
        buffers[COLOR_OBJECT] == state.INVALID || 
        buffers[INDEX_OBJECT] == state.INVALID )

        setupRendering( state, buffers );
    
    if( state.useColors() )
    {
        glBindBuffer( GL_ARRAY_BUFFER, buffers[COLOR_OBJECT] );
        glColorPointer( 4, GL_UNSIGNED_BYTE, 0, 0 );
    }
    glBindBuffer( GL_ARRAY_BUFFER, buffers[NORMAL_OBJECT] );
    glNormalPointer( GL_FLOAT, 0, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, buffers[VERTEX_OBJECT] );
    glVertexPointer( 3, GL_FLOAT, 0, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[INDEX_OBJECT] );
    glDrawElements( GL_TRIANGLES, GLsizei( _indexLength ), GL_UNSIGNED_SHORT, 0 );
}


/*  Read leaf node from memory.  */
void VertexBufferLeaf::fromMemory( char** addr, VertexBufferData& globalData )
{
    size_t nodeType;
    memRead( reinterpret_cast< char* >( &nodeType ), addr, sizeof( size_t ) );
    if( nodeType != LEAF_TYPE )
        throw MeshException( "Error reading binary file. Expected a leaf "
                             "node, but found something else instead." );
    VertexBufferBase::fromMemory( addr, globalData );
    memRead( reinterpret_cast< char* >( &_vertexStart ), addr, 
             sizeof( Index ) );
    memRead( reinterpret_cast< char* >( &_vertexLength ), addr, 
             sizeof( ShortIndex ) );
    memRead( reinterpret_cast< char* >( &_indexStart ), addr, 
             sizeof( Index ) );
    memRead( reinterpret_cast< char* >( &_indexLength ), addr, 
             sizeof( Index ) );
}


/*  Write leaf node to output stream.  */
void VertexBufferLeaf::toStream( std::ostream& os )
{
    size_t nodeType = LEAF_TYPE;
    os.write( reinterpret_cast< char* >( &nodeType ), sizeof( size_t ) );
    VertexBufferBase::toStream( os );
    os.write( reinterpret_cast< char* >( &_vertexStart ), sizeof( Index ) );
    os.write( reinterpret_cast< char* >( &_vertexLength ),sizeof( ShortIndex ));
    os.write( reinterpret_cast< char* >( &_indexStart ), sizeof( Index ) );
    os.write( reinterpret_cast< char* >( &_indexLength ), sizeof( Index ) );
}

}
