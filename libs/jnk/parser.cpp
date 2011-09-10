/*
 * qi.hpp
 *
 *  Created on: Apr 9, 2011
 *      Author: daniel
 */

#define FUSION_MAX_VECTOR_SIZE 16

#include <fstream>
#include <iomanip>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>

#include "adapted.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace classic = boost::spirit::classic;

#ifdef __BIG_ENDIAN__
# define ENDIAN "BIG_ENDIAN"
#else
# define ENDIAN "LITTLE_ENDIAN"
#endif

namespace
{

struct scalar_symbols: qi::symbols<char, junk::type>
{
	scalar_symbols()
	{
		this->add
			("int8",    junk::s_int_08)
			("int16",   junk::s_int_16)
			("int32",   junk::s_int_32)
			("uint8",   junk::u_int_08)
			("uint16",  junk::u_int_16)
			("uint32",  junk::u_int_32)
			("float32", junk::float_32)
			("float64", junk::float_64)
		;
	}
};

template<typename Iterator, typename Skipper>
struct grammar: qi::grammar<Iterator, junk::element_list(), Skipper>
{
	grammar() :
		grammar::base_type(start)
	{
		start
			%= qi::eps
			> "#define " ENDIAN
			> *element_
			> qi::eoi
			;

		element_
			%= "typedef struct {"
			> *attribute_
			> '}' > string_ > ',' > string_ > size_ > ';'
			;

		attribute_
			%= scalar_ > string_ > size_ > ';'
			;

		string_
			%= qi::lexeme[+(ascii::alnum | qi::char_('_'))]
			;

		size_
			%= ('[' > qi::uint_ > ']') | qi::eps(qi::_val = 1)
			;
	}

	qi::rule<Iterator, junk::element_list(), Skipper> start;
	qi::rule<Iterator, junk::element(), Skipper> element_;
	qi::rule<Iterator, junk::attribute(), Skipper> attribute_;
	qi::rule<Iterator, std::string(), Skipper> string_;
	qi::rule<Iterator, std::size_t(), Skipper> size_;

	scalar_symbols scalar_;
};

} // unnamed namespace

namespace junk
{

bool load_header(const std::string& filename, element_list& elements)
{
	typedef std::istreambuf_iterator<char> input_iterator;
	typedef boost::spirit::multi_pass<input_iterator> forward_iterator;
	typedef classic::position_iterator2<forward_iterator> iterator;

	std::ifstream file(filename.c_str());
	input_iterator input(file);
	forward_iterator forward = boost::spirit::make_default_multi_pass(input);
	forward_iterator forward_end;

	iterator iter(forward, forward_end, filename);
	iterator end;

	BOOST_AUTO(comment, "//" >> *(ascii::char_ - qi::eol) >> qi::eol);
	BOOST_AUTO(skip, ascii::space | comment);

	try
	{
		grammar<iterator, BOOST_TYPEOF(skip)> grammar;
		return qi::phrase_parse(iter, end, grammar, skip, elements);
	}
	catch (qi::expectation_failure<iterator>& e)
	{
		typedef classic::file_position_base<std::string> position;
		const position& pos = e.first.get_position();

		std::stringstream msg;
		msg << "parse error at file " << pos.file
				<< " line " << pos.line
				<< " column " << pos.column << std::endl
				<< "'" << e.first.get_currentline() << "'" << std::endl
				<< std::setw(pos.column) << " " << "^- here";

		throw std::runtime_error(msg.str());
	}
}

} // namespace junk
