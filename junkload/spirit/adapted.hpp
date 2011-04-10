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

BOOST_FUSION_ADAPT_STRUCT(stream_process::attribute,
		(std::string, name_)
		(stream_process::data_type_id, type_)
		(std::size_t, size_)
)

typedef const std::vector<stream_process::attribute>& attributes_t;

BOOST_FUSION_ADAPT_ADT(stream_process::element,
		(std::string, std::string, obj.name(), obj.name(val))
		(std::size_t, std::size_t, obj.size(), obj.size(val))
		(attributes_t, attributes_t, obj.attributes(), obj.attributes(val))
)

#endif /* JUNKLOAD_SPIRIT_ADAPTED_HPP */
