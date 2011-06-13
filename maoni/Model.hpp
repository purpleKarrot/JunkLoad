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

#include <Maoni.hpp>
#include <junk/mapped_data_set.hpp>
#include <boost/scoped_ptr.hpp>

class Model: public Path
{
public:
	Model(const char* filename);

	virtual ~Model();

	void draw(int ranges) const;

private:
	void reset();
	const char* const filter() const;

	void read_file() const;

private:
	mutable boost::scoped_ptr<junk::mapped_data_set> data_set;
	mutable std::size_t faces;
	mutable GLuint vbuffer;
	mutable GLuint ibuffer;
};
