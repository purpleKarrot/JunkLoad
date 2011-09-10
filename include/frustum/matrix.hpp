//Copyright (c) 2010 Daniel Pfeifer

#ifndef FRUSTUM_MATRIX_HPP
#define FRUSTUM_MATRIX_HPP

#include <vmmlib/matrix.hpp>
#include <boost/qvm/m_traits.hpp>

namespace frustum
{

typedef vmml::matrix<4, 4, float> Matrix;

} // namespace frustum

namespace boost
{
namespace qvm
{

template<size_t M, size_t N, typename T>
struct matrix_traits<vmml::matrix<M, N, T> >
{
	typedef vmml::matrix<M, N, T> matrix_type;

	static const int rows = M;
	static const int cols = N;

	typedef T scalar_type;

	template<int Row, int Col>
	static scalar_type r(const matrix_type& m)
	{
		BOOST_STATIC_ASSERT(Row >= 0);
		BOOST_STATIC_ASSERT(Row < rows);
		BOOST_STATIC_ASSERT(Col >= 0);
		BOOST_STATIC_ASSERT(Col < cols);
		return m.at(Col, Row);
	}

	template<int Row, int Col>
	static scalar_type& w(matrix_type& m)
	{
		BOOST_STATIC_ASSERT(Row >= 0);
		BOOST_STATIC_ASSERT(Row < rows);
		BOOST_STATIC_ASSERT(Col >= 0);
		BOOST_STATIC_ASSERT(Col < cols);
		return m.at(Col, Row);
	}

	static scalar_type ir(int row, int col, const matrix_type& m)
	{
		BOOST_ASSERT(row >= 0);
		BOOST_ASSERT(row < rows);
		BOOST_ASSERT(col >= 0);
		BOOST_ASSERT(col < cols);
		return m.at(col, row);
	}

	static scalar_type& iw(int row, int col, matrix_type& m)
	{
		BOOST_ASSERT(row >= 0);
		BOOST_ASSERT(row < rows);
		BOOST_ASSERT(col >= 0);
		BOOST_ASSERT(col < cols);
		return m.at(col, row);
	}
};

} // namespace qvm
} // namespace boost

#endif /* FRUSTUM_MATRIX_HPP */
