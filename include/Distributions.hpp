template <class RandomNumberEngine, class RandomNumberDistribution>
std::unique_ptr<RandomNumberGenerator> make_dist(RandomNumberEngine gen, RandomNumberDistribution dist)
{
	return std::make_unique<Distribution<RandomNumberEngine,RandomNumberDistribution>>(gen,dist);
}


template <class T>
class RandomNumberGenerator
{
	public:
	virtual T operator ()()=0;
	virtual ~RandomNumberGenerator()=default;
};


template <class RandomNumberEngine, class RandomNumberDistribution>
class Distribution: public RandomNumberGenerator<RandomNumberDistribution::result_type>
{
	RandomNumberDistribution dist;
	RandomNumberEngine gen;

	public:
	Distribution(RandomNumberEngine gen, RandomNumberDistribution dist): dist(dist), gen(gen) {}

	RandomNumberDistribution::result_type operator()() override { return dist(gen); }

};
