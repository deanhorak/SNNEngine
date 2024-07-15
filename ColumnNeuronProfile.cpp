#include "ColumnNeuronProfile.h"

ColumnNeuronProfile::ColumnNeuronProfile(void)
{
	for(unsigned int i=0;i<6;i++)
	{
		neuronMorphology.push_back(new NeuronMorphology(Pyramidal, 100, 10));
	}
}

ColumnNeuronProfile::~ColumnNeuronProfile(void)
{
	for(unsigned int i=0;i<6;i++)
	{
		delete neuronMorphology[i];
	}
}
