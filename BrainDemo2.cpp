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
#include "BrainDemo2.h"

BrainDemo2::BrainDemo2(void)
{
}

BrainDemo2::~BrainDemo2(void)
{
}

Brain * BrainDemo2::create(bool rebuild) 
{

	Brain *brain; 

	if(!rebuild) // if not rebuilding, just return brain
	{
		brain = Brain::load();
		// configure counters
		return brain;
	}

	brain = Brain::create();

	size_t regionCount = 2;															// # of regions
	std::stringstream ss;

//	LOGSTREAM(ss) << " Creating brain" << std::endl;
//	globalObject->log(ss);

	
	LOGSTREAM(ss) << " Creating brain" << std::endl;
	globalObject->log(ss);

	std::string dummyStr;
	SpatialDetails sd(1, 2, 3, 4, 5, 6); // Dummy test locations/size

	for(size_t i1=0;i1<regionCount;i1++) 
	{
		Region *r = Region::create(dummyStr,sd);
		size_t nucleiCount = 2;														// # of Nuclei
		for(size_t i2=0;i2<nucleiCount;i2++) 
		{
			SpatialDetails sd(1, 2, 3, 4, 5, 6); // Dummy test locations/size

			Nucleus *n = Nucleus::create("dummy",sd,r->id);
			size_t columnCount = 2;													// # of Columns
			for(size_t i3=0;i3<columnCount;i3++) 
			{
				Column *c = Column::create(sd,n->id);
				size_t lSize = c->layers.size();
				for(size_t k1=0;k1<lSize;k1++)
				{
					Layer *layer = globalObject->layerDB.getComponent(c->layers[k1]);
					size_t clusterCount = 5;											// # of Clusters
					for(size_t i5=0;i5<clusterCount;i5++) 
					{
						SpatialDetails rsd(sd);
						rsd.randomizeLocation();
						Cluster *cl = Cluster::create(rsd,n->id);
						size_t neuronCount = 10;										// # of Neurons
						for(size_t i6=0;i6<neuronCount;i6++) 
						{
							Neuron *neur = Neuron::create(sd, Pyramidal,cl->id,n->nucleusType);
							cl->getNeurons().push_back(neur->id);
							cl->setDirty();
						}
						layer->clusters.push_back(cl->id);
						layer->setDirty();
					}
				}
				n->columns.push_back(c->id);
				n->setDirty();
			}
			r->nuclei.push_back(n->id);
			r->setDirty();
		}
		brain->regions.push_back(r->id);
		brain->setDirty();
	}

	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << globalObject->regionsSize() << " regions created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->nucleiSize() << " nuclei created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->columnsSize() << " columns created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->layersSize() << " layers created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->clustersSize() << " clusters created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->neuronsSize() << " neurons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->axonsSize() << " axons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << " Attaching layers within columns " << std::endl;
	globalObject->log(ss);
	// Now, attach layers within a column
//	CollectionIterator<Column *> itColumn(Global::getColumnsCollection());

	for (globalObject->columnDB.begin(); globalObject->columnDB.more(); globalObject->columnDB.next())
	{
		Column *column = globalObject->columnDB.getValue();

//		std::cout << " Initializing layers within column " << column->id << std::endl;
		LOGSTREAM(ss) << " Initializing layers within column " << column->id << std::endl;
		globalObject->log(ss);

		//column->initializeLayers(0);
	}

	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << globalObject->regionsSize() << " regions created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->nucleiSize() << " nuclei created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->columnsSize() << " columns created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->layersSize() << " layers created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->clustersSize() << " clusters created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->neuronsSize() << " neurons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->axonsSize() << " axons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << " Attach regions" << std::endl;
	globalObject->log(ss);
// Finally, attach the Regions
	for (globalObject->regionDB.begin(); globalObject->regionDB.more(); globalObject->regionDB.next())
	{
		Region *r1 = globalObject->regionDB.getValue();
		for (globalObject->regionDB.begin(1); globalObject->regionDB.more(1); globalObject->regionDB.next(1))
		{
			long key1 = globalObject->regionDB.key();
			long key2 = globalObject->regionDB.key(1);

			if(key1 != key2)
			{
				Region *r2 = globalObject->regionDB.getValue(1);
//				std::cout << "Connecting region " << r1->id << " to " << r2->id << "..." << std::endl;

				std::stringstream ss;
				ss  << "Connecting region " << r1->id << " to " << r2->id << "..." << std::endl;
//				sprintf_s(buffer,"Connecting Region %d to Region %d.",r1->id, r2->id);
//				std::string msg(buffer);
				globalObject->log(ss);

				r1->projectTo(r2);
			}
		}
	}

	LOGSTREAM(ss) << "------------------------------------------------------" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->dendritesSize() << " dendrites created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->synapsesSize() << " synapses created" << std::endl;
	globalObject->log(ss);

	unsigned long zeroCount = 0;
//	CollectionIterator<Axon *> itAxon(Global::getAxonsCollection());
//	for (itAxon.begin(); itAxon.more(); itAxon.next())
	for (globalObject->axonDB.begin(); globalObject->axonDB.more(); globalObject->axonDB.next())
	{
		if(globalObject->axonDB.getValue()->getSynapses()->size()==0) 
			zeroCount++;
	}
	LOGSTREAM(ss) << zeroCount << " axons have no synapses." << std::endl;
	globalObject->log(ss);

	globalObject->flush();
	return brain;
}

