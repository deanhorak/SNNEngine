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
 * without express written permission from Your Name.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * If you have any questions about this license, please contact Your Name at dean.horak@gmail.com.
 */

#include "Neuron.h"
#include "TR1Random.h"
#include "Global.h"
#include "Axon.h"
#include "Dendrite.h"
#include "Synapse.h"
#include "SpatialDetails.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

// Settable externs
extern long FIRING_WINDOW;
extern long PROPAGATION_DELAY_MICROSECONDS;
extern float DECAY_FACTOR;
extern long REFACTORY_PERIOD;
extern float WEIGHT_GRADATION;
extern float RATE_GRADATION;

Neuron::Neuron(unsigned long parentId, int nucleusType) : NNComponent(ComponentTypeNeuron)
{
	this->nucleusType = nucleusType;
	this->parentId = parentId;
	this->threshold = 0.0;
	this->potential = RESTING_POTENTIAL;
	this->firing = false;
	this->latch = false;
	this->lastfired = 0; // globalObject->current_timestep;
	this->latch = false;
	SpatialDetails sd(2500, 2500, 2500, 5000, 5000, 5000); // Dummy test locations/size
	sd.randomizeLocation();
	this->location = sd.location;
	// Neurons always have (at least one axon)
	if (parentId == 0)
	{
		std::cout << "Neuron " << id << " has an invalid parent cluster [" << parentId << "] " << std::endl;
	}
}

Neuron::~Neuron(void)
{
}

void Neuron::toJSON(std::ofstream &outstream)
{
	std::string sep("");
	outstream << "                        { \"_type\": \"Neuron\", \"id\": " << id << ", \"neuronType\": " << neuronType << ", \"nucleusType\": " << nucleusType << ", \"threshold\": " << threshold << ", \"potential\": " << potential << ", \"location\": [" << location.x << ", " << location.y << ", " << location.z << "], \"axons\": [ " << std::endl;
	for (unsigned int i = 0; i < axons.size(); i++)
	{
		outstream << sep;
		sep = ",";
		Axon *a = globalObject->axonDB.getComponent(axons[i]);
		a->toJSON(outstream);
	}
	outstream << "                        ], \"dendrites\": [ " << std::endl;
	sep = "";
	for (unsigned int i = 0; i < dendrites.size(); i++)
	{
		outstream << sep;
		sep = ",";
		Dendrite *d = globalObject->dendriteDB.getComponent(dendrites[i]);
		d->toJSON(outstream);
	}
	outstream << "                        ] } " << std::endl;
}

void Neuron::save(void)
{
	globalObject->neuronDB.save(this);
}

void Neuron::commit(void)
{
	globalObject->neuronDB.addToCache(this);
}

Neuron *Neuron::create(SpatialDetails details, NeuronType nType, unsigned long parentId, int nucleusType)
{
	Neuron *n = new Neuron(parentId, nucleusType);
	n->id = globalObject->nextComponent(ComponentTypeNeuron);
	n->neuronType = nType;
	n->location = details.location;
	Axon *a = Axon::create(n);
	n->axons.push_back(a->id);

	globalObject->insert(n);

	//	if(a->id > globalObject->componentBase[ComponentTypeAxon] + 2384L)
	//	{
	//		std::cout << "Neuron.create: extra axon created " << a->id << std::endl;
	//	}

	return n;
}

long Neuron::getNucleusId(void)
{
	Cluster *cluster = globalObject->clusterDB.getComponent(parentId);
	if (cluster == NULL)
	{
		std::cout << "Neuron " << id << " has an invalid parent cluster [" << parentId << "] " << std::endl;
		return 0;
	}

	return cluster->parentId;
}

void Neuron::initializeRandom(void)
{
}

// Connect two neurons
// The connection is always via dendrite, meaning it's always to receive input
// Therefore orientation between the neurons must be correct
// The presynaptic neuron will use it's axon to connect, via a synapse, with the dendrite of the postsynaptic neuron
//
// In the connectTo method, "this" is the postSynaptic neuron and the parameter is the preSynapticNeuron
//
// Note that sice the synapse always belongs to a dendrite, and the owner of the dendrite is always the postSynapticNeuron
//

