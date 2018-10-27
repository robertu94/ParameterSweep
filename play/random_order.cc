#include "RandomOrder.hpp"
#include <vector>
#include <iostream>

int main(int argc, char *argv[])
{
	std::vector<int> v(11);
	std::iota(v.begin(), v.end(), 0);
	RandomOrderA<decltype(v)> random(v);

	for (auto const& i : random) {
		std::cout << i << std::endl;
	}


	return 0;
}
