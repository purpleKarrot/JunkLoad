/*
 * libMaoni - common viewing framework
 *
 * Copyright (C) 2011 Daniel Pfeifer
 * Visualization and Multimedia Lab, University of Zurich
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Maoni/glew.h>
#include "Shader.hpp"
#include <iostream>
#include <fstream>

Shader::Shader(const char* filename) :
		Path(filename), name(0)
{
}

Shader::~Shader()
{
}

void Shader::bind() const
{
	if (!name)
		read_file();

	glUseProgram(name);
}

void Shader::unbind() const
{
	glUseProgram(0);
}

void Shader::reset()
{
	if (name)
	{
		glDeleteProgram(name);
		name = 0;
	}
}

const char* const Shader::filter() const
{
	return "Shader files (*.shader)";
}

static const char* const define_vert = "#define VERTEX_SHADER\n";
static const char* const define_frag = "#define FRAGMENT_SHADER\n";

#define ATTACH_SHADER(TYPE)                                                    \
{                                                                              \
    GLuint shader = glCreateShader(GL_##TYPE##_SHADER);                        \
    const char* source[2] = { "#define " #TYPE "_SHADER\n", code.c_str() };    \
    glShaderSource(shader, 2, source, 0);                                      \
    glCompileShader(shader);                                                   \
    glAttachShader(name, shader);                                              \
    glDeleteShader(shader);                                                    \
}                                                                              \

void Shader::read_file() const
{
	std::ifstream file(path().c_str());
	std::istreambuf_iterator<char> begin(file), end;
	std::string code(begin, end);

	name = glCreateProgram();

	ATTACH_SHADER(VERTEX)
	ATTACH_SHADER(FRAGMENT)

	glLinkProgram(name);

	GLint success;
	glGetProgramiv(name, GL_LINK_STATUS, &success);
	if (!success)
		std::cerr << "error linking program" << std::endl;

	GLint length;
	glGetProgramiv(name, GL_INFO_LOG_LENGTH, &length);
	if (length > 0)
	{
		std::string log(length, 0);
		glGetProgramInfoLog(name, length, 0, &log[0]);
		std::clog << log << std::endl;
	}
}