void Neuron::connect(Neuron *preSynapticNeuron, Neuron *postSynapticNeuron)
{
	if(preSynapticNeuron->isConnectedTo(postSynapticNeuron))
		return;

	Dendrite *dendrite = Dendrite::create(postSynapticNeuron, preSynapticNeuron); // Create a dendrite and synapse and add to postSynapticNeuron collection

	preSynapticNeuron->dendriteMap.insert(std::make_pair(postSynapticNeuron->id, dendrite->id));

	size_t axonSize = preSynapticNeuron->axons.size();
	for (size_t axonsIndex = 0; axonsIndex < axonSize; axonsIndex++)
	{
		Axon *thisAxon = globalObject->axonDB.getComponent(preSynapticNeuron->axons[axonsIndex]);
		long synapseId = dendrite->getSynapseId();
		thisAxon->insertSynapse(synapseId);

		//float pct = (float)tr1random->generate(1, 100); // Random 1 to 100% of the axon distance
		//pct = pct / 100;
		float position = thisAxon->getDistance(); // * pct;

		Synapse *s = globalObject->synapseDB.getComponent(synapseId);
		s->setPosition(position);
		//		std::cout << "Neuron.connectTo: Axon " << thisAxon->id << ", synapase " << s->id << " added." << std::endl;
	}
}

// Connect TO a target neuron.
// The presynaptic neuron will use it's axon to connect, via a synapse, with the dendrite of the postsynaptic neuron
void Neuron::connectTo(Neuron *preSynapticNeuron)
{
	Neuron *postSynapticNeuron = this; // Just to keep our head clear - the compiler will optimize this out
	connect(preSynapticNeuron, postSynapticNeuron);
}

// Connect FROM a source neuron.
// The connection is always via dendrite, meaning it's always to receive input
void Neuron::connectFrom(Neuron *postSynapticNeuron)
{
	Neuron *preSynapticNeuron = this;
	connect(preSynapticNeuron, postSynapticNeuron);
}

// Add synapses to the axons of this neuron.
// each synapse is associated with a dendrite of the target neuron, and also contains this neuron as it's associated neuron
// TODO: set the position of the synapse. Currently somewhat random
void Neuron::projectTo(Neuron *targetNeuron)
{
	connectTo(targetNeuron);
}

bool Neuron::isFromSensoryNucleus()
{
	if (this->nucleusType == SENSORY_NUCLEUS)
		return true;
	return false;
}

void Neuron::setFiring(bool value)
{
	std::stringstream ss;
	if (firing != value)
	{

		if (globalObject->setLogFiring)
		{
			globalObject->logFiring(this, value);
		}

		if (!isFromSensoryNucleus())
		{
			if (value)
			{
				unsigned long nucId = getNucleusId();
				if (nucId != 0)
				{
					Nucleus *nuc = globalObject->nucleusDB.getComponent(nucId);
				}
			}
			else
			{
				unsigned long nucId = getNucleusId();
				if (nucId != 0)
				{
					Nucleus *nuc = globalObject->nucleusDB.getComponent(nucId);
					LOGSTREAM(ss) << "Neuron " << id << " in nucleus " << nuc->name << " set non-firing at timestamp " << globalObject->current_timestep << "." << std::endl;
				}
				else
				{
					LOGSTREAM(ss) << "Neuron " << id << " (unknown nucleus) set non-firing at timestamp " << globalObject->current_timestep << "." << std::endl;
				}
			}
		}

		firing = value;
		setDirty(true);
		if (value) // we are firing, also set the latch
		{
			latch = value;
		}
	}
}

