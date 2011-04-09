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
		(std::string, _name)
		(stream_process::data_type_id, _data_type_id)
		(std::size_t, _number_of_elements)
		(std::size_t, _flags)
)

BOOST_FUSION_ADAPT_ADT(stream_process::data_element,
		(std::string, _name)
		(stream_process::data_type_id, _data_type_id)
		(std::size_t, _number_of_elements)
		(std::size_t, _flags)
)

#endif /* JUNKLOAD_SPIRIT_ADAPTED_HPP */
