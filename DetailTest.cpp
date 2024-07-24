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
#include "TR1Random.h"
#include "DetailTest.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>


DetailTest::DetailTest(void)
{
}

DetailTest::~DetailTest(void)
{
}

Brain * DetailTest::create(bool rebuild) 
{

	Brain *brain; 

	std::stringstream ss;

	if(!rebuild) // if not rebuilding, just return brain
	{
		LOGSTREAM(ss) << " Loading brain" << std::endl;
		globalObject->log(ss);
		brain = Brain::load();
	} 
	else 
	{
		LOGSTREAM(ss) << " Creating brain" << std::endl;
		globalObject->log(ss);
		brain = Brain::create();
	}

	SpatialDetails sd(2500, 2500, 2500, 5000, 5000, 5000); // Dummy test locations/size


	// Create Thalamus
	LOGSTREAM(ss) << "Create Micro Thalamus ... " << std::endl;
	globalObject->log(ss);

	Region *regionThalamus = 0L;
	if(brain->restartpoint())
	{
		regionThalamus = Region::create("regionThalamus", sd);
		brain->add(regionThalamus);
	} 
	else 
	{
		globalObject->regionDB.next();
		regionThalamus = globalObject->regionDB.getValue();
		LOGSTREAM(ss) << "regionThalamus globalObject->regionDB.getValue() " << std::endl;
		globalObject->log(ss);
	}
	brain->syncpoint();


	// Create Thalamic Nuclei
	ColumnNeuronProfile profile; // default profile for all layers is Pyramidal neurons, 100 neurons per cluster, with 10 clusters

	Nucleus *nucleusAnteroventral = 0L;
	if(brain->restartpoint())
	{
		nucleusAnteroventral = Nucleus::create("nucleusAnteroventral", sd);
		nucleusAnteroventral->nucleusType = SENSORY_NUCLEUS;
		regionThalamus->add(nucleusAnteroventral);
//		nucleusAnteroventral->addColumns(10,profile); // 10 columns, each with 6 layers, each with 5 clusters, each with 10 neurons
		nucleusAnteroventral->addColumns(1,6,1,10); // 1 column, each with 6 layers, each with 1 clusters, each with 10 neurons
	} 
	else 
	{
		globalObject->nucleusDB.next();
		nucleusAnteroventral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionThalamus->name << " complete with " << regionThalamus->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	
	// Wrap up construction


	LOGSTREAM(ss) << " Attaching layers within columns " << std::endl;
	globalObject->log(ss);
	// Now, attach layers within a column
//	CollectionIterator<Column *> itColumn(Global::getColumnsCollection());

	size_t columnCount = globalObject->columnsSize();
	size_t columnNum = 0;

	for (globalObject->columnDB.begin(); globalObject->columnDB.more(); globalObject->columnDB.next())
	{
		Column *column = globalObject->columnDB.getValue();

		size_t pct = (columnNum*100) / columnCount;
//		std::cout << " Initializing layers within column " << column->id << std::endl;
		LOGSTREAM(ss) << " Initializing layers within column " << column->id << " - (" << columnNum++ << " of " << columnCount << " - " << pct << "%) " << std::endl;
		globalObject->log(ss);

		if(brain->restartpoint())
		{
		//	column->initializeLayers(0);
		}
		brain->syncpoint();
	}
	
	LOGSTREAM(ss) << "Connecting columns " << std::endl;
	globalObject->log(ss);
	for (globalObject->columnDB.begin(); globalObject->columnDB.more(); globalObject->columnDB.next())
	{
		Column *column1 = globalObject->columnDB.getValue();
		for (globalObject->columnDB.begin(1); globalObject->columnDB.more(1); globalObject->columnDB.next(1))
		{
			Column *column2 = globalObject->columnDB.getValue(1);
			if(column1 != column2) {
				column1->projectTo(column2);
				column2->projectTo(column1);
			}

		}
	}

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



	LOGSTREAM(ss) << "------------------------------------------------------" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->dendritesSize()) << " dendrites created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->synapsesSize()) << " synapses created" << std::endl;
	globalObject->log(ss);

	unsigned long zeroCount = 0;
//	CollectionIterator<Axon *> itAxon(Global::getAxonsCollection());
//	for (itAxon.begin(); itAxon.more(); itAxon.next())
	for (globalObject->axonDB.begin(); globalObject->axonDB.more(); globalObject->axonDB.next())
	{
		if(globalObject->axonDB.getValue()->getSynapses()->size()==0) 
			zeroCount++;
	}
	LOGSTREAM(ss) << formatNumber(zeroCount) << " axons have no synapses." << std::endl;
	globalObject->log(ss);

	// Make final Adustments
	finalAdjustments(ss);


	LOGSTREAM(ss) << "SNNEngine ready to accept input..." << std::endl;
	globalObject->log(ss);


	globalObject->flush();
	return brain;
}

