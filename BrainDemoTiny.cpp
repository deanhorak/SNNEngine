/*
 * Proprietary License
 * 
 * Copyright (c) 2024-2025 Dean S Horak
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
#include "TR1Random.h"
#include "BrainDemoTiny.h"
#include "SNNEngine.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>


BrainDemoTiny::BrainDemoTiny(void)
{
}

BrainDemoTiny::~BrainDemoTiny(void)
{
}

Brain * BrainDemoTiny::create(bool rebuild) 
{

	Brain *brain; 

	std::stringstream ss;

	if(!rebuild) // if not rebuilding, just return brain
	{
		LOGSTREAM(ss) << " Loading brain" << std::endl;
		globalObject->log(ss);
		brain = Brain::load("../../../database/","BrainDemoTiny");
		globalObject->readCounters();
	} 
	else 
	{
		LOGSTREAM(ss) << " Creating brain" << std::endl;
		globalObject->log(ss);
		brain = Brain::create(true,"../../../database/","BrainDemoTiny");

	}

	/*
	enum ComponentType {ComponentTypeUnknown=0, ComponentTypeBrain, ComponentTypeRegion, ComponentTypeNucleus, ComponentTypeColumn, ComponentTypeLayer, 
					ComponentTypeCluster, ComponentTypeNeuron, ComponentTypeAxon, ComponentTypeDendrite, ComponentTypeSynapse, 
					ComponentTypeActionPotential, ComponentTypeTimedEvent};
	#define CTYPE_COUNT 13
	*/


	SpatialDetails sdregionDigits(-10000, 5000, 5000, 5000, 5000, 5000); // Dummy test locations/size
	SpatialDetails sdnucleusDigits(-10000, 5000, 5000, 5000, 5000, 5000); // Dummy test locations/size

	SpatialDetails sdregionAssociative(5000, 0, -2500, 5000, 5000, 5000); // Dummy test locations/size
	SpatialDetails sdnucleusAssociative(5000, 0, -2500, 5000, 5000, 5000); // Dummy test locations/size

	SpatialDetails sdregionImages(-10000, -5000, -5000, 5000, 5000, 5000); // Dummy test locations/size
	SpatialDetails sdnucleusImages(-10000, -5000, -5000, 5000, 5000, 5000); // Dummy test locations/size

	// Create Thalamus
	LOGSTREAM(ss) << "Create regionDigits region... " << std::endl;
	globalObject->log(ss);

	// Digits - 10 neurons
	Region *regionDigits = 0L;
	if(brain->restartpoint())
	{
		regionDigits = Region::create("regionDigits", sdregionDigits);
	} 
	else 
	{
		long regionId = globalObject->componentBase[ComponentTypeRegion]; 
		regionDigits = globalObject->regionDB.getComponent(regionId);
		globalObject->insert(regionDigits);

		LOGSTREAM(ss) << "regionDigits globalObject->regionDB.getComponent(" << regionId << ") " << std::endl;
		globalObject->log(ss);
	}
	brain->syncpoint();


	// Create Thalamic Nuclei
	ColumnNeuronProfile profile; // default profile for all layers is Pyramidal neurons, 100 neurons per cluster, with 10 clusters

	Nucleus *nucleusDigits = 0L;
	if(brain->restartpoint())
	{
		nucleusDigits = Nucleus::create("nucleusDigits", sdnucleusDigits);
		nucleusDigits->nucleusType = MOTOR_NUCLEUS;
		regionDigits->add(nucleusDigits);
//		regionDigits->addColumns(10,profile); // 10 columns, each with 6 layers, each with 5 clusters, each with 10 neurons
//		regionDigits->addColumns(1,1,10); // 1 column, each with 6 layers, each with 1 clusters, each with 10 neurons
		nucleusDigits->addColumns(1,1,1,10); // 1 column, each with 2 layers, each with 1 clusters, each with 10 neurons


		float minX = nucleusDigits->location.x;
		float maxX = minX + nucleusDigits->area.w;
		float minZ = nucleusDigits->location.z;
		float maxZ = minZ + nucleusDigits->area.d;

		float spaceX = (nucleusDigits->area.w / 10); 

		Column *column = globalObject->columnDB.getComponent(nucleusDigits->columns[0]);
		Layer *layer = globalObject->layerDB.getComponent(column->layers[0]);
		std::vector<long> clusters = layer->clusters;
		int clusterCount = clusters.size();
		float xCoord = minX;
		float zCoord = minZ;
		for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
		{
			Cluster *cluster = globalObject->clusterDB.getComponent(clusters[clusterIndex]);
			std::vector<long> neurons = cluster->getNeurons();
			int nCount = neurons.size();
			float thisXCoord = xCoord;
			for (int nIndex = 0; nIndex < nCount; nIndex++)
			{
				Neuron *neuron = globalObject->neuronDB.getComponent(neurons[nIndex]);
				neuron->location.x = xCoord;
				neuron->location.z = zCoord;
				xCoord += spaceX;
			}
		}
	} 
	else 
	{
		long nucleusId = globalObject->componentBase[ComponentTypeNucleus]; 
		nucleusDigits = globalObject->nucleusDB.getComponent(nucleusId);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionDigits->name << " complete with " << regionDigits->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	// Images - 784 neurons
	Region *regionImages = 0L;
	if(brain->restartpoint())
	{
		regionImages = Region::create("regionImages", sdregionImages);
	}
	else
	{
		long regionId = globalObject->componentBase[ComponentTypeRegion] + 1; 
		regionImages = globalObject->regionDB.getComponent(regionId);
		globalObject->insert(regionImages);
	}
	brain->syncpoint();

	Nucleus *nucleusImages = 0L;
	if(brain->restartpoint())
	{
		nucleusImages = Nucleus::create("nucleusImages", sdnucleusImages);
		nucleusImages->nucleusType = SENSORY_NUCLEUS;

		regionImages->add(nucleusImages);
//		nucleusImages->addColumns(1,1,784); // 1 column, each with 6 layers, each with 1 clusters, each with 784 neurons
		nucleusImages->addColumns(1,1,784,8); // 1 column, each with 1 layers, each with 1 clusters, each with 784*8 neurons

		int totalChangedCount = 0;
		float minX = nucleusImages->location.x;
		float maxX = minX + nucleusImages->area.w;
		float minZ = nucleusImages->location.z;
		float maxZ = minZ + nucleusImages->area.d;
		float spaceX = (nucleusImages->area.w / 28) / 4; // 28 groupings of 8 neurons
		float spaceZ = (nucleusImages->area.d / 28) / 2; // 28 groupings of 8 neurons
		Column *column = globalObject->columnDB.getComponent(nucleusImages->columns[0]);
		Layer *layer = globalObject->layerDB.getComponent(column->layers[0]);
		std::vector<long> clusters = layer->clusters;
		int clusterCount = clusters.size();
		float xCoord = minX;
		float zCoord = minZ;
		int row =0;
		for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
		{
			Cluster *cluster = globalObject->clusterDB.getComponent(clusters[clusterIndex]);
			std::vector<long> neurons = cluster->getNeurons();
			int nCount = neurons.size();
			float thisXCoord = xCoord;
			for (int nIndex = 0; nIndex < nCount; nIndex++)
			{
				Neuron *neuron = globalObject->neuronDB.getComponent(neurons[nIndex]);

				neuron->location.x = xCoord;
				neuron->location.z = zCoord;
				xCoord += spaceX;
				totalChangedCount++;
			}
			xCoord += spaceX*2;
			if ((clusterIndex+1) % 28*8 == 0)
			{
				xCoord = minX;
				zCoord += spaceZ * 2;
			}

		}

		std::cout << "Total Neurons Adjusted: " << totalChangedCount << std::endl;
	}

	else
	{
		long nucleusId = globalObject->componentBase[ComponentTypeNucleus] + 1; 
		nucleusImages = globalObject->nucleusDB.getComponent(nucleusId);
	} 
	brain->syncpoint();


	// Associative region - 10 neurons
	Region *regionAssociative = 0L;
	if(brain->restartpoint())
	{
		regionAssociative = Region::create("regionAssociative", sdregionAssociative);
	} 
	else 
	{
		long regionId = globalObject->componentBase[ComponentTypeRegion] + 2; 
		regionAssociative = globalObject->regionDB.getComponent(regionId);
		globalObject->insert(regionAssociative);

		LOGSTREAM(ss) << "regionAssociative globalObject->regionDB.getComponent(" << regionId << ") " << std::endl;
		globalObject->log(ss);
	}
	brain->syncpoint();


	Nucleus *nucleusAssociative = 0L;
	if(brain->restartpoint())
	{
		nucleusAssociative = Nucleus::create("nucleusAssociative", sdnucleusAssociative);
		nucleusAssociative->nucleusType = MOTOR_NUCLEUS;
		regionAssociative->add(nucleusAssociative);
//		regionDigits->addColumns(10,profile); // 10 columns, each with 6 layers, each with 5 clusters, each with 10 neurons
//		regionDigits->addColumns(1,1,10); // 1 column, each with 6 layers, each with 1 clusters, each with 10 neurons
		int numNeurons = 10; // 10 neurons in each cluster
		nucleusAssociative->addColumns(5,6,5,numNeurons); // 1 column, each with 2 layers, each with 1 clusters, each with 10 neurons
	} 
	else 
	{
		long nucleusId = globalObject->componentBase[ComponentTypeNucleus]; 
		nucleusAssociative = globalObject->nucleusDB.getComponent(nucleusId);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionDigits->name << " complete with " << regionDigits->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);


	LOGSTREAM(ss) << "Region " << regionImages->name << " complete with " << regionImages->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << "Region " << regionAssociative->name << " complete with " << regionAssociative->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	
	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << formatNumber(globalObject->regionsSize()) << " regions created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->nucleiSize()) << " nuclei created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->columnsSize()) << " columns created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->layersSize()) << " layers created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->clustersSize()) << " clusters created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->neuronsSize()) << " neurons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->axonsSize()) << " axons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << "Attach regions" << std::endl;
	globalObject->log(ss);