long Neuron::fire(void)
{

	lastfired = globalObject->current_timestep;
	setFiring(true);

	if (globalObject->logEvents)
	{
		std::vector<long> neurons = Server::getNeurons("nucleusAnteroventral", LayerType::output); // Layer 1 = input
		std::stringstream ss;
		for (int i = 0; i < neurons.size(); i++)
		{
			long neuronId = neurons[i];
			Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
			if (this->id == neuron->id)
			{
				ss << "output_neuron_firing: neuron=" << this->id << ", potential=" << potential; // << ", syanpses[";
				globalObject->writeEventLog(ss.str().c_str());
				break;
			}
		}
	}

	if (globalObject->logResponseMode)
	{
		// Get neurons in output layer
		std::vector<long> neurons = Server::getNeurons("nucleusAnteroventral", LayerType::output); // Layer 1 = input
		for (int i = 0; i < neurons.size(); i++)
		{
			long neuronId = neurons[i];
			Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
			if (this->id == neuron->id)
			{
				std::cout << "Output Neuron " << this->id << " firing." << std::endl;
				break;
			}
		}
	}


	// If already firing, or within refactory period, re-fire anyway.
	/*
		if (firing)
		{
			return;
		}
		else if (globalObject->current_timestep - lastfired < REFACTORY_PERIOD)
		{
			return;
		}
	*/
/*
	if (globalObject->logEvents)
	{
		std::stringstream ss;
		if (potential < 0)
		{
//			ss << "gettestresponse_neuron_firing: neuron=" << this->id << ", potential=" << potential; // << ", syanpses[";
			ss << "setactivationpattern_neuron_firing: neuron=" << this->id << ", potential=" << potential; // << ", syanpses[";
		}
		else
		{
			ss << "organic_neuron_firing: neuron=" << this->id << ", potential=" << potential; // << ", syanpses[";
		}
		globalObject->writeEventLog(ss.str().c_str());
	}
*/

	size_t aSize = axons.size();
	long lowestOffset = MAX_TIMEINTERVAL_BUFFER_SIZE;
	for (size_t i = 0; i < aSize; i++)
	{
		Axon *a = globalObject->axonDB.getComponent(axons[i]);
		long offset = a->fire();
		if(lowestOffset > offset) lowestOffset = offset;
	}

	// potential = RESTING_POTENTIAL;
	// globalObject->insertFiring(this);
	return lowestOffset;
}

void Neuron::cycle(void)
{
	// determine whether or not we need to fire an AP
	// collect all dendrites that have delivered an AP payload
	// Is the sum payload in this interval enough to depolarize this cell?
	// If so, attach an AP to each axon (typically only one).
	if (potential > threshold)
	{
		if (globalObject->logEvents)
		{
			std::stringstream ss;
			ss << "neuron_ready_to_fire: neuron=" << this->id << ", potential=" << potential << ", threshold-" << threshold;
			globalObject->writeEventLog(ss.str().c_str());
		}
		fire();
	}
	else
	{
		if (globalObject->current_timestep - lastfired < FIRING_WINDOW )
		{
			float ds = this->dendrites.size();
			float oldPotential = potential;
			float workPotential = 0;
			for (int i = 0; i < ds; i++)
			{
				long did = this->dendrites[i];
				Dendrite *d = globalObject->dendriteDB.getComponent(did);
				long sid = d->getSynapseId();
				Synapse *s = globalObject->synapseDB.getComponent(sid);
				long nId = d->getPreSynapticNeuronId();
				Neuron *preSynapticNeuron = globalObject->neuronDB.getComponent(nId);
				long preLastFired = preSynapticNeuron->lastfired;
				if(preLastFired > globalObject->current_timestep - FIRING_WINDOW)
				{
					long age = globalObject->current_timestep - preLastFired;
					float decayedWeight = s->getWeight() * exp(-DECAY_FACTOR * age);
					workPotential += decayedWeight;
				}
			}
			potential = workPotential / ds;

			if (oldPotential != potential)
			{

				if (globalObject->logEvents)
				{
					std::stringstream ss;
					ss << "neuron_potential_change: neuron=" << this->id << ", old=" << oldPotential << ", new=" << potential;
					globalObject->writeEventLog(ss.str().c_str());
				}

				if (potential >= threshold) // If we've exceeded threshold, fire
				{
					if (globalObject->logEvents)
					{
						std::stringstream ss;
						ss << "neuron_ready_to_fire: neuron=" << this->id << ", potential=" << potential << ", threshold-" << threshold;
						globalObject->writeEventLog(ss.str().c_str());
					}

					fire();
				}
			}
		}
	}
	/*
		else
		{
			if (firing && globalObject->current_timestep - lastfired > REFACTORY_PERIOD)
			{
				setFiring(false);
				potential = RESTING_POTENTIAL;
			}
		}
	*/
}

