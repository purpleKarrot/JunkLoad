#ifndef __STREAM_PROCESS__SP_TYPES__HPP__
#define __STREAM_PROCESS__SP_TYPES__HPP__

#include <stream_process/VMMLibIncludes.h>

#include <stream_process/stream_data.hpp>
#include <stream_process/operator_base.hpp>
#include <stream_process/stream_operator.hpp>
#include <stream_process/chain_operator.hpp>
#include <stream_process/neighbors.hpp>
#include <stream_process/job.hpp>

#include <cstddef>

#define STREAM_PROCESS_TYPES \
    typedef sp_types_t                          sp_types; \
    typedef typename sp_types::slice_type       slice_type; \
    typedef typename sp_types::sp_float_type    sp_float_type; \
    typedef typename sp_types::hp_float_type    hp_float_type; \
    typedef typename sp_types::vec2             vec2; \
    typedef typename sp_types::vec2hp           vec2hp; \
    typedef typename sp_types::vec3             vec3; \
    typedef typename sp_types::vec3hp           vec3hp; \
    typedef typename sp_types::vec4             vec4; \
    typedef typename sp_types::vec4hp           vec4hp; \
    typedef typename sp_types::mat3             mat3; \
    typedef typename sp_types::mat3hp           mat3hp; \
    typedef typename sp_types::mat4             mat4; \
    typedef typename sp_types::mat4hp           mat4hp; \
    typedef operator_base< sp_types >           op_base_type; \
    typedef stream_operator< op_base_type >     stream_op_type; \
    typedef chain_operator< op_base_type >      chain_op_type; \
    typedef neighbors< sp_types >               nbh_type; \
    typedef typename nbh_type::iterator         nb_iterator; \
    typedef typename nbh_type::const_iterator   nb_const_iterator; \
    typedef neighbor_info< sp_types >           neighbor_type; \
    typedef vmml::vector< 3, stream_data* >     ptr_vec3; \
    typedef typename slice_type::container_type stream_container_type;
    

namespace stream_process
{

template< typename float_tx, typename hp_float_tx, typename data_slice_tx >
class stream_process_types
{
public:
	typedef float_tx                            sp_float_type;
	typedef hp_float_tx                         hp_float_type;
	typedef data_slice_tx                       slice_type;

	typedef vmml::vector< 2, sp_float_type >    vec2;
	typedef vmml::vector< 2, hp_float_type >    vec2hp;

	typedef vmml::vector< 3, sp_float_type >    vec3;
	typedef vmml::vector< 3, hp_float_type >    vec3hp;

	typedef vmml::vector< 4, sp_float_type >    vec4;
	typedef vmml::vector< 4, hp_float_type >    vec4hp;

	typedef vmml::matrix< 3, 3, sp_float_type >	mat3;
	typedef vmml::matrix< 3, 3, hp_float_type >	mat3hp;

	typedef vmml::matrix< 4, 4, sp_float_type >	mat4;
	typedef vmml::matrix< 4, 4, hp_float_type >	mat4hp;

protected:

}; // class sp_types

} // namespace stream_process

#endif

