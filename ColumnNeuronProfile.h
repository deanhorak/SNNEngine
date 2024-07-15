#pragma once
#include <vector>
#include "NeuronMorphology.h"

class ColumnNeuronProfile
{
public:
	ColumnNeuronProfile(void);
	~ColumnNeuronProfile(void);

	std::vector<NeuronMorphology *> neuronMorphology;
};