void DetailTest::finalAdjustments(std::stringstream &ss)
{
	LOGSTREAM(ss) << "Making final adjustments..." << std::endl;
	globalObject->log(ss);

	unsigned long zeroCount = 0;
	for (globalObject->axonDB.begin(); globalObject->axonDB.more(); globalObject->axonDB.next())
	{
		Axon* ax = globalObject->axonDB.getValue();
		if (ax->getSynapses()->size() == 0)
		{
			zeroCount++;
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
								Dendrite* den = Dendrite::create(neuX, neu,-1.0);				// Create a new dendrite
                                neuX->dendriteMap.insert(std::make_pair(neu->id, den->id));
								Synapse* s = Synapse::create(den,-1.0);					// Add a synapse to the dendrite
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
	for (globalObject->axonDB.begin(); globalObject->axonDB.more(); globalObject->axonDB.next())
	{
		Axon* ax = globalObject->axonDB.getValue();
		if (ax->getSynapses()->size() == 0)
		{
			zeroCount++;
		}
	}

	LOGSTREAM(ss) << formatNumber(zeroCount) << " axons have no synapses after fixup." << std::endl;
	globalObject->log(ss);


	LOGSTREAM(ss) << "Final adjustments complete..." << std::endl;
	globalObject->log(ss);

}



void DetailTest::step(Brain *brain)
{
	(void)brain;
//	std::cout << "Current timestamp " << globalObject->current_timestep << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;


	std::stringstream ss;
/****
	if(globalObject->actionPotentialsSize()>0) {
		LOGSTREAM(ss) << "Current timestamp " << globalObject->current_timestep << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;
		globalObject->log(ss);
	}
****/

}

std::string DetailTest::formatNumber(unsigned long long number) {
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << number;
	return ss.str();

}

// This kludge is to sprinkle synapses among the clusters within nucleus, and within the neurons within clusters
void DetailTest::insertSynapses(Nucleus* nuc) {
	for (size_t i = 0; i < nuc->columns.size(); i++) {
		long colId = nuc->columns[i];
		Column* col = globalObject->columnDB.getComponent(colId);
		for (size_t j = 0; j < col->layers.size(); j++) {
			long layerId = col->layers[j];
			Layer *lay = globalObject->layerDB.getComponent(layerId);
			// If inputlayer, polarity is inhibitory, otherwise excitatory
			float polarity = EXCITATORY;
			if(j==(size_t)col->inputLayer)
				polarity = EXCITATORY;

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
						int percent = 95; // start with 98%
						//
						boost::random::mt19937 gen;

						for (size_t ll = 0; ll < clu->getNeurons().size(); ll++) {
							unsigned long neuronId2 = clu->getNeurons()[ll];
							if (neuronId2 != neu->id) { // If not ourself
								Neuron* neu2 = globalObject->neuronDB.getComponent(neuronId2);
								boost::random::uniform_int_distribution<> dist(0, 100);
								int value = dist(gen);
								if (value <= percent) 
								{
									if(!neu->isConnectedTo(neu2)) {
										neu->connectTo(neu2,polarity);
									}
								}
							}

						}

					}

				}
			}
		}
	}
}

// This kludge is to sprinkle synapses between nucleus's A and B
void DetailTest::insertSynapses(Nucleus* nucA, Nucleus* nucB) {
	(void)nucA;
	(void)nucB;
	// null for now
}


Brain* DetailTest::createFromJSON(void) 
{
	Brain* brain;

	std::stringstream ss;

	LOGSTREAM(ss) << " Loading brain from JSON" << std::endl;
	globalObject->log(ss);
	brain = Brain::loadFromJSON();
	return brain;
}

