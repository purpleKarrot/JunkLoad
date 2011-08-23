#ifndef JNK_TRAITS_HPP
#define JNK_TRAITS_HPP

#include <jnk/types.hpp>
#include <boost/cstdint.hpp>
#include <boost/qvm/v_traits.hpp>
#include <boost/utility/enable_if.hpp>

namespace junk
{
namespace traits
{

template<typename T, typename Enabled = void>
struct type
{
};

template<>
struct type<boost::int8_t>
{
	static const junk::type value = junk::s_int_08;
};

template<>
struct type<boost::int16_t>
{
	static const junk::type value = junk::s_int_16;
};

template<>
struct type<boost::int32_t>
{
	static const junk::type value = junk::s_int_32;
};

template<>
struct type<boost::uint8_t>
{
	static const junk::type value = junk::u_int_08;
};

template<>
struct type<boost::uint16_t>
{
	static const junk::type value = junk::u_int_16;
};

template<>
struct type<boost::uint32_t>
{
	static const junk::type value = junk::u_int_32;
};

template<>
struct type<float>
{
	static const junk::type value = junk::float_32;
};

template<>
struct type<double>
{
	static const junk::type value = junk::float_32;
};

template<typename V>
struct type<V, typename boost::enable_if<boost::qvm::is_v<V> >::type>
{
	typedef typename boost::qvm::v_traits<V>::scalar_type scalar_type;
	static const junk::type value = type<scalar_type>::value;
};

template<typename T, typename Enabled = void>
struct size
{
	static const std::size_t value = 1;
};

template<typename V>
struct size<V, typename boost::enable_if<boost::qvm::is_v<V> >::type>
{
	static const std::size_t value = boost::qvm::v_traits<V>::dim;
};

} // namespace traits
} // namespace junk

#endif /* JNK_TRAITS_HPP */