// Finally, attach the Regions


	// receiveInputFrom means that the that the source extends dendrites to receive input from the target

	// Digits receiveInputFrom images
	LOGSTREAM(ss) << "    regionDigits->receiveInputFrom(regionImages)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionDigits->receiveInputFrom(regionImages,100.f,EXCITATORY_SYNAPSE);
	}
	brain->syncpoint();

	// Associative receiveInputFrom to images
	LOGSTREAM(ss) << "    regionAssociative->receiveInputFrom(regionImages)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionAssociative->receiveInputFrom(regionImages,100.f,EXCITATORY_SYNAPSE);
	}
	brain->syncpoint();

	// Associative receiveInputFrom to digits
	LOGSTREAM(ss) << "    regionAssociative->receiveInputFrom(regionDigits)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionAssociative->receiveInputFrom(regionDigits,100.f,EXCITATORY_SYNAPSE); // Inhibitory synapses!
	}
	brain->syncpoint();

	// Digits receiveInputFrom to associative
	LOGSTREAM(ss) << "    regionDigits->receiveInputFrom(regionAssociative)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionDigits->receiveInputFrom(regionAssociative,100.f,INHIBITORY_SYNAPSE);	// Inhibitory synapses!
	}
	brain->syncpoint();


	LOGSTREAM(ss) << "------------------------------------------------------" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->dendritesSize()) << " dendrites created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->synapsesSize()) << " synapses created" << std::endl;
	globalObject->log(ss);

	

	if(true)
	{
		long start_neuron = regionDigits->getStartNeuron();
		long end_neuron = regionDigits->getEndNeuron();
		LOGSTREAM(ss) << "          regionDigits " << (end_neuron - start_neuron +1 ) << " neurons [" << start_neuron << " - " << end_neuron << "]" << std::endl;
		globalObject->log(ss);


		start_neuron = regionImages->getStartNeuron();
		end_neuron = regionImages->getEndNeuron();
		LOGSTREAM(ss) << "          regionImages " << (end_neuron - start_neuron + 1) << " neurons [" << start_neuron << " - " << end_neuron << "]" << std::endl;
		globalObject->log(ss);

		start_neuron = regionAssociative->getStartNeuron();
		end_neuron = regionAssociative->getEndNeuron();
		LOGSTREAM(ss) << "          regionAssociative " << (end_neuron - start_neuron + 1) << " neurons [" << start_neuron << " - " << end_neuron << "]" << std::endl;
		globalObject->log(ss);
	}


	unsigned long zeroAxonCount = 0;
	unsigned long zeroDendriteCount = 0;
