#pragma once
#include <memory>
#include <tuple>
#include <random>
template <class T>
class RandomNumberGenerator
{
	public:
	using result_type = T;
	virtual T operator ()()=0;
	virtual void seed(T value)=0;
	virtual void seed(size_t value)=0;
	virtual void seed(std::seed_seq seq)=0;
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
	void seed(typename RandomNumberDistribution::result_type value) override { return gen.seed(value); }
	void seed(size_t value) override { return gen.seed(value); }
	void seed(std::seed_seq seq) override { return gen.seed(seq); }

};

template <class RandomNumberEngine, class RandomNumberDistribution>
auto make_dist(RandomNumberEngine gen, RandomNumberDistribution dist)
{
	return std::make_unique<Distribution<RandomNumberEngine,RandomNumberDistribution>>(gen,dist);
}

template <class RandomNumberDistribution, class Tuple>
std::shared_ptr<
  RandomNumberGenerator<typename RandomNumberDistribution::result_type>>
make_dist_from_tuple(Tuple&& tuple)
{
  std::seed_seq seeds;
  std::minstd_rand gen(seeds);
  auto dist =
    std::make_from_tuple<RandomNumberDistribution>(std::forward<Tuple>(tuple));
  return make_dist(gen, dist);
}