bool Neuron::isConnectedTo(Neuron *neuron)
{
	size_t aSize = neuron->axons.size();
	for (size_t j = 0; j < aSize; j++)
	{
		Axon *a = globalObject->axonDB.getComponent(neuron->axons[j]);
		std::vector<long> *synapseVector = a->getSynapses();
		size_t ssize = synapseVector->size();
		for (size_t i = 0; i < ssize; i++)
		{
			unsigned long sId = (*synapseVector)[i];

			Synapse *s = globalObject->synapseDB.getComponent(sId);
			Dendrite *d = globalObject->dendriteDB.getComponent(s->getOwningDendriteId());
			if (d->isSameNeuron(neuron->id))
			{
				return true;
			}
		}
	}
	return false;
}

std::vector<long> *Neuron::getAxonConnectedSynapses(void)
{

	std::vector<long> *connectedSynapses = new std::vector<long>();

	size_t aSize = axons.size();
	for (size_t j = 0; j < aSize; j++)
	{
		Axon *a = globalObject->axonDB.getComponent(axons[j]);

		std::vector<long> *it2 = a->getSynapses();
		size_t ssize = it2->size();
		for (size_t i = 0; i < ssize; i++)
		{
			connectedSynapses->push_back((*it2)[i]);
		}
	}
	return connectedSynapses;
}

bool Neuron::containsDendrite(long did)
{
	size_t dendriteSize = dendrites.size();
	for (size_t i = 0; i < dendriteSize; i++)
	{
		if (did == dendrites[i])
			return true;
	}
	return false;
}

bool Neuron::isAssociated(long synapseId)
{
	size_t dendriteSize = this->dendrites.size();
	for (size_t dendriteIndex = 0; dendriteIndex < dendriteSize; dendriteIndex++)
	{
		long dendriteId = dendrites[dendriteIndex];
		Dendrite *dendrite = globalObject->dendriteDB.getComponent(dendriteId);
		if (synapseId == dendrite->getSynapseId())
			return true;
	}
	return false;
}

