/*
 * qi.hpp
 *
 *  Created on: Apr 9, 2011
 *      Author: daniel
 */

#ifndef JUNKLOAD_SPIRIT_QI_HPP
#define JUNKLOAD_SPIRIT_QI_HPP

#define FUSION_MAX_VECTOR_SIZE 16
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>

#include "adapted.hpp"
#include "../data_types.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace classic = boost::spirit::classic;

namespace stream_process
{
namespace parser
{

struct scalar_symbols: qi::symbols<char, data_type_id>
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

struct endian_policy: qi::bool_policies<>
{
    template<typename Iterator, typename Attribute>
	static bool parse_false(Iterator& first, Iterator const& last, Attribute& attr)
	{
		if (qi::detail::string_parse("little_endian", first, last, qi::unused))
		{
			boost::spirit::traits::assign_to(false, attr);
			return true;
		}
		return false;
	}

    template<typename Iterator, typename Attribute>
	static bool parse_true(Iterator& first, Iterator const& last, Attribute& attr)
	{
		if (qi::detail::string_parse("big_endian", first, last, qi::unused))
		{
			boost::spirit::traits::assign_to(true, attr);
			return true;
		}
		return false;
	}
};

template<typename Iterator, typename Skipper>
struct grammar: qi::grammar<Iterator, header(), Skipper>
{
	grammar() :
		grammar::base_type(start)
	{
		start %= qi::eps
			> "transform" > mat4d_
			> "min" > vec3d_
			> "max" > vec3d_
			> "byteorder" > endian
			> *element_
			> qi::eoi
			;

		element_
			%= string_
			> qi::uint_
			> '{'
			> *attribute_
			> '}'
			;

		attribute_
			%= string_ > scalar_ > qi::uint_ > ';'
			;

		vec3d_
			%= '[' > qi::double_ > qi::double_ > qi::double_ > ']'
			;

		mat4d_
			%= '['
			> qi::double_ > qi::double_ > qi::double_ > qi::double_
			> qi::double_ > qi::double_ > qi::double_ > qi::double_
			> qi::double_ > qi::double_ > qi::double_ > qi::double_
			> qi::double_ > qi::double_ > qi::double_ > qi::double_
			> ']'
			;

		string_
			%= qi::lexeme[+(ascii::alnum | '_')]
			;
	}

	qi::rule<Iterator, header(), Skipper> start;
	qi::rule<Iterator, element(), Skipper> element_;
	qi::rule<Iterator, attribute(), Skipper> attribute_;
	qi::rule<Iterator, vec3d(), Skipper> vec3d_;
	qi::rule<Iterator, mat4d(), Skipper> mat4d_;
	qi::rule<Iterator, std::string(), Skipper> string_;

	scalar_symbols scalar_;
	qi::bool_parser<bool, endian_policy> endian;
};

} // namespace parser

bool load_header(std::istream& in, header& h)
{
	typedef std::istreambuf_iterator<char> input_iterator;
	typedef boost::spirit::multi_pass<input_iterator> forward_iterator;
	typedef classic::position_iterator2<forward_iterator> iterator;

	input_iterator input(in);
	forward_iterator forward = boost::spirit::make_default_multi_pass(input);
	forward_iterator forward_end;

	iterator iter(forward, forward_end, "junkfile");
	iterator end;

	BOOST_AUTO(comment, '#' >> *(ascii::char_ - qi::eol) >> qi::eol);
	BOOST_AUTO(skip, ascii::space | comment);

	try
	{
		parser::grammar<iterator, BOOST_TYPEOF(skip)> g;
		return qi::phrase_parse(iter, end, g, skip, h);
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

} // namespace stream_process

#endif /* JUNKLOAD_SPIRIT_QI_HPP */
