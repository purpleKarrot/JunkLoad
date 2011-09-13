/*
 * BoxModel.hpp
 *
 *  Created on: Sep 13, 2011
 *      Author: dan
 */

#ifndef BOX_MODEL_HPP
#define BOX_MODEL_HPP

#include <Maoni.hpp>
#include <jnk/data_set.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/dynamic_bitset.hpp>
#include "BoxNode.hpp"

class BoxModel: public Path
{
public:
	BoxModel(const char* filename);

	virtual ~BoxModel();

	void draw(int nodes) const;

	uint32_t total_nodes() const
	{
		return branches_ + leaves_;
	}

	bool is_leaf(uint32_t index) const
	{
		assert(index < total_nodes());
		return index >= branches_;
	}

	uint32_t fanout() const
	{
		return fanout_;
	}

	void load(uint32_t index, GLuint& vbuffer, GLuint& ibuffer,
			GLsizei& faces) const;

	void unload(uint32_t index, GLuint& vbuffer, GLuint& ibuffer,
			GLsizei& faces) const;

	void get_box(uint32_t index, frustum::AlignedBox& box) const;

private:
	void reset();
	const char* const filter() const;

	void read_file() const;

private:
	uint32_t fanout_;
	uint32_t height_;

	uint32_t leaves_;
	uint32_t branches_;

	mutable boost::dynamic_bitset<> loaded_nodes;
	mutable uint32_t allowed_nodes;

	mutable boost::scoped_ptr<const junk::data_set> data_set;
	mutable boost::scoped_ptr<BoxNode> root;
};

#endif /* BOX_MODEL_HPP */
