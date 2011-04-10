/*
 * generator.hpp
 *
 *  Created on: Apr 10, 2011
 *      Author: daniel
 */

#ifndef JUNKLOAD_SPIRIT_GENERATOR_HPP
#define JUNKLOAD_SPIRIT_GENERATOR_HPP

#include <boost/spirit/include/karma.hpp>
#include "adapted.hpp"
#include "../data_types.hpp"

namespace stream_process
{

struct scalar_symbols_generator:
	boost::spirit::karma::symbols<data_type_id, const char*>
{
	scalar_symbols_generator()
	{
		this->add
			(SP_INT_8,    "int8"   )
			(SP_INT_16,   "int16"  )
			(SP_INT_32,   "int32"  )
			(SP_INT_64,   "int64"  )
			(SP_UINT_8,   "uint8"  )
			(SP_UINT_16,  "uint16" )
			(SP_UINT_32,  "uint32" )
			(SP_UINT_64,  "uint64" )
			(SP_FLOAT_32, "float32")
			(SP_FLOAT_64, "float64")
		;
	}
};



} // namespace stream_process

#endif /* JUNKLOAD_SPIRIT_GENERATOR_HPP */
