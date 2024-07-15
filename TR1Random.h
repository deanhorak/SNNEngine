#pragma once
#include <boost/random/mersenne_twister.hpp>

class TR1Random
{
public:
	TR1Random(void);
	~TR1Random(void);
	boost::random::mt19937 gen;
	int generate(int rangeMin,int rangeMax);
};

#ifndef noexterntr1random
#define noexterntr1random 1
extern TR1Random *tr1random;
#endif