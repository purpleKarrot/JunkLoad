/*
 * qi.hpp
 *
 *  Created on: Apr 9, 2011
 *      Author: daniel
 */

#ifndef JUNKLOAD_SPIRIT_QI_HPP
#define JUNKLOAD_SPIRIT_QI_HPP

#include "adapted.hpp"

namespace stream_process
{

template<typename Iterator, typename Skipper>
struct header_grammar: boost::spirit::qi::grammar<Iterator, data_element(), Skipper>
{
	header_grammar() :
		header_grammar::base_type(start)
	{
		namespace qi    = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;

		start %= qi::eps
				> "ply" > qi::eol
				> "format" > format_ > qi::double_ > qi::eol
				> *element_
				> "end_header" > qi::eol;

		element_ %= *(ascii::char_ - qi::int_) > qi::int_ > '{' > *property_
				> '}';

		property_ %= "property" > (list_ | scalar_) > *(ascii::char_ - qi::eol) > qi::eol;

		list_ %= "list" > size_ > scalar_;
	}

	boost::spirit::qi::rule<Iterator, ply::header(),   Skipper> start;
	boost::spirit::qi::rule<Iterator, ply::element(),  Skipper> element_;
	boost::spirit::qi::rule<Iterator, ply::property(), Skipper> property_;
	boost::spirit::qi::rule<Iterator, ply::list(),     Skipper> list_;

	size_symbols   size_;
	scalar_symbols scalar_;
	format_symbols format_;
};

} // namespace stream_process

#endif /* JUNKLOAD_SPIRIT_QI_HPP */
