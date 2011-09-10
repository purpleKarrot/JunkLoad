//

#include <zix/z_index.hpp>
#include <boost/qvm/all.hpp>

#ifndef ZIX_Z_ORDER_INT_HPP
#define ZIX_Z_ORDER_INT_HPP

namespace zix
{

template<typename Vector>
inline typename boost::enable_if<boost::qvm::is_v<Vector>, bool>::type //
zorder_less_int(const Vector& a_, const Vector& b_)
{
	typedef typename boost::qvm::v_traits<Vector> v_traits;

	unsigned int a[v_traits::dim];
	unsigned int b[v_traits::dim];

	for (int i = 0; i < v_traits::dim; ++i)
	{
		a[i] = discretize(v_traits::ir(i, a_));
		b[i] = discretize(v_traits::ir(i, b_));
	}

	int j = 0;
	int x = 0;

	for (int k = 0; k < v_traits::dim; ++k)
	{
		int y = a[k] ^ b[k];

		if (x < y && x < (x ^ y))
		{
			j = k;
			x = y;
		}
	}

	return a[j] - b[j];
}

} // namespace zix

#endif /* ZIX_Z_ORDER_INT_HPP */
