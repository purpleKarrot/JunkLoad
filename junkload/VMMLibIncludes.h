#ifndef __STREAM_PROCESS__VMMLIB_INCLUDES__H__
#define __STREAM_PROCESS__VMMLIB_INCLUDES__H__

#define VMMLIB_CUSTOM_CONFIG

// enable for debugging purposes, but it's quite a performance hit for
// release versions....

// #define VMMLIB_SAFE_ACCESSORS
// #define VMMLIB_THROW_EXCEPTIONS 

#include <vmmlib/vector.hpp>
#include <vmmlib/vector_traits.hpp>

#include <vmmlib/matrix.hpp>

#include <vmmlib/jacobi_solver.hpp>
#include <boost/cstdint.hpp>

namespace junk
{

typedef vmml::vector<2, unsigned char> vec2ub;
typedef vmml::vector<2, int32_t> vec2i;
typedef vmml::vector<2, uint32_t> vec2ui;
typedef vmml::vector<2, float> vec2f;
typedef vmml::vector<2, double> vec2d;

typedef vmml::vector<3, unsigned char> vec3ub;
typedef vmml::vector<3, int32_t> vec3i;
typedef vmml::vector<3, uint32_t> vec3ui;
typedef vmml::vector<3, float> vec3f;
typedef vmml::vector<3, double> vec3d;

typedef vmml::vector<4, unsigned char> vec4ub;
typedef vmml::vector<4, int32_t> vec4i;
typedef vmml::vector<4, uint32_t> vec4ui;
typedef vmml::vector<4, float> vec4f;
typedef vmml::vector<4, double> vec4d;

typedef vmml::matrix<3, 3, float> mat3f;
typedef vmml::matrix<3, 3, double> mat3d;
typedef vmml::matrix<4, 4, float> mat4f;
typedef vmml::matrix<4, 4, double> mat4d;

} // namespace junk

#endif
