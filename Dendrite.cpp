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

#include "Dendrite.h"
#include "Global.h"
#include "Neuron.h"

Dendrite::Dendrite(Neuron *neuron):
	Process(ComponentTypeDendrite)
{

	setPreSynapticNeuronId(neuron->id);
	setRate(DEFAULT_DENDRITE_RATE);
	setDistance(10.0);

	Synapse *s = Synapse::create(this);
	this->synapseId = s->id;
	s->setOwningDendriteId(this->id);
}

Dendrite::Dendrite(Neuron *neuron, long newId):
	Process(ComponentTypeDendrite)
{
	setPreSynapticNeuronId(neuron->id);
	setRate(DEFAULT_DENDRITE_RATE);
	setDistance(10.0);

	Synapse *s = Synapse::create(this);
	this->synapseId = s->id;
	s->setOwningDendriteId(newId);
	neuron->setDirty();
}

Dendrite::~Dendrite(void)
{
}

/*
	long neuronId;
	long synapseId
*/
void Dendrite::toJSON(std::ofstream& outstream)
{
	outstream << "                            { \"_type\": \"Dendrite\", \"id\": " << id << ", \"synapseId\": " << synapseId << " } " << std::endl;

}


void Dendrite::save(void)
{
	globalObject->dendriteDB.save(this);
}

void Dendrite::commit(void)
{
	globalObject->dendriteDB.addToCache(this);
}


Dendrite *Dendrite::create(Neuron *postSynapticNeuron, Neuron *preSynapticNeuron)
{

	long newId = globalObject->nextComponent(ComponentTypeDendrite);
	Dendrite *d = new Dendrite(preSynapticNeuron, newId);
	d->id = newId;
	d->setPostSynapticNeuronId(postSynapticNeuron->id);
	d->setPreSynapticNeuronId(preSynapticNeuron->id);
	globalObject->insert(d);
	postSynapticNeuron->getDendrites()->push_back(d->id);

/*/
	if(preSynapticNeuron->id == postSynapticNeuron->id)
	{
		std::cout << "I'm here" << std::endl;
	}
*/	
	return d;
}

void Dendrite::fire(void)
{

		float lclDistance = getDistance();
		float lclRate = getRate();

		Synapse *s = globalObject->synapseDB.getComponent(this->synapseId);
		long offset = lclDistance * lclRate;
		if(offset < MAX_TIMEINTERVAL_BUFFER_SIZE && offset > 0)
		{
			long ownerId = s->getOwningDendriteId();
			Axon *a = globalObject->axonDB.getComponent(ownerId);

			// std::cout << "Dendrite Offset " << offset << std::endl;
			TimedEvent *te = TimedEvent::create(globalObject->current_timestep + offset, a, s->id);
		}
		else 
		{
//			std::cout << "Dendrite Offset " << offset << " too large. ignoring." << std::endl;
		}

}

Dendrite::Dendrite(void): 
	Process(ComponentTypeDendrite)
{
	setRate(DEFAULT_DENDRITE_RATE);
	setDistance(10.0);
}

Tuple *Dendrite::getImage(void)
{

	float lclDistance = getDistance();
	float lclRate = getRate();

	size_t size = sizeof(parentId) + sizeof(lclDistance) + sizeof(lclRate) + sizeof(postSynapticNeuronId) + sizeof(preSynapticNeuronId) + sizeof(synapseId);

	char *image = globalObject->allocClearedMemory(size);
	char *ptr = (char*)image;



	memcpy(ptr, &parentId, sizeof(parentId)); 		ptr += sizeof(parentId);
	memcpy(ptr,&lclDistance,sizeof(lclDistance)); 	ptr+=sizeof(lclDistance);
	memcpy(ptr,&lclRate,sizeof(lclRate)); 			ptr+=sizeof(lclRate);
	memcpy(ptr,&postSynapticNeuronId,sizeof(postSynapticNeuronId)); 		ptr+=sizeof(postSynapticNeuronId);
	memcpy(ptr,&preSynapticNeuronId,sizeof(preSynapticNeuronId)); 		ptr+=sizeof(preSynapticNeuronId);
	memcpy(ptr,&synapseId,sizeof(synapseId)); 		ptr+=sizeof(synapseId);


	Tuple* tuple = new Tuple();
	tuple->objectPtr = image;
	tuple->value = size;

	return tuple;
}

Dendrite *Dendrite::instantiate(long key, size_t len, void *data)
{

	Dendrite *dendrite = new Dendrite();
	dendrite->id = key;

	char *ptr = (char*)data;

	float lclDistance = 0;
	float lclRate = 0;

	memcpy(&dendrite->parentId, ptr, sizeof(dendrite->parentId)); 	ptr += sizeof(dendrite->parentId); 
	memcpy(&lclDistance, ptr, sizeof(lclDistance)); 				ptr += sizeof(lclDistance); dendrite->setDistance(lclDistance);
	memcpy(&lclRate, ptr, sizeof(lclRate)); 						ptr += sizeof(lclRate); dendrite->setRate(lclRate);
	memcpy(&dendrite->postSynapticNeuronId,ptr,sizeof(dendrite->postSynapticNeuronId)); 	ptr+=sizeof(dendrite->postSynapticNeuronId); 	
	memcpy(&dendrite->preSynapticNeuronId,ptr,sizeof(dendrite->preSynapticNeuronId)); 	ptr+=sizeof(dendrite->preSynapticNeuronId); 	
	memcpy(&dendrite->synapseId,ptr,sizeof(dendrite->synapseId)); 	ptr+=sizeof(dendrite->synapseId); 	

	return dendrite;
}
