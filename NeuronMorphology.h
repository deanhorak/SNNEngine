#pragma once
#include "Neuron.h"

class NeuronMorphology
{
public:
	NeuronMorphology(NeuronType nType=Pyramidal, int cSize=100, int cCount=10);
	~NeuronMorphology(void);

	NeuronType type;
	int clusterSize;
	int clusterCount;
};