void BrainDemo2::step(Brain *brain)
{
//	std::cout << "Current timestamp " << globalObject->current_timestep << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;


	std::stringstream ss;
//	LOGSTREAM(ss) << "Current timestamp " << globalObject->current_timestep << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;
//	globalObject->log(ss);

	if(globalObject->current_timestep == 10) 
	{
		// At timestep 10, initialize all of the synapses attached to the first axon with a weight of 1.0 
//		CollectionIterator<Neuron *> itNeuron(Global::getNeuronsCollection());
//		for (itNeuron.begin(); itNeuron.more(); itNeuron.next())
		for (globalObject->neuronDB.begin(); globalObject->neuronDB.more(); globalObject->neuronDB.next())
		{
			std::vector<long> *it2 = globalObject->neuronDB.getValue()->getAxons();
			size_t ssize = it2->size();
			for(size_t i=0;i<ssize;i++)
			{
				Axon *axon = globalObject->axonDB.getComponent((*it2)[i]);
				std::vector<long> *it3 = axon->getSynapses();
				size_t ssize = it3->size();
				for(size_t i=0;i<ssize;i++)
				{
//				for (it3.begin(); it3.more(); it3.next())
//				{
					size_t rndA = (size_t) tr1random->generate(1,10); // Random # of Axons
					if(rndA==5) {
						Synapse *s = globalObject->synapseDB.getComponent((*it3)[i]);
						s->setWeight(1.0);
					}
				}
			}
		}
		globalObject->neuronDB.begin(); // Reset iterator to beginning

		// Then fire the first neuron
		Neuron *n = globalObject->neuronDB.getValue();
		n->fire(); // First 1st neuron
	
		for(size_t i=0;i<20;i++) { globalObject->neuronDB.next(); }
		for(size_t i=0;i<20;i++) // 21st-41st neuron
		{
			Neuron *n2 = globalObject->neuronDB.getValue();
			n2->fire(); // First 1st neuron
//			globalObject->neuronDB.value()->fire(); 
			globalObject->neuronDB.next();
		}


		for(size_t i=0;i<20;i++) { globalObject->neuronDB.next(); }
		for(size_t i=0;i<20;i++) // First 61st-81st neuron
		{
			Neuron *n2 = globalObject->neuronDB.getValue();
			n2->fire(); // First 1st neuron
			globalObject->neuronDB.next();
		}
		for(size_t i=0;i<20;i++) { globalObject->neuronDB.next(); }
		for(size_t i=0;i<20;i++) // First 101st-121st neuron
		{
			Neuron *n2 = globalObject->neuronDB.getValue();
			n2->fire(); // First 1st neuron
			globalObject->neuronDB.next();
		}

	}

}
