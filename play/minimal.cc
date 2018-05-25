/*
 * A minimal library example
 */
#include <vector>
#include <ParameterSweep.hpp>

int main(int argc, char *argv[])
{
	std::vector<int> i = {1,2,3};
	std::vector<float> f = {1.1f,2.2f,3.3f};
	ParameterSweep::Builder builder(i,f);
	builder.set_replicants(1);
	builder.set_design(ParameterSweep::Design::OneAtATime);

	for(auto const& [int_value, float_value]: builder)
	{
		std::cout << int_value << " " << float_value << std::endl;
	}

	return 0;
}