//
void Neuron::applySTDP(std::pair<std::vector<Neuron *> *, std::vector<Neuron *> *> *neurons, long learningInterval)
{
	// Get list of connected neurons by tracing all synapses on the axon to dendrites to neurons

	std::vector<Neuron *> *preNeurons = neurons->first;
	std::vector<Neuron *> *postNeurons = neurons->second;
	size_t num_pre_neurons = preNeurons->size();
	size_t num_post_neurons = postNeurons->size();

	double A_plus = DEFAULT_STDP_RATE;	// Amplitude of potentiation
	double A_minus = DEFAULT_STDP_RATE; // Amplitude of depression
	double tau_plus = 10.0;				// Time constant for potentiation
	double tau_minus = 10.0;			// Time constant for depression

	if (num_pre_neurons == 0 && num_post_neurons == 0) // just return if no pre and post neurons firing
	{
		return;
	}

	/*If a presynaptic neuron fires a spike shortly before a postsynaptic neuron,
	the synaptic connection between them is typically strengthened (known as Long-Term Potentiation, LTP).
	*/

	for (size_t preNeuronIndex = 0; preNeuronIndex < num_pre_neurons; preNeuronIndex++)
	{
		Neuron *preneuron = (*preNeurons)[preNeuronIndex];

		if (preneuron->id != this->id)
		{
			Dendrite *preDendrite = globalObject->findConnectingDendrite(this, preneuron);
			if(preDendrite!=NULL)
			{
				long synapseId = preDendrite->getSynapseId();
				Synapse *synapse = globalObject->synapseDB.getComponent(synapseId);
				unsigned long lastTimeNeuronFired = (unsigned long)preneuron->lastfired; // timestamp

				float distance = (float)(this->lastfired - lastTimeNeuronFired);
				float delta = 0;
				float prev = preDendrite->getRate();
				long synapseId2 = preDendrite->getSynapseId();
				Synapse *synapse2 = globalObject->synapseDB.getComponent(synapseId2);

				if (distance != 0.0) // If we're in sync don't change anything
				{
					float prev = preDendrite->getRate();
					if (std::isinf(prev))
						prev = 0.1;
					float p1 = synapse->getPosition();
					float desiredOffset = distance;
					float newRate = desiredOffset / p1;

					if(newRate > prev)
						delta = newRate - prev;
					else 
						delta = prev - newRate;

					float adjustedRate = prev + delta;
					if(adjustedRate > MAXIMUM_DENDRITE_RATE)
						adjustedRate = MAXIMUM_DENDRITE_RATE;
					if(adjustedRate < -MAXIMUM_DENDRITE_RATE)
						adjustedRate = -MAXIMUM_DENDRITE_RATE;
					preDendrite->setRate(adjustedRate);


				}
				double deltaT = (double)(learningInterval - lastTimeNeuronFired);
				double weightChange = A_plus * std::exp(-deltaT / tau_plus);
				float oldWeight = synapse2->getWeight();
				float newWeight = oldWeight - (float)weightChange; // axonpreSynapseIdId strengthened (known as Long-Term Potentiation, LTP).
				if(newWeight > MAXIMUM_SYNAPSE_WEIGHT)
					newWeight = MAXIMUM_SYNAPSE_WEIGHT;  // cap the weight to within reason
				if(newWeight < -MAXIMUM_SYNAPSE_WEIGHT)
					newWeight = -MAXIMUM_SYNAPSE_WEIGHT;  // cap the weight to within reason
				synapse2->setWeight(newWeight);
//				std::cout << "applySTDP:neuronPre " << preneuron->id << " distance=" << distance << ", position =" << synapse->getPosition() << " fired before " << this->id << " by " << distance << "ms ratechg: " << delta << " from " << prev << " to " << preDendrite->getRate() << ", weight adjusted from " << oldWeight << " to " << newWeight << std::endl;
			}
		}
	}

	/*If the presynaptic neuron fires a spike shortly after the postsynaptic neuron,
	the synaptic connection is typically weakened (known as Long-Term Depression, LTD).
	*/
	for (size_t postNeuronIndex = 0; postNeuronIndex < num_post_neurons; postNeuronIndex++)
	{
		Neuron *postneuron = (*postNeurons)[postNeuronIndex];

		if (postneuron->id != this->id)
		{
			Dendrite *postDendrite = globalObject->findConnectingDendrite(this, postneuron);
			if(postDendrite!=NULL)
			{
				long synapseId = postDendrite->getSynapseId();
				Synapse *synapse = globalObject->synapseDB.getComponent(synapseId);
				unsigned long lastTimeNeuronFired = (unsigned long)postneuron->lastfired; // timestamp

				float distance = (float)(lastTimeNeuronFired - this->lastfired);
				float delta = 0;
				float prev = postDendrite->getRate();
				long synapseId2 = postDendrite->getSynapseId();
				Synapse *synapse2 = globalObject->synapseDB.getComponent(synapseId2);

				if (distance != 0.0) // If we're in sync don't change anything
				{
					if (std::isinf(prev))
						prev = 0.1;
					float p1 = synapse->getPosition();
					float desiredOffset = distance;
					float newRate = desiredOffset / p1;

					if(newRate > prev)
						delta = newRate - prev;
					else 
						delta = prev - newRate;
					float adjustedRate = prev + delta;
					if(adjustedRate > MAXIMUM_DENDRITE_RATE)
						adjustedRate = MAXIMUM_DENDRITE_RATE;
					if(adjustedRate < -MAXIMUM_DENDRITE_RATE)
						adjustedRate = -MAXIMUM_DENDRITE_RATE;


					postDendrite->setRate(adjustedRate);


				}
				double deltaT = (double)(lastTimeNeuronFired - learningInterval);
				double weightChange = A_plus * std::exp(-deltaT / tau_plus);
				float oldWeight = synapse2->getWeight();
				float newWeight = oldWeight + (float)weightChange; // axonpreSynapseIdId weakened (known as Long-Term Depression, LTD).
				if(newWeight > MAXIMUM_SYNAPSE_WEIGHT)
					newWeight = MAXIMUM_SYNAPSE_WEIGHT;  // cap the weight to within reason
				if(newWeight < -MAXIMUM_SYNAPSE_WEIGHT)
					newWeight = -MAXIMUM_SYNAPSE_WEIGHT;  // cap the weight to within reason
				synapse2->setWeight(newWeight);

//				std::cout << "applySTDP:neuronPost " << postneuron->id << " distance=" << distance << ", position =" << synapse->getPosition() << " fired before " << this->id << " by " << distance << "ms ratechg: " << delta << " from " << prev << " to " << postDendrite->getRate()  << ", weight adjusted from " << oldWeight << " to " << newWeight << std::endl;
			}
		}
	}
}

