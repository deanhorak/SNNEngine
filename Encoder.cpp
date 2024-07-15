#include "Encoder.h"
#include "Global.h"

Encoder::Encoder(Nucleus *nucleus)
{
	this->nucleus = nucleus;
}

Encoder::~Encoder(void)
{
}

void Encoder::post(Event *e)
{
//	int eSize = e->sampleSize;
/*
	int nSize = 0;
	for(size_t i=0;i<nucleus->columns.size();i++)
	{
		Column *col = nucleus->columns[i];
		for(size_t j=0;j<col->layers[3]->clusters.size();j++) // layer 4 is input
		{
			Cluster *cls = col->layers[3]->clusters[j];
			nSize += cls->neurons.size();
		}
	}
*/
	// We now know the number of neurons in this layer
	int ePos = 0;
	for(size_t i=0;i<nucleus->columns.size();i++)
	{
		if(ePos<e->sampleSize) 
		{
			Column *col = globalObject->columnDB.getComponent(nucleus->columns[i]);
			Layer *thisLayer = globalObject->layerDB.getComponent(col->layers[3]);
			size_t clSize = thisLayer->clusters.size();
			for(size_t j=0;j<clSize;j++) // layer 4 is input
			{
				if(ePos<e->sampleSize) 
				{
					Cluster *cls = globalObject->clusterDB.getComponent(thisLayer->clusters[j]);
					for(size_t k=0;k<cls->getNeurons().size();k++)
					{	
						if(ePos<e->sampleSize) 
						{
							if(e->samples[ePos++]!=0)
							{
								Neuron *neuron = globalObject->neuronDB.getComponent(cls->getNeurons()[k]);
								//
								if(globalObject->logEvents) 
								{	
									std::stringstream ss;
									ss << "encoder_post_firing: neuron=" << neuron->id;
									globalObject->writeEventLog(ss.str().c_str());
								}
								//
								neuron->fire();
							}
						}
						else
							break;
					}
				}
				else
					break;
			}
		}
		else
			break;
	}
}

Event *Encoder::get(void)
{
	size_t nSize = 0;
	for(size_t i=0;i<nucleus->columns.size();i++)
	{
		Column *col = globalObject->columnDB.getComponent(nucleus->columns[i]);
		Layer *thisLayer = globalObject->layerDB.getComponent(col->layers[0]); // layer 1 is input
		for(size_t j=0;j<thisLayer->clusters.size();j++) 
		{
			Cluster *cls = globalObject->clusterDB.getComponent(thisLayer->clusters[j]);
			nSize += cls->getNeurons().size();
		}
	}

	Event *e = new Event(nSize);

	int ePos = 0;
	for(size_t i=0;i<nucleus->columns.size();i++)
	{
		if(ePos<e->sampleSize) 
		{
			Column *col = globalObject->columnDB.getComponent(nucleus->columns[i]);
			Layer *thisLayer = globalObject->layerDB.getComponent(col->layers[0]); // layer 1 is input
			for(size_t j=0;j<thisLayer->clusters.size();j++) 
			{
				if(ePos<e->sampleSize) 
				{
					Cluster *cls = globalObject->clusterDB.getComponent(thisLayer->clusters[j]);
					for(size_t k=0;k<cls->getNeurons().size();k++)
					{	
						if(ePos<e->sampleSize) 
						{
							Neuron *neuron = globalObject->neuronDB.getComponent(cls->getNeurons()[k]);
							if(neuron->isFiring())
								e->samples[ePos++]=1;
							else
								e->samples[ePos++]=0;
						}
						else
							break;
					}
				}
				else
					break;
			}
		}
		else
			break;
	}
	return e;
}