//	CollectionIterator<Axon *> itAxon(Global::getAxonsCollection());
//	for (itAxon.begin(); itAxon.more(); itAxon.next()) 
	long axonIdStart = globalObject->componentBase[ComponentTypeAxon];
	long axonIdEnd = globalObject->componentCounter[ComponentTypeAxon];

	for (long axonId = axonIdStart; axonId < axonIdEnd; axonId++)
	{
		Axon *axon = globalObject->axonDB.getComponent(axonId);
		
		if(axon->getSynapses()->size()==0) {
			zeroAxonCount++;
//			std::cout << "BrainDemoHWRecognition.create: Axon " << axon->id << " has no synapses." << std::endl;

		}
		long neuronId = axon->neuronId;
		Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
		if(neuron->getDendrites()->size()==0) {
			zeroDendriteCount++;
//			std::cout << "BrainDemoHWRecognition.create: Axon " << axon->id << " has no synapses." << std::endl;

		}
	}
	LOGSTREAM(ss) << formatNumber(zeroAxonCount) << " axons have no synapses." << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << formatNumber(zeroDendriteCount) << " neurons have no dendrites." << std::endl;
	globalObject->log(ss);

	// Make final Adustments
	//finalAxonAdjustments(ss);

	//finalDendriteAdjustments(ss);

	long nucleusIdStart = globalObject->componentBase[ComponentTypeNucleus];
	long nucleusIdEnd = globalObject->componentCounter[ComponentTypeNucleus];

	for (long nucleusId = nucleusIdStart; nucleusId < nucleusIdEnd; nucleusId++)
	{
		Nucleus *nuc = globalObject->nucleusDB.getComponent(nucleusId);
		std::vector<long> neurons = Server::getNeurons(nuc->name, LayerType::input); // only one layer for now
		long startNeuronId = globalObject->componentBase[ComponentTypeAxon] - 1; // highest possible neuronId
		long endNeuronId = 0;
		for(size_t nIndex = 0; nIndex < neurons.size(); nIndex++)
		{
			long thisNeuronId = neurons[nIndex];

			if(thisNeuronId < startNeuronId)
				startNeuronId = thisNeuronId;

			if(thisNeuronId > endNeuronId)
				endNeuronId = thisNeuronId;

			if(nuc->name.compare("nucleusAssociative")==0) // make associative neurons inhibitory
			{
				Neuron *n = globalObject->neuronDB.getComponent(thisNeuronId);
				n->neuronPolarity = Polarity::INHIBITORY_NEURON;
			}
		}

		LOGSTREAM(ss) << "Nucleus ..." << nuc->name << ": beginning neuron=" << startNeuronId <<", ending neuron=" << endNeuronId << std::endl;
		globalObject->log(ss);
		globalObject->flush();
	}


	LOGSTREAM(ss) << "Flush started..." << std::endl;
	globalObject->log(ss);
	globalObject->flush();
	LOGSTREAM(ss) << "Flush complete..." << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << "SNNEngine ready to accept input..." << std::endl;
	globalObject->log(ss);

	globalObject->logStructure();