Tuple *Neuron::getImage(void)
{
	long axonCount = (u_int32_t)axons.size();
	long dendriteCount = (u_int32_t)dendrites.size();

	size_t size = sizeof(nucleusType) + sizeof(parentId) + sizeof(neuronType) + sizeof(threshold) + sizeof(potential) +
				  sizeof(location.x) + sizeof(location.y) + sizeof(location.z) + sizeof(axonCount) + (axonCount * sizeof(long)) + sizeof(dendriteCount) + (dendriteCount * sizeof(long));

	char *image = globalObject->allocClearedMemory(size);
	char *ptr = (char *)image;
	unsigned long processId = 0;

	memcpy(ptr, &nucleusType, sizeof(nucleusType));
	ptr += sizeof(nucleusType);
	memcpy(ptr, &parentId, sizeof(parentId));
	ptr += sizeof(parentId);
	memcpy(ptr, &neuronType, sizeof(neuronType));
	ptr += sizeof(neuronType);
	memcpy(ptr, &threshold, sizeof(threshold));
	ptr += sizeof(threshold);
	memcpy(ptr, &potential, sizeof(potential));
	ptr += sizeof(potential);
	memcpy(ptr, &location.x, sizeof(location.x));
	ptr += sizeof(location.x);
	memcpy(ptr, &location.y, sizeof(location.y));
	ptr += sizeof(location.y);
	memcpy(ptr, &location.z, sizeof(location.z));
	ptr += sizeof(location.z);
	memcpy(ptr, &axonCount, sizeof(axonCount));
	ptr += sizeof(axonCount);
	memcpy(ptr, &dendriteCount, sizeof(dendriteCount));
	ptr += sizeof(dendriteCount);

	for (size_t i = 0; i < axonCount; i++)
	{
		processId = axons[i];
		memcpy(ptr, &processId, sizeof(processId));
		ptr += sizeof(processId);
	}
	for (size_t i = 0; i < dendriteCount; i++)
	{
		processId = dendrites[i];
		memcpy(ptr, &processId, sizeof(processId));
		ptr += sizeof(processId);
	}

	Tuple *tuple = new Tuple();
	tuple->objectPtr = image;
	tuple->value = size;

	return tuple;
}

