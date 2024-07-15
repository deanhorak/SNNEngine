#include "TR1Random.h"
#include "time.h"

#include <boost/random/uniform_int_distribution.hpp>

TR1Random::TR1Random(void)
{
	gen.seed((boost::uint32_t)time(NULL));
}

TR1Random::~TR1Random(void)
{
}

int TR1Random::generate(int rangeMin,int rangeMax)
{
	boost::random::uniform_int_distribution<> dist(rangeMin, rangeMax);
    return dist(gen);
}