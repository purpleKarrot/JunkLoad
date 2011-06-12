//

#include <junk/mapped_data_set.hpp>

#include <boost/qvm/all.hpp>
using namespace boost::qvm;

typedef vec<float, 3> position;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "usage: fix_scale <junkfile>" << std::endl;
		return 0;
	}

	// load file
	junk::mapped_data_set data_set_(argv[1]);

	junk::mapped_data_element& mesh_ = data_set_.get_vertex_map();

	junk::attribute_accessor<position> get_position(get_attribute(data_set_.get_vertex_element(), "position").offset);

	typedef junk::mapped_data_element::iterator iterator;
	iterator begin = mesh_.begin();
	iterator end = mesh_.end();

	position lower_left = get_position(*begin);
	position upper_right = get_position(*begin);

	// calculate bounding box
	for (iterator i = begin; i != end; ++i)
	{
		position& pos = get_position(*i);
		for (size_t i = 0; i < 3; ++i)
		{
			lower_left.a[i] = std::min(lower_left.a[i], pos.a[i]);
			upper_right.a[i] = std::max(upper_right.a[i], pos.a[i]);
		}
	}

	// find largest dimension and determine scale factor
	float factor = 0.0f;
	for (size_t i = 0; i < 3; ++i)
		factor = std::max(factor, upper_right.a[i] - lower_left.a[i]);

	factor = 2.f / factor;

	// determine scale offset
	position offset = (lower_left + upper_right) * 0.5f;

	// scale the data
	for (iterator i = begin; i != end; ++i)
	{
		position& pos = get_position(*i);
		pos -= offset;
		pos *= factor;
	}
}
