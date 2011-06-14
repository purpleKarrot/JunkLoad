/* Copyright (c) 2009, Stefan Eilemann <eile@equalizergraphics.com>
 * Copyright (c) 2007, Tobias Wolf <twolf@access.unizh.ch>
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

#ifndef MESH_VERTEXBUFFERSTATE_H
#define MESH_VERTEXBUFFERSTATE_H

#include "typedefs.h"
#include <map>

#include <eq/eq.h>
#include "../channel.h"

namespace mesh
{

/*  The abstract base class for kd-tree rendering state.  */
class VertexBufferState
{
public:
	static const GLuint INVALID = 0;

	VertexBufferState(eq::Window::ObjectManager* objectManager) :
			_glewContext(objectManager->glewGetContext()), _useColors(false), _objectManager(objectManager)
	{
		MESHASSERT(_glewContext);
	}

	bool useColors() const
	{
		return _useColors;
	}

	void setColors(const bool colors)
	{
		_useColors = colors;
	}

	GLuint getBufferObject(const void* key)
	{
		return _objectManager->getBuffer(key);
	}

	GLuint newBufferObject(const void* key)
	{
		return _objectManager->newBuffer(key);
	}

	GLuint getProgram(const void* key)
	{
		return _objectManager->getProgram(key);
	}

	GLuint newProgram(const void* key)
	{
		return _objectManager->newProgram(key);
	}

	GLuint getShader(const void* key)
	{
		return _objectManager->getShader(key);
	}

	GLuint newShader(const void* key, GLenum type)
	{
		return _objectManager->newShader(key, type);
	}

	void deleteAll()
	{
		_objectManager->deleteAll();
	}

	bool isShared() const
	{
		return _objectManager->isShared();
	}

	void setChannel(const eqPly::Channel* channel)
	{
		_channel = channel;
	}

	bool stopRendering() const
	{
		EQASSERT( _channel );
		return _channel->stopRendering();
	}

	const GLEWContext* glewGetContext() const
	{
		return _glewContext;
	}

private:
	const GLEWContext* const _glewContext;
	bool _useColors;

	eq::Window::ObjectManager* _objectManager;
	const eqPly::Channel* _channel;
};

} // namespace mesh

#endif // MESH_VERTEXBUFFERSTATE_H