Neuron *Neuron::instantiate(long key, size_t len, void *data)
{

	unsigned long defaultClusterId = ComponentType::ComponentTypeCluster;

	long axonCount = 0;
	long dendriteCount = 0;

	size_t size = sizeof(nucleusType) + sizeof(parentId) + sizeof(neuronType) + sizeof(threshold) + sizeof(potential) +
				  sizeof(location.x) + sizeof(location.y) + sizeof(location.z) + sizeof(axonCount) + sizeof(dendriteCount);

	Neuron *neuron = new Neuron(defaultClusterId, INTER_NUCLEUS);
	neuron->id = key;
	char *ptr = (char *)data;

	memcpy(&neuron->nucleusType, ptr, sizeof(nucleusType));
	ptr += sizeof(nucleusType);
	memcpy(&neuron->parentId, ptr, sizeof(parentId));
	ptr += sizeof(parentId);
	memcpy(&neuron->neuronType, ptr, sizeof(neuronType));
	ptr += sizeof(neuronType);
	memcpy(&neuron->threshold, ptr, sizeof(threshold));
	ptr += sizeof(threshold);
	memcpy(&neuron->potential, ptr, sizeof(potential));
	ptr += sizeof(potential);
	memcpy(&neuron->location.x, ptr, sizeof(location.x));
	ptr += sizeof(location.x);
	memcpy(&neuron->location.y, ptr, sizeof(location.y));
	ptr += sizeof(location.y);
	memcpy(&neuron->location.z, ptr, sizeof(location.z));
	ptr += sizeof(location.z);
	memcpy(&axonCount, ptr, sizeof(axonCount));
	ptr += sizeof(axonCount);
	memcpy(&dendriteCount, ptr, sizeof(dendriteCount));
	ptr += sizeof(dendriteCount);

	for (size_t i = 0; i < axonCount; i++)
	{
		long thisKey;
		memcpy(&thisKey, ptr, sizeof(long));
		ptr += sizeof(long);
		neuron->axons.push_back(thisKey);
	}
	for (size_t i = 0; i < dendriteCount; i++)
	{
		long thisKey;
		memcpy(&thisKey, ptr, sizeof(long));
		ptr += sizeof(long);
		neuron->dendrites.push_back(thisKey);
		// neuron->dendriteMap.insert
		Dendrite *dendrite = globalObject->dendriteDB.getComponent(thisKey);
		neuron->dendriteMap.insert(std::make_pair(dendrite->getPreSynapticNeuronId(), thisKey));
	}

	return neuron;
}

long Neuron::getCurrentTimestep(void)
{
	return globalObject->current_timestep;
}

std::string Neuron::getLocationOfNeuron(void)
{
	std::string retString;

	// iterate through nuclei
	// nucleus base=300000000
	for (long nucidx = 0; nucidx < globalObject->nucleusDB.size(); nucidx++)
	{
		long nucid = globalObject->componentBase[ComponentTypeNucleus] + nucidx;
		Nucleus *nucleus = globalObject->nucleusDB.getComponent(nucid);
		int columns = nucleus->columns.size();
		for (int i = 0; i < columns; i++)
		{
			Column *column = globalObject->columnDB.getComponent(nucleus->columns[i]);
			int layers = column->layers.size();
			for (int j = 0; j < layers; j++)
			{
				Layer *layer = globalObject->layerDB.getComponent(column->layers[j]);
				int clusters = layer->clusters.size();
				for (int k = 0; k < clusters; k++)
				{
					Cluster *cluster = globalObject->clusterDB.getComponent(layer->clusters[k]);
					int neurons = cluster->neurons.size();
					for (int m = 0; m < neurons; m++)
					{
						if (cluster->neurons[m] == this->id) // if equal we found our neuron
						{
							std::stringstream sss;

							// sss << nucleus->name << "/Column " << column->id << "/Layer " << (j+1) << "/Cluster " << cluster->id << "/Neuron " << this->id;
							sss << nucleus->name << "/" << (j + 1) << "/" << this->id;
							retString = sss.str();
							break;
						}
					}
				}
			}
		}
	}

	return retString;
}

bool Neuron::isFiring(float delay) 
{
	if (delay == 0) {
		return firing;
	}
	else
	{
		long timediff = globalObject->current_timestep - lastfired;
		long diff = std::abs(timediff);
		if (delay > diff) // if within the spacified window (+ or - delay value), report firing state
			return true;
		else
			return false; // else return not firing
	}
}
