/*
 * attribute.hpp
 *
 *  Created on: Apr 9, 2011
 *      Author: daniel
 */

#ifndef JUNKLOAD_SPIRIT_ADAPTED_HPP
#define JUNKLOAD_SPIRIT_ADAPTED_HPP

#include <boost/fusion/adapted/adt/adapt_adt.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

#include "../attribute.hpp"
#include "../element.hpp"
#include "../data_set_header.hpp"
#include "../VMMLibIncludes.h"

BOOST_FUSION_ADAPT_STRUCT(stream_process::attribute,
		(std::string, name)
		(stream_process::data_type_id, type)
		(std::size_t, size)
)

BOOST_FUSION_ADAPT_STRUCT(stream_process::element,
		(std::string, _name)
		(std::size_t, _size)
		(std::vector<stream_process::attribute>, attributes_)
)

BOOST_FUSION_ADAPT_STRUCT(stream_process::header,
		(stream_process::mat4d, transform)
		(stream_process::vec3d, min)
		(stream_process::vec3d, max)
		(bool, big_endian)
		(std::vector<stream_process::element>, elements)
)

BOOST_FUSION_ADAPT_STRUCT(stream_process::vec3d,
		(double, array[0])
		(double, array[1])
		(double, array[2])
)

BOOST_FUSION_ADAPT_STRUCT(stream_process::mat4d,
		(double, array[0])
		(double, array[1])
		(double, array[2])
		(double, array[3])
		(double, array[4])
		(double, array[5])
		(double, array[6])
		(double, array[7])
		(double, array[8])
		(double, array[9])
		(double, array[10])
		(double, array[11])
		(double, array[12])
		(double, array[13])
		(double, array[14])
		(double, array[15])
)

#endif /* JUNKLOAD_SPIRIT_ADAPTED_HPP */
