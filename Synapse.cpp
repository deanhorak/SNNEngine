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
#include "Synapse.h"

// Settable externs
extern long FIRING_WINDOW;
extern long PROPAGATION_DELAY_MICROSECONDS;
extern float DECAY_FACTOR;
extern long REFACTORY_PERIOD;
extern float WEIGHT_GRADATION;
extern float RATE_GRADATION;

Synapse::Synapse(Dendrite *dendrite) : NNComponent(ComponentTypeSynapse)
{
	//size_t rnd = (size_t)tr1random->generate(1, 1000); // Random # between one and 1000
//	if (rnd <= 100)									   // if random number == 5 (.1%) the set weight to 1
//	{
		//		std::cout << "Synapse " << id << " randomly primed with a weight of 1"  << std::endl;
//		weight = 0.25;
//	}
//	else
//	{
	weight = 0.01;
	weightUpdateCounter=0;
//	}

	position = 100.0; // (float)tr1random->generate(1, 1000); // Random # between one and 1000;
	setOwningDendriteId(dendrite->id);
	dendrite->setDirty();
}

Synapse::Synapse(void) : NNComponent(ComponentTypeSynapse)
{
	weight = 0.01;
	weightUpdateCounter=0;
	position = 100.0;
	setOwningDendriteId(0);
}

Synapse::~Synapse(void)
{
}

/*
	unsigned long owningprocessId;
	float weight;
	float position;
*/
void Synapse::toJSON(std::ofstream &outstream)
{
	outstream << "                                { \"_type\": \"Synapse\", \"id\": " << id << ", \"owningprocessId\": " << owningDendriteId << ", \"weight\": " << weight << ", \"position\": " << position << " } " << std::endl;
}

void Synapse::save(void)
{
	globalObject->synapseDB.save(this);
}

void Synapse::commit(void)
{
	globalObject->synapseDB.addToCache(this);
}

Synapse *Synapse::create(Dendrite *dendrite, float polar)
{
	Synapse *s = new Synapse(dendrite);
	s->id = globalObject->nextComponent(ComponentTypeSynapse);
	s->position = dendrite->getDistance();
	s->postSynapticNeuronId = dendrite->getPreSynapticNeuronId();
	s->polarity = polar;
	globalObject->insert(s);
	return s;
}

float Synapse::sumweights(Neuron *neuron)
{
	float sum = 0;
	std::vector<long> *dendrites = neuron->getDendrites();
	long numDendrites = (*dendrites).size();
	for(size_t i=0;i < (size_t)numDendrites;i++)
	{
		long dId = (*dendrites)[i];
		Dendrite *d = globalObject->dendriteDB.getComponent(dId);
		long nId = d->getPreSynapticNeuronId();
		Neuron *n = globalObject->neuronDB.getComponent(nId);
		float diff = (float) (globalObject->current_timestep - n->lastfired);
		if(diff < FIRING_WINDOW) // if the presynaptic neuron has fired within the window, use it's weight
		{
			long sId = d->getSynapseId();
			Synapse *s = globalObject->synapseDB.getComponent(sId);
			float attenuation = (FIRING_WINDOW - diff) / (FIRING_WINDOW);
			float attenuatedWeight = s->getWeight() * attenuation;	// attenuate the weight based on how long ago it fired.
			sum += attenuatedWeight;
		}
	}
	return sum;
}

void Synapse::receiveAP(ActionPotential *ap)
{
	Dendrite *dendrite = getOwningDendrite();
	long neuronId = dendrite->getPostSynapticNeuronId();
	Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
	bool pass = false;
	/////////////////////////////////////////////////////////////////////
	// DSH
	// KEY COMPONENT OF THE LOGIC In need of further development
	// HOw to make the decision on whether or not to pass the spike onto the dendrite
	// For now, we compare the syapse's weight to a simple threshold value (50%)
	//	IF it's weight is greater than or equal to a fixed value (0.5),
	//		THEN pass the AP on by firing the dendrite
	//		ELSE ignore the AP
	//
	////////////////////////////////////////////////////////////////////

	
	// if(neuron->potential >= neuron->threshold) // threshold?

	float total_weight = sumweights(neuron);

	neuron->potential = total_weight; // += this->weight;
	if (neuron->potential >= neuron->threshold)
		pass = true;

	if (pass)
	{
		if(globalObject->logEvents) 
		{	
			std::stringstream ss;
			ss << "synapse_receiveAP_firing: synapse=" << this->id << ", weight=" << this->weight << ", ap=" << ap->id << ", dendrite=" << dendrite->id << ", neuron=" << neuronId << ", neuron_potential=" << neuron->potential << ", presynapticneuron=" << dendrite->getPreSynapticNeuronId();
			globalObject->writeEventLog(ss.str().c_str());
		}

		//std::cout << "Synapse " << id << " (on Dendrite " << dendrite->id << ") triggered by AP " << ap->id << " and current weight is " << weight << " so firing dendrite."  << std::endl;
		//std::cout << "Synapse::receiveAP() Neuron " << neuron->id << " triggered by AP " << ap->id << " and potential is " << neuron->potential << " so firing neuron."  << std::endl;
		neuron->fire();
	}
	else
	{
		if(globalObject->logEvents) 
		{	
			std::stringstream ss;
			ss << "synapse_receiveAP_notfiring: synapse=" << this->id << ", weight=" << this->weight << ", ap=" << ap->id << ", dendrite=" << dendrite->id << ", neuron=" << neuronId << ", neuron_potential=" << neuron->potential << ", presynapticneuron=" << dendrite->getPreSynapticNeuronId();
			globalObject->writeEventLog(ss.str().c_str());
		}
		// std::cout << "Synapse::receiveAP() Neuron " << neuron->id << " NOT triggered by AP " << ap->id << " and potential is " << neuron->potential << " so not firing neuron."  << std::endl;
	}
}

