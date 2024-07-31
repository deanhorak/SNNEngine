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

#include "Axon.h"
#include "TR1Random.h"
#include "Global.h"
#include "Neuron.h"

extern long REFACTORY_PERIOD;

Axon::Axon(Neuron *neuron) : Process(ComponentTypeAxon)
{
	if (neuron->id == 0)
	{
		printf("%ld", neuron->id);
	}

	this->neuronId = neuron->id;
	setRate(DEFAULT_AXON_RATE);
	float dist = 100.0; // (float)tr1random->generate(1, 1000); // Random # between one and 1000;

	setDistance(dist);
	this->parentId = this->neuronId;
	neuron->setDirty();
}

Axon::~Axon(void)
{
}

/*
	long neuronId;

	std::vector<long> synapses;
*/
void Axon::toJSON(std::ofstream &outstream)
{
	std::string sep("");
	outstream << "                            { \"_type\": \"Axon\", \"id\": " << id << ", \"neuronId\": " << neuronId << ", \"synapses\": [" << std::endl;
	for (unsigned int i = 0; i < synapses.size(); i++)
	{
		outstream << sep;
		sep = ",";
		Synapse *s = globalObject->synapseDB.getComponent(synapses[i]);
		s->toJSON(outstream);
	}
	outstream << "                            ] } " << std::endl;
}

void Axon::save(void)
{
	globalObject->axonDB.save(this);
}

void Axon::commit(void)
{
	globalObject->axonDB.addToCache(this);
}

Axon *Axon::create(Neuron *neuron)
{

	if (neuron->id == 0)
	{
		printf("%ld", neuron->id);
	}

	Axon *a = new Axon(neuron);
	a->id = globalObject->nextComponent(ComponentTypeAxon);
	globalObject->insert(a);
	return a;
}

void Axon::initializeRandom(void)
{
	/*
		size_t rnd = (size_t) tr1random->generate(1,10); // Random # of Synapses
		for(size_t i=0;i<rnd;i++)
		{
			Synapse *s = new Synapse();
			//s->initializeRandom();
			synapses.push_back(s);
		}
	*/
}
/*
void Axon::removeDeadAPs(void)
{
	
		bool done = false;
		while(!done)
		{
			done = true;
			CollectionIterator<ActionPotential *> it(&actionpotentials);
			for (it.begin(); it.more(); it.next())
			{
				ActionPotential *ap = it.value();
				if(ap->finished)
				{
					actionpotentials.erase(ap->id);
					done = false;
					break;
				}
			}
		}
	
}
*/


/*
void Axon::cycle(void)
{
	CollectionIterator<ActionPotential *> it(&actionpotentials);
	for (it.begin(); it.more(); it.next())
	{
		ActionPotential *ap = it.value();
		ap->cycle();
	}
	removeDeadAPs();
}
*/

void Axon::insertSynapse(long synapseId)
{
	bool found = false;
	size_t synapseCount = synapses.size();
	for(size_t i=0;i<synapseCount;i++)
	{
		if(synapseId == synapses[i])	
			found = true;

	}
	if(!found)
	{
		synapses.push_back(synapseId);
		setDirty();
	}
};

long Axon::fire(void)
{
	if(globalObject->logEvents) 
	{		
		std::stringstream ss;
		ss << "axon_fire: neuron=" << this->neuronId;
		globalObject->writeEventLog(ss.str().c_str());
	}

	long lowestOffset = MAX_TIMEINTERVAL_OFFSET;
	size_t synapseSize = this->synapses.size();
	for(size_t synapseIndex = 0;synapseIndex < synapseSize; synapseIndex++) 
	{
		long synapseId = this->synapses[synapseIndex];
		Synapse *thisSynapse = globalObject->synapseDB.getComponent(synapseId);

		long dendriteId = thisSynapse->getOwningDendriteId();
		Dendrite *thisDendrite = globalObject->dendriteDB.getComponent(dendriteId);

		float lclRate = thisDendrite->getRate();
//		long offset = (long)(s->getPosition() * lclRate) + REFACTORY_PERIOD; // give ourselves 100ms buffer
// lets not adjust the axon rate - keep it steady. Only adjust dendrite rates in applySTDP method
		float pos = thisSynapse->getPosition();
		long offset = ActionPotential::computeOffset(pos, lclRate) + REFACTORY_PERIOD; // give ourselves 10ms buffer
		
		if(offset > 0)
		{
			if(lowestOffset > offset) 
				lowestOffset = offset;
			// std::cout << "Axon Offset " << offset << std::endl;
			long timeslice = globalObject->current_timestep + offset;
			
			TimedEvent::create(timeslice, thisDendrite, thisSynapse->id);

			if(globalObject->logEvents) 
			{		
				std::stringstream ss;
				ss << "timed_event: synapse=" << thisSynapse->id << ", dendrite=" << thisDendrite->id << ", neuron=" << thisDendrite->getPostSynapticNeuronId() << ", presynaptic_neuron=" << thisDendrite->getPreSynapticNeuronId() << ", offset=" << offset << ", timeslice=" << timeslice;
				globalObject->writeEventLog(ss.str().c_str());
			}

		} 
		else 
		{
//			std::cout << "Axon Offset " << offset << " too large. ignoring." << std::endl;
		}
	}


	//	actionpotentials.insert(std::pair<long,ActionPotential *>(ap->id,ap));

	//	std::cout << "Axon " << id << " firing from Neuron " << this->neuron->id << std::endl;
	return lowestOffset;
}

