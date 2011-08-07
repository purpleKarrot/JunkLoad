/*
 * generator.hpp
 *
 *  Created on: Apr 10, 2011
 *      Author: daniel
 */

#include <fstream>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <junk/types.hpp>

namespace karma = boost::spirit::karma;
namespace ascii = boost::spirit::ascii;

namespace
{

struct scalar_symbols: karma::symbols<junk::typid, const char*>
{
	scalar_symbols()
	{
		this->add
			(junk::SP_INT_8,    "int8"   )
			(junk::SP_INT_16,   "int16"  )
			(junk::SP_INT_32,   "int32"  )
			(junk::SP_UINT_8,   "uint8"  )
			(junk::SP_UINT_16,  "uint16" )
			(junk::SP_UINT_32,  "uint32" )
			(junk::SP_FLOAT_32, "float32")
			(junk::SP_FLOAT_64, "float64")
		;
	}
};

struct endian_policy: karma::bool_policies<>
{
    template <typename CharEncoding, typename Tag, typename OutputIterator>
    static bool generate_true(OutputIterator& sink, bool b)
    {
        return karma::string_inserter<CharEncoding, Tag>::call(sink, "BIG_ENDIAN");
    }

    template<typename CharEncoding, typename Tag, typename OutputIterator>
	static bool generate_false(OutputIterator& sink, bool b)
	{
		return karma::string_inserter<CharEncoding, Tag>::call(sink, "LITTLE_ENDIAN");
	}
};

template<typename Iterator>
struct header_grammar: karma::grammar<Iterator, junk::header()>
{
	header_grammar() :
		header_grammar::base_type(start)
	{
		start
			%= "#define " << endian << karma::eol << karma::eol
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

	karma::rule<Iterator, junk::header()> start;
	karma::rule<Iterator, junk::element()> element_;
	karma::rule<Iterator, junk::attribute()> attribute_;
	karma::rule<Iterator, std::size_t()> size_;

	scalar_symbols scalar_;
	karma::bool_generator<bool, endian_policy> endian;
};

} // unnamed namespace

namespace junk
{

bool save_header(const std::string& filename, const header& h)
{
	typedef std::ostream_iterator<char> sink_type;

	std::ofstream file(filename.c_str());
	sink_type sink(file);
	header_grammar<sink_type> g;
	return karma::generate(sink, g, h);
}

} // namespace junk
