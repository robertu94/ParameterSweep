#include <memory>
template <class T>
class RandomNumberGenerator
{
	public:
	using result_type = T;
	virtual T operator ()()=0;
	virtual ~RandomNumberGenerator()=default;
};


template <class RandomNumberEngine, class RandomNumberDistribution>
class Distribution: public RandomNumberGenerator<typename RandomNumberDistribution::result_type>
{
	RandomNumberDistribution dist;
	RandomNumberEngine gen;

	public:
	using param_type = typename RandomNumberDistribution::param_type;
	Distribution(RandomNumberEngine gen, RandomNumberDistribution dist): dist(dist), gen(gen) {}

	typename RandomNumberDistribution::result_type operator()() override { return dist(gen); }

};

template <class RandomNumberEngine, class RandomNumberDistribution>
auto make_dist(RandomNumberEngine gen, RandomNumberDistribution dist)
{
	return std::make_unique<Distribution<RandomNumberEngine,RandomNumberDistribution>>(gen,dist);
}
