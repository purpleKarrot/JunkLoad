//

#include <cmath>
#include <limits>
#include <boost/qvm/all.hpp>

template<typename Vector>
inline typename boost::enable_if<boost::qvm::is_v<Vector>, bool>::type
zorder_less(const Vector& a, const Vector& b)
{
	typedef typename boost::qvm::v_traits<Vector> v_traits;

	int j = 0;
	int x = (std::numeric_limits<int>::min)();

	for (int k = 0; k < v_traits::dim; ++k)
	{
		typename v_traits::scalar_type ak = v_traits::ir(k, a);
		typename v_traits::scalar_type bk = v_traits::ir(k, b);

		if ((ak < 0) != (bk < 0))
			return ak < bk;

		int y = msdb(ak, bk);
		if (x < y)
		{
			j = k;
			x = y;
		}
	}

	return v_traits::ir(j, a) < v_traits::ir(j, b);
}

template<typename Float>
int msdb(Float a, Float b)
{
	enum { int_array_size = sizeof(Float) / sizeof(unsigned int) };

	if (a == b)
		return (std::numeric_limits<int>::min)();

	int x_exp;
	int y_exp;

	union
	{
		Float d;
		unsigned int i[int_array_size];
	} x_sig, y_sig, lzero;

	x_sig.d = std::frexp(a, &x_exp);
	y_sig.d = std::frexp(b, &y_exp);

	if (x_exp > y_exp)
		return x_exp;

	if (x_exp < y_exp)
		return y_exp;

	lzero.d = 0.5f;

	for (int i = 0; i < int_array_size; ++i)
	{
		x_sig.i[i] ^= y_sig.i[i];
		x_sig.i[i] |= lzero.i[i];
	}

	std::frexp(x_sig.d - lzero.d, &y_exp);
	return x_exp + y_exp;
}
