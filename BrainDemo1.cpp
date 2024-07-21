/*
 * Proprietary License
 * 
 * Copyright (c) 2024 Dean S Horak
 * All rights reserved.
 * 
 * This software is the confidential and proprietary information of Dean S Horak ("Proprietary Information").
 * You shall not disclose such Proprietary Information and shall use it only in accordance with the terms
 * of the license agreement you entered into with Dean S Horak.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are not permitted
 * without express written permission from Dean S Horak.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * If you have any questions about this license, please contact Your Name at dean.horak@gmail.com.
 */

#include "Global.h"
#include "BrainDemo1.h"

BrainDemo1::BrainDemo1(void)
{
}

BrainDemo1::~BrainDemo1(void)
{
}

Brain * BrainDemo1::create(bool rebuild) 
{

	Brain *brain; 

	if(!rebuild) // if not rebuilding, just return brain
	{
		brain = Brain::load();
		// configure counters
		return brain;
	}

	brain = Brain::create();


	brain->initializeRandom();

// Initial network created. 
// Now, attach synapses to axons within clusters
	for (globalObject->clusterDB.begin(); globalObject->clusterDB.more(); globalObject->clusterDB.next())
	{
		Cluster *cluster = globalObject->clusterDB.getValue();
		size_t size = cluster->getNeurons().size();
		for(size_t i=0;i<size;i++)
		{
			Neuron *neuronA = globalObject->neuronDB.getComponent(cluster->getNeurons()[i]);
			for(size_t j=0;j<size;j++)
			{
				if(i!=j)
				{
					Neuron *neuronB = globalObject->neuronDB.getComponent(cluster->getNeurons()[j]);
					neuronA->connectTo(neuronB);
				}
			}
		}
	}
// Now, attach clusters within a layer 
	for (globalObject->layerDB.begin(); globalObject->layerDB.more(); globalObject->layerDB.next())
	{
		Layer *layer = globalObject->layerDB.getValue();
		size_t cSize = layer->clusters.size();
		for(size_t i=0;i<cSize;i++)
		{
			for(size_t j=0;j<cSize;j++)
			{
				if(i!=j) 
				{
					Cluster *clusterA = globalObject->clusterDB.getComponent(layer->clusters[i]);
					Cluster *clusterB = globalObject->clusterDB.getComponent(layer->clusters[j]);
					clusterA->projectTo(clusterB);
				}
			}
		}
/*
		std::map<long,Cluster *>::iterator itCluster = layer->clusters.begin();
		for (itCluster=layer->clusters.begin(); itCluster!=layer->clusters.end(); ++itCluster)
		{
			std::map<long,Cluster *>::iterator itCluster2 = layer->clusters.begin();
			for (itCluster2=layer->clusters.begin(); itCluster2!=layer->clusters.end(); ++itCluster2)
			{
				if(itCluster->first != itCluster2->first)
				{
					itCluster->second->connectTo(itCluster2->second);
				}
			}
		}
*/
	}
/*
	CollectionIterator<Layer *> itLayer(Global::getLayersCollection());
	for (itLayer.begin(); itLayer.more(); itLayer.next())
	{
		Layer *layer = itLayer.value();
		std::map<long,Cluster *>::iterator itCluster = layer->clusters.begin();
		for (itCluster=layer->clusters.begin(); itCluster!=layer->clusters.end(); ++itCluster)
		{
			std::map<long,Cluster *>::iterator itCluster2 = layer->clusters.begin();
			for (itCluster2=layer->clusters.begin(); itCluster2!=layer->clusters.end(); ++itCluster2)
			{
				if(itCluster->first != itCluster2->first)
				{
					itCluster->second->connectTo(itCluster2->second);
				}
			}
		}
	}

// Now, attach layers within a column
	for (globalObject->columnDB.begin(); globalObject->columnDB.more(); globalObject->columnDB.next())
	{
		Column *column = globalObject->columnDB.getValue();
		column->initializeLayers(0);
	}
/*
// Now, attach layers within a column
	std::map<long,Column *>::iterator itColumn = globalObject->columns.begin();
	for (itColumn=globalObject->columns.begin(); itColumn!=globalObject->columns.end(); ++itColumn)
	{
		Column *column = itColumn->second;
		std::map<long,Layer *>::iterator itLayer = column->layers.begin();
		Layer *layer1 = (itLayer++)->second;
		Layer *layer2 = (itLayer++)->second;
		Layer *layer3 = (itLayer++)->second;
		Layer *layer4 = (itLayer++)->second;
		Layer *layer5 = (itLayer++)->second;
		Layer *layer6 = (itLayer)->second;
		
		// Layer 4 is input, layer 1 is output
		// Layer 4 connects to layer 3 and 5
		layer4->connectTo(layer3);
		layer4->connectTo(layer5);
		// Layer 5 connects to layer 2
		layer5->connectTo(layer2);
		// Layer 2 and 3 connects to layer 6
		layer2->connectTo(layer6);
		layer3->connectTo(layer6);
		// Layer 6 connects to layer 1
		layer6->connectTo(layer1);

	}
*/

// Now, attach columns within a Nucleus

//	CollectionIterator<Nucleus *> itNucleus(Global::getNucleiCollection());
	for (globalObject->nucleusDB.begin(); globalObject->nucleusDB.more(); globalObject->nucleusDB.next())
	{
		Nucleus *nucleus = globalObject->nucleusDB.getValue();

		size_t cSize = nucleus->columns.size();
		for(size_t i=0;i<cSize;i++)
		{
			Column *col1 = globalObject->columnDB.getComponent(nucleus->columns[i]);
			for(size_t j=0;j<cSize;j++)
			{
				if(i!=j)
				{
					Column *col2 = globalObject->columnDB.getComponent(nucleus->columns[j]);
					col1->projectTo(col2);
				}
			}
		}
	}

// Now, attach Nuclei within a Region
//	CollectionIterator<Region *> itRegion(Global::getRegionsCollection());
//	CollectionIterator<Region *> *itRegion = Global::getRegionsIterator();
	for (globalObject->regionDB.begin(); globalObject->regionDB.more(); globalObject->regionDB.next())
	{
		Region *region = globalObject->regionDB.getValue();
		size_t nSize = region->nuclei.size();
		for(size_t i=0;i<nSize;i++)
		{
			Nucleus *nuc1 = globalObject->nucleusDB.getComponent(region->nuclei[i]);
			for(size_t j=0;j<nSize;j++)
			{
				if(j!=i)
				{
					Nucleus *nuc2 = globalObject->nucleusDB.getComponent(region->nuclei[j]);
					nuc1->projectTo(nuc2);
				}
			}
		}
	}

// Finally, attach the Regions
	for (globalObject->regionDB.begin(); globalObject->regionDB.more(); globalObject->regionDB.next())
	{
		for (globalObject->regionDB.begin(1); globalObject->regionDB.more(1); globalObject->regionDB.next(1))
		{
			if(globalObject->regionDB.key() != globalObject->regionDB.key(1))
			{
				Region *r1 = globalObject->regionDB.getValue();
				Region *r2 = globalObject->regionDB.getValue(1);
				r1->projectTo(r2);
			}
		}
	}

	globalObject->flush();

	return brain;
}

void BrainDemo1::step(Brain *brain)
{
}