//	globalObject->logStructure(regionDigits);
//	globalObject->logStructure(nucleusImages);


	return brain;
}
/*
void BrainDemoTiny::experiments(Nucleus *nucleusImages)
{
	// Set any clusters beyond the first cluster to inhibitory
	size_t colSize = nucleusImages->columns.size();
	for (size_t i = 0; i < colSize; i++)
	{
		long thisColId = nucleusImages->columns[i];
		Column *thisCol = globalObject->columnDB.getComponent(thisColId);
		size_t layerSize = thisCol->layers.size();
		for (size_t j = 0; j < layerSize; j++)
		{
			long thisLayerId = thisCol->layers[j];
			Layer *thisLayer = globalObject->layerDB.getComponent(thisLayerId);
			size_t clusterSize = thisLayer->clusters.size();
			for (size_t k = 0; k < clusterSize; k++)
			{
				if (k != 0) // only change those clusters beyond the first
				{
					long thisClusterId = thisLayer->clusters[k];
					Cluster *thisCluster = globalObject->clusterDB.getComponent(thisClusterId);
					size_t neuronsSize = thisCluster->neurons.size();
					for (size_t m = 0; m < neuronsSize; m++)
					{
						long thisNeuronId = thisCluster->neurons[m];
						Neuron *thisNeuron = globalObject->neuronDB.getComponent(thisNeuronId);
						std::vector<long> *dendrites = thisNeuron->getDendrites();
						size_t dendritesSize = dendrites->size();
						for (size_t n = 0; n < dendritesSize; n++)
						{
							long dendriteId = (*dendrites)[n];
							Dendrite *thisDendrite = globalObject->dendriteDB.getComponent(dendriteId);
							long thisSynapseId = thisDendrite->getSynapseId();
							Synapse *thisSynapse = globalObject->synapseDB.getComponent(thisSynapseId);
							thisSynapse->polarity = INHIBITORY;
						}
					}
				}
			}
		}
	}
}
*/

void BrainDemoTiny::finalDendriteAdjustments(std::stringstream &ss)
{
	LOGSTREAM(ss) << "Making final dendrite adjustments..." << std::endl;
	globalObject->log(ss);

	long neuronIdStart = globalObject->componentBase[ComponentTypeNeuron];
	long neuronIdEnd = globalObject->componentCounter[ComponentTypeNeuron];
	for(long neuronId=neuronIdStart;neuronId<neuronIdEnd;neuronId++)
	{
		Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
		if(neuron->getDendrites()->size()==0) {
			// add dendites by connecting to neurons referenced by our axons 

			std::vector<long> *axons = neuron->getAxons();
			long mainAxonId = (*axons)[0];
			Axon *thisAxon = globalObject->axonDB.getComponent(mainAxonId);
			std::vector<long> *synapses = thisAxon->getSynapses();
			if(synapses->size()>0)
			{
				long synapseId = (*synapses)[0];
				Synapse *thisSynapse = globalObject->synapseDB.getComponent(synapseId);

				long sourceNeuronId = thisSynapse->postSynapticNeuronId; //????
				Neuron *sourceNeuron = globalObject->neuronDB.getComponent(sourceNeuronId);
				neuron->connectFrom(sourceNeuron,thisSynapse->polarity);
			}
		}
	}

	unsigned long zeroDendriteCount = 0;
//	CollectionIterator<Axon *> itAxon(Global::getAxonsCollection());
//	for (itAxon.begin(); itAxon.more(); itAxon.next()) 
	long axonIdStart = globalObject->componentBase[ComponentTypeAxon];
	long axonIdEnd = globalObject->componentCounter[ComponentTypeAxon];

	for (long axonId = axonIdStart; axonId < axonIdEnd; axonId++)
	{
		Axon *axon = globalObject->axonDB.getComponent(axonId);
		long neuronId = axon->neuronId;
		Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
		if(neuron->getDendrites()->size()==0) {
			zeroDendriteCount++;
//			std::cout << "BrainDemoHWRecognition.create: Axon " << axon->id << " has no synapses." << std::endl;

		}
	}
	LOGSTREAM(ss) << formatNumber(zeroDendriteCount) << " neurons have no dendrites after fixup." << std::endl;
	globalObject->log(ss);

}

void BrainDemoTiny::finalAxonAdjustments(std::stringstream &ss)
{
	LOGSTREAM(ss) << "Making final axon adjustments..." << std::endl;
	globalObject->log(ss);

	unsigned long zeroCount = 0;

	long axonIdStart = globalObject->componentBase[ComponentTypeAxon];
	long axonIdEnd = globalObject->componentCounter[ComponentTypeAxon];

	for (long axonId = axonIdStart; axonId < axonIdEnd; axonId++)
	{
		Axon* ax = globalObject->axonDB.getComponent(axonId);
		if (ax->getSynapses()->size() == 0)
		{
			zeroCount++;
//			std::cout << "BrainDemoHWRecognition.finalAdjustments: Axon " << ax->id << " has no synapses." << std::endl;
		}
	}

	LOGSTREAM(ss) << formatNumber(zeroCount) << " axons have no synapses before fixup." << std::endl;
	globalObject->log(ss);

	unsigned long added = 0;
	if (zeroCount > 0) {

		float pct = 0;
		float index = 1;
		//std::vector<unsigned long, unsigned long> newSynapses;
		//	CollectionIterator<Axon *> itAxon(Global::getAxonsCollection());
		//	for (itAxon.begin(); itAxon.more(); itAxon.next())
		size_t axonCount = globalObject->axonDB.size();

		for (globalObject->axonDB.begin(); globalObject->axonDB.more(); globalObject->axonDB.next())
		{
			Axon* ax = globalObject->axonDB.getValue();
			if (ax->getSynapses()->size() == 0)
			{
				// An axon with zero synapses found. Create a synapse for this guy to somewhere... anywhere
	//////////////////////////
				long neuronId = ax->neuronId;
				Neuron* neu = globalObject->neuronDB.getComponent(neuronId);				// we have the neuron in question
				long parentClusterId = neu->parentId;
				Cluster* cl = globalObject->clusterDB.getComponent(parentClusterId);			// we have the cluster that this neuron belongs to
				size_t cSize = cl->getNeurons().size();
				for (size_t i = 0; i < cSize; i++)										// Loop through each neuron in this cluster
				{
					long nId = cl->getNeurons()[i];
					if (nId > 0)
					{
						if (nId != neuronId)										// Ignore if we're looking at our own neuron
						{	// Valud neuronId and not equal to current one
							Neuron* neuX = globalObject->neuronDB.getComponent(nId);			// Get the neuron we want to attach to....
							if (neuX != NULL)												// Make sure it's valid
							{
								added++;
								Dendrite* den = Dendrite::create(neuX,neu,-1.0);		 		// Create a new inhibitory dendrite 
                                neuX->dendriteMap.insert(std::make_pair(neu->id, den->id));
								Synapse* s = Synapse::create(den,-1.0);					// Add a synapse to the dendrite -- inhibitory 
								long sid = s->id;
								ax->insertSynapse(sid);								// Attatch this new synapse to our axon
							}
							else
							{
								LOGSTREAM(ss) << "Neuron id " << formatNumber(nId) << " in cluster  " << formatNumber(parentClusterId) << " not found" << std::endl;
								globalObject->log(ss);
							}
						}
					}
					else
					{
						LOGSTREAM(ss) << "Zero neuronId found in cluster  " << formatNumber(parentClusterId) << " not found" << std::endl;
						globalObject->log(ss);
					}
				}
				//////////////////////////
			}
			pct = index / axonCount;
			printf("Pct Complete %f\r", pct * 100.0f);

			index++;
		}
	}

	// Wrap up...

	LOGSTREAM(ss) << formatNumber(added) << " synapses added." << std::endl;
	globalObject->log(ss);


	zeroCount = 0;

	for (long axonId = axonIdStart; axonId < axonIdEnd; axonId++)
	{
		Axon* ax = globalObject->axonDB.getComponent(axonId);
		if (ax->getSynapses()->size() == 0)
		{
			zeroCount++;
			std::cout << "BrainDemoHWRecognition.wrapip: Axon " << ax->id << " has no synapses." << std::endl;
		}
	}

	LOGSTREAM(ss) << formatNumber(zeroCount) << " axons have no synapses after fixup." << std::endl;
	globalObject->log(ss);


	LOGSTREAM(ss) << "Final axon adjustments complete..." << std::endl;
	globalObject->log(ss);

}



