/*
 * generator.hpp
 *
 *  Created on: Apr 10, 2011
 *      Author: daniel
 */

#ifndef JUNKLOAD_SPIRIT_GENERATOR_HPP
#define JUNKLOAD_SPIRIT_GENERATOR_HPP

#include <fstream>
#include <boost/spirit/include/karma.hpp>
#include <junk/types.hpp>

namespace karma = boost::spirit::karma;
namespace ascii = boost::spirit::ascii;

namespace junk
{

struct scalar_symbols: karma::symbols<typid, const char*>
{
	scalar_symbols()
	{
		this->add
			(SP_INT_8,    "int8"   )
			(SP_INT_16,   "int16"  )
			(SP_INT_32,   "int32"  )
			(SP_UINT_8,   "uint8"  )
			(SP_UINT_16,  "uint16" )
			(SP_UINT_32,  "uint32" )
			(SP_FLOAT_32, "float32")
			(SP_FLOAT_64, "float64")
		;
	}
};

struct endian_policy: karma::bool_policies<>
{
    template <typename CharEncoding, typename Tag, typename OutputIterator>
    static bool generate_true(OutputIterator& sink, bool b)
    {
        return karma::string_inserter<CharEncoding, Tag>::call(sink, "big_endian");
    }

    template<typename CharEncoding, typename Tag, typename OutputIterator>
	static bool generate_false(OutputIterator& sink, bool b)
	{
		return karma::string_inserter<CharEncoding, Tag>::call(sink, "little_endian");
	}
};

template<typename Iterator, typename Skipper>
struct header_grammar: karma::grammar<Iterator, header(), Skipper>
{
	header_grammar() :
		header_grammar::base_type(start)
	{
		start
			%= "byteorder" << endian << karma::eol
			<< karma::eol
			<< element_ % karma::eol
			;

		element_
			%= karma::string
			<< karma::uint_
			<< karma::eol << '{' << karma::eol
			<< attribute_ % karma::eol
			<< karma::eol << '}' << karma::eol
			;

		attribute_
			%= karma::string << scalar_ << karma::uint_ << ';'
			;
	}

	karma::rule<Iterator, header(), Skipper> start;
	karma::rule<Iterator, element(), Skipper> element_;
	karma::rule<Iterator, attribute(), Skipper> attribute_;

	scalar_symbols scalar_;
	karma::bool_generator<bool, endian_policy> endian;
};

bool save_header(const std::string& filename, const header& h)
{
	typedef std::ostream_iterator<char> sink_type;

	std::ofstream file(filename.c_str());
	sink_type sink(file);
	header_grammar<sink_type, ascii::space_type> g;
	return karma::generate_delimited(sink, g, ascii::space, h);
}

} // namespace junk

#endif /* JUNKLOAD_SPIRIT_GENERATOR_HPP */
