/*
 * generator.hpp
 *
 *  Created on: Apr 10, 2011
 *      Author: daniel
 */

#include <fstream>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include "adapted.hpp"

namespace karma = boost::spirit::karma;
namespace ascii = boost::spirit::ascii;

#ifdef __BIG_ENDIAN__
# define ENDIAN "BIG_ENDIAN"
#else
# define ENDIAN "LITTLE_ENDIAN"
#endif

namespace
{

struct scalar_symbols: karma::symbols<junk::type, const char*>
{
	scalar_symbols()
	{
		this->add
			(junk::s_int_08, "int8"   )
			(junk::s_int_16, "int16"  )
			(junk::s_int_32, "int32"  )
			(junk::u_int_08, "uint8"  )
			(junk::u_int_16, "uint16" )
			(junk::u_int_32, "uint32" )
			(junk::float_32, "float32")
			(junk::float_64, "float64")
		;
	}
};

template<typename Iterator>
struct header_grammar: karma::grammar<Iterator, junk::element_list()>
{
	header_grammar() :
		header_grammar::base_type(start)
	{
		start
			%= "#define " ENDIAN << karma::eol << karma::eol
			<< element_ % karma::eol
			;

		element_
			%= "typedef struct {" << karma::eol
			<< attribute_ % karma::eol << karma::eol
			<< "} " << karma::string << ", " << karma::string
			<< size_ << ';' << karma::eol
			;

		attribute_
			%= "   " << scalar_ << ' ' << karma::string << size_ << ';'
			;

		size_
			%= karma::eps(karma::_val == 1) | '[' << karma::uint_ << ']'
			;
	}

	karma::rule<Iterator, junk::element_list()> start;
	karma::rule<Iterator, junk::element()> element_;
	karma::rule<Iterator, junk::attribute()> attribute_;
	karma::rule<Iterator, std::size_t()> size_;

	scalar_symbols scalar_;
};

} // unnamed namespace

namespace junk
{

bool save_header(const std::string& filename, const element_list& elements)
{
	typedef std::ostream_iterator<char> sink_type;

	std::ofstream file(filename.c_str());
	sink_type sink(file);
	header_grammar<sink_type> grammar;
	return karma::generate(sink, grammar, elements);
}

} // namespace junk
