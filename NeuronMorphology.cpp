#include "NeuronMorphology.h"

NeuronMorphology::NeuronMorphology(NeuronType nType, int cSize, int cCount)
{
	type = nType;
	clusterSize = cSize;
	clusterCount = cCount;
}

NeuronMorphology::~NeuronMorphology(void)
{
}
