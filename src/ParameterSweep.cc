#include <ParameterSweep.hpp>
#include <iostream>

namespace ParameterSweep {
std::ostream& operator<<(std::ostream& out, Order const& order)
{
	switch(order)
	{
		case Order::Default:
		return out << "default";
		case Order::Sorted:
		return out << "sorted";
	}
}


std::ostream& operator<<(std::ostream& out, Design const& design)
{
	switch(design)
	{
		case Design::FullFactorial:
		return out << "full factorial";
		case Design::OneAtATime:
		return out << "one at a time";
	}
}

}