Tuple *Synapse::getImage(void)
{
	/* -- persisted values
		Dendrite *owningprocess;
		float weight;
		float position;
	*/

	size_t size = sizeof(owningDendriteId) + sizeof(postSynapticNeuronId) + sizeof(weight) + sizeof(polarity) + sizeof(weightUpdateCounter) + sizeof(position);

	char *image = globalObject->allocClearedMemory(size);
	char *ptr = (char *)image;

	memcpy(ptr, &owningDendriteId, sizeof(owningDendriteId));
	ptr += sizeof(owningDendriteId);

	memcpy(ptr, &postSynapticNeuronId, sizeof(postSynapticNeuronId));
	ptr += sizeof(postSynapticNeuronId);

	memcpy(ptr, &weight, sizeof(weight));
	ptr += sizeof(weight);

	memcpy(ptr, &polarity, sizeof(polarity));
	ptr += sizeof(polarity);

	memcpy(ptr, &weightUpdateCounter, sizeof(weightUpdateCounter));
	ptr += sizeof(weightUpdateCounter);

	memcpy(ptr, &position, sizeof(position));
	ptr += sizeof(position);

	Tuple *tuple = new Tuple();
	tuple->objectPtr = image;
	tuple->value = size;

	return tuple;
}

Synapse *Synapse::instantiate(long key, size_t len, void *data)
{
	(void)len;
	// 	u_int32_t size = sizeof(long)+sizeof(float)+sizeof(float);

	Synapse *synapse = new Synapse();
	synapse->id = key;
	char *ptr = (char *)data;

	memcpy(&synapse->owningDendriteId, ptr, sizeof(synapse->owningDendriteId));
	ptr += sizeof(synapse->owningDendriteId); 

	memcpy(&synapse->postSynapticNeuronId, ptr, sizeof(synapse->postSynapticNeuronId));
	ptr += sizeof(synapse->postSynapticNeuronId); 

	memcpy(&synapse->weight, ptr, sizeof(synapse->weight));
	ptr += sizeof(synapse->weight);

	memcpy(&synapse->polarity, ptr, sizeof(synapse->polarity));
	ptr += sizeof(synapse->polarity);

	memcpy(&synapse->weightUpdateCounter, ptr, sizeof(synapse->weightUpdateCounter));
	ptr += sizeof(synapse->weightUpdateCounter);

	memcpy(&synapse->position, ptr, sizeof(synapse->position));
	ptr += sizeof(synapse->position);

	return synapse;
}

Dendrite *Synapse::getOwningDendrite(void)
{
	return globalObject->dendriteDB.getComponent(owningDendriteId);
};
void Synapse::setWeight(float value)
{
	
	float oldweight = weight;
	if (value != weight)
	{
		boost::mutex::scoped_lock  amx(synapse_mutex);
		weightUpdateCounter++;

		//		std::cout << "Synapse::setWeight Synapse: " << id << " adjusting weight from " << weight << " to " << value << std::endl;

		float diff = weight - value;
		//dampen the actual value so that the weight changes more slowly over time 
		// calculate weightFactor, which should initially be a value of 1, followed by 0.5, followed by 0.333 etc
		float weightFactor = (WEIGHT_GRADATION / weightUpdateCounter) / WEIGHT_GRADATION;
		// take the difference in value and weight it based on weightFactor
		float delta = diff * weightFactor;

		float adjustedWeight = weight + delta; // calc the adjusted weight 

		weight = adjustedWeight;
		setDirty(true);

		if(globalObject->logEvents) 
		{
			std::stringstream ss;
			if(this->owningDendriteId >= globalObject->componentBase[ComponentTypeAxon] && this->owningDendriteId < globalObject->componentBase[ComponentTypeDendrite])
			{
				ss << "synapse_weight_change: synapse=" << this->id << ",axon=" << this->owningDendriteId << ", old=" << oldweight << ", new=" << weight;
			}
			else if(this->owningDendriteId >= globalObject->componentBase[ComponentTypeDendrite] && this->owningDendriteId < globalObject->componentBase[ComponentTypeSynapse])
			{
				Dendrite *d = getOwningDendrite();
				ss << "synapse_weight_change: synapse=" << this->id  << ",dendrite=" << this->owningDendriteId << ", old=" << oldweight << ", new=" << weight << ", presynapticneuron=" << d->getPreSynapticNeuronId() << ", postsynapticneuron=" << d->getPostSynapticNeuronId();
			}
			globalObject->writeEventLog(ss.str().c_str());
		}

	}
}