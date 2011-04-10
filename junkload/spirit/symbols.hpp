/*
 * symbols.hpp
 *
 *  Created on: Apr 9, 2011
 *      Author: daniel
 */

#ifndef JUNKLOAD_SPIRIT_SYMBOLS_HPP
#define JUNKLOAD_SPIRIT_SYMBOLS_HPP

#include <boost/spirit/include/qi.hpp>
#include "../data_types.hpp"

namespace stream_process
{

struct scalar_symbols: boost::spirit::qi::symbols<char, data_type_id>
{
	scalar_symbols()
	{
		this->add
			("int8",    SP_INT_8   )
			("int16",   SP_INT_16  )
			("int32",   SP_INT_32  )
			("int64",   SP_INT_64  )
			("uint8",   SP_UINT_8  )
			("uint16",  SP_UINT_16 )
			("uint32",  SP_UINT_32 )
			("uint64",  SP_UINT_64 )
			("float32", SP_FLOAT_32)
			("float64", SP_FLOAT_64)
		;
	}
};

} // namespace stream_process

#endif /* JUNKLOAD_SPIRIT_SYMBOLS_HPP */