void BrainDemoTiny::step(Brain *brain)
{
	(void)brain;
//	std::cout << "Current timestamp " << globalObject->getCurrentTimestamp() << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;


	std::stringstream ss;
/****
	if(globalObject->actionPotentialsSize()>0) {
		LOGSTREAM(ss) << "Current timestamp " << globalObject->getCurrentTimestamp() << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;
		globalObject->log(ss);
	}
****/

}

std::string BrainDemoTiny::formatNumber(unsigned long long number) {
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << number;
	return ss.str();

}

// This kludge is to sprinkle synapses among the clusters within nucleus, and within the neurons within clusters
void BrainDemoTiny::insertSynapses(Nucleus* nuc) {
	for (size_t i = 0; i < nuc->columns.size(); i++) {
		long colId = nuc->columns[i];
		Column* col = globalObject->columnDB.getComponent(colId);
		for (size_t j = 0; j < col->layers.size(); j++) {
			long layerId = col->layers[j];
			Layer *lay = globalObject->layerDB.getComponent(layerId);
			// If inputlayer, polarity is inhibitory, otherwise excitatory
			float polarity = EXCITATORY_SYNAPSE;
			if(j==(size_t)col->inputLayer)
				polarity = EXCITATORY_SYNAPSE;

			for (size_t k = 0; k < lay->clusters.size(); k++) {
				long clusterId = lay->clusters[k];
				Cluster* clu = globalObject->clusterDB.getComponent(clusterId);
				for (size_t l = 0; l < clu->getNeurons().size(); l++) {
					long neuronId = clu->getNeurons()[l];
					Neuron* neu = globalObject->neuronDB.getComponent(neuronId);
					std::vector<long> *axons = neu->getAxons();
					for (size_t m = 0; m < axons->size(); m++) {
						//long axonId = (*axons)[m];
						//Axon* axon = globalObject->axonDB.getComponent(axonId);
						// We know have the axon which we will be attaching synapses to
						// This is a place holder for a more sophisticated algorythm to assign synapses. Perhaps future models with 3D information loaded in (SpatialDetails)
						// can place synapses where axons and dendrites come in close proximity
						//
						// But for now, let's just do random
						//
						// Iterate over neurons in this cluster and associate X% of them, selected at random, with the dendrites of other neurons
						// 
						//int percent = 100; // start with 98%
						//
						//boost::random::mt19937 gen;

						for (size_t ll = 0; ll < clu->getNeurons().size(); ll++) {
							unsigned long neuronId2 = clu->getNeurons()[ll];
							if (neuronId2 != neu->id) { // If not ourself
								Neuron* neu2 = globalObject->neuronDB.getComponent(neuronId2);
								//boost::random::uniform_int_distribution<> dist(0, 100);
								//int value = dist(percent);
								//if (value <= percent) 
								//{
									if(!neu->isConnectedTo(neu2)) 
									{
										neu->connectTo(neu2,polarity);
									}
								//}
							}

						}

					}

				}
			}
		}
	}
}

// This kludge is to sprinkle synapses between nucleus's A and B
void BrainDemoTiny::insertSynapses(Nucleus* nucA, Nucleus* nucB) {
	// null for now
	(void)nucA;
	(void)nucB;
}


Brain* BrainDemoTiny::createFromJSON(void) 
{
	Brain* brain;

	std::stringstream ss;

	LOGSTREAM(ss) << " Loading brain from JSON" << std::endl;
	globalObject->log(ss);
	brain = Brain::loadFromJSON("../../../database/","BrainDemoTiny");
	return brain;
}


int main(int argc, char *argv[])
{
	SNNEngine *engine = new SNNEngine();
	engine->initialize("../../../database/","BrainDemoTiny");
	engine->startEngine();
}

