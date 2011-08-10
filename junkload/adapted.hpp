/*
 * adapted.hpp
 *
 *  Created on: 10.08.2011
 *      Author: daniel
 */

#ifndef JNK_ADAPTED_HPP
#define JNK_ADAPTED_HPP

#include <junk/types.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(junk::attribute,
	(junk::type, type)
	(std::string, name)
	(std::size_t, size)
	(std::size_t, offset)
	)

BOOST_FUSION_ADAPT_STRUCT(junk::element,
	(junk::attrib_list, attributes)
	(std::string, name_sg)
	(std::string, name_pl)
	(std::size_t, size)
	)

#endif /* JNK_ADAPTED_HPP */