Axon::Axon(void) : Process(ComponentTypeAxon)
{
}

Tuple *Axon::getImage(void)
{
	/* -- persisted values
		float distance;
		float rate;
		Neuron *neuron;
		std::map<long,Synapse *> synapses;
	*/
	long synapseCount = synapses.size();
	float lclDistance = getDistance();
	float lclRate = getRate();

//	int sizeOflong = sizeof(long);
//	int sizeOffloat = sizeof(float);


	size_t size = sizeof(parentId) + sizeof(lclDistance) + sizeof(lclRate) + sizeof(neuronId) +
				  sizeof(synapseCount) + (synapseCount * sizeof(long));

	char *image = globalObject->allocClearedMemory(size);
	char *ptr = (char *)image;

	memcpy(ptr, &parentId, sizeof(parentId));
	ptr += sizeof(parentId);
	memcpy(ptr, &lclDistance, sizeof(lclDistance));
	ptr += sizeof(lclDistance);
	memcpy(ptr, &lclRate, sizeof(lclRate));
	ptr += sizeof(lclRate);
	memcpy(ptr, &neuronId, sizeof(neuronId));
	ptr += sizeof(neuronId);
	memcpy(ptr, &synapseCount, sizeof(synapseCount));
	ptr += sizeof(synapseCount);

	//	if(id == 800000256) {
	// sanity check on # of synapses.
	if (synapseCount > 1000000 || synapseCount < 0)
	{ // max reasonable synapses is 1 million
		printf("getImage: Axon %ld has %ld synapse\n", this->id, synapseCount);
	}
	else
	{
		for (size_t i = 0; i < (size_t)synapseCount; i++)
		{
			long k = synapses[i];
			memcpy(ptr, &k, sizeof(k));
			ptr += sizeof(k);
		}
	}

	Tuple *tuple = new Tuple();
	tuple->objectPtr = image;
	tuple->value = size;
	return tuple;
}

Axon *Axon::instantiate(long key, size_t len, void *data)
{

	(void)len; 
	long lclsynapseCount = 0;
	long lclneuronId = 0;
	long lclpId = 0;
	float lclDistance = 0;
	float lclRate = 0;

//	int sizeOflong = sizeof(long);
//	int sizeOffloat = sizeof(float);


	Axon *axon = new Axon();
	axon->id = key;

	char *ptr = (char *)data;
	memcpy(&lclpId, ptr, sizeof(lclpId));
	ptr += sizeof(lclpId);
	memcpy(&lclDistance, ptr, sizeof(lclDistance));
	ptr += sizeof(lclDistance);
	memcpy(&lclRate, ptr, sizeof(lclRate));
	ptr += sizeof(lclRate);
	memcpy(&lclneuronId, ptr, sizeof(lclneuronId));
	ptr += sizeof(lclneuronId);
	memcpy(&lclsynapseCount, ptr, sizeof(lclsynapseCount));
	ptr += sizeof(lclsynapseCount);

	axon->parentId = lclpId;
	axon->setDistance(lclDistance);
	axon->setRate(lclRate);
	axon->neuronId = lclneuronId;


	if (lclsynapseCount > 1000000 || lclsynapseCount < 0) {
		printf("getImage: Axon %ld has %ld synapse\n", axon->id, lclsynapseCount);
	}
	else
	{
		for (size_t i = 0; i < (size_t)lclsynapseCount; i++)
		{
			long thisKey;
			memcpy(&thisKey, ptr, sizeof(long));
			ptr += sizeof(long);
			axon->synapses.push_back(thisKey);
		}
	}

	axon->setDirty(false);
	return axon;
}
