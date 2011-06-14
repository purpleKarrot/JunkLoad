/* Copyright (c) 2007-2011, Stefan Eilemann <eile@equalizergraphics.com> 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of Eyescale Software GmbH nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "localInitData.h"
#include <boost/program_options.hpp>

namespace eqPly
{

LocalInitData::LocalInitData() :
		model_path("/usr/share/Equalizer/data/bunny.ply"),
		shader_path("/usr/share/Equalizer/data/blinn_phong.shader")
{
}

void LocalInitData::parseArguments(const int argc, char** argv)
{
	using namespace boost::program_options;
	options_description desc("Allowed options");
	desc.add_options()
		("help", "produce this help message")
		("model,M", value<std::string> (&model_path), "path to the model")
		("shader,S", value<std::string> (&shader_path), "path to the shader")
		;

	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << "\n";
		::exit(EXIT_SUCCESS);
	}
}

} // namespace eqPly
