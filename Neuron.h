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

#pragma once
#include <cmath>
#include "NNComponent.h"
#include <map>
#include <vector>
#include "Axon.h"
#include "Dendrite.h"
#include "Synapse.h"
#include "ActionPotential.h"
#include "SpatialDetails.h"
#include "Cluster.h"

enum NeuronType { Pyramidal, Unipolar, Bipolar, Multipolar, Purkinje };

class Neuron: public NNComponent
{
	Neuron(unsigned long parentId, int nucleusType);
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const size_t version)
	{
		ar & boost::serialization::base_object<NNComponent>(*this);

		for(unsigned int i=0;i<axons.size();i++)
		{
			ar & axons[i];
		}
		for(unsigned int i=0;i<dendrites.size();i++)
		{
			ar & dendrites[i];
		}
        ar & threshold;
        ar & potential;
		ar & location.x;
		ar & location.y;
		ar & location.z;
		ar & lastfired;
        ar & firing;
	}

public:
	virtual ~Neuron(void);
	static Neuron*create(SpatialDetails details, NeuronType nType, unsigned long parentId, int nucleusType);
	static Neuron*recreate(void);
	void initializeRandom(void);
	bool isAssociated(long synapseId);
	void connect(Neuron *preSynapticNeuron, Neuron *postSynapticNeuron, float polarity);
	void connectTo(Neuron *targetNeuron, float polarity);
	void connectFrom(Neuron *sourceNeuron, float polarity);
	void projectTo(Neuron *targetNeuron, float polarity);
	void cycle(void);
	bool isConnectedTo(Neuron *neuron);
	void applySTDP(ActionPotential *ap, std::pair<std::vector<Tuple*>*, std::vector<Tuple*>* >*slices);
	void applySTDP(std::pair<std::vector<Neuron*>*, std::vector<Neuron*>* >*neurons, long learningInterval);
	long fire(void);
	Tuple *getImage(void);
	static Neuron *instantiate(long key, size_t len, void *data);

	long getNucleusId(void);

	long getCurrentTimestep(void);

	bool containsDendrite(long did);

	bool isFiring(float delay = 0);
	inline bool hasFired(void) { return latch; };
	inline void resetLatch(void) { latch = false; };
	inline long getLastFired(void) { return lastfired; };
	inline void setLastFired(unsigned long value) { lastfired = value; setDirty(true); };
	inline std::vector<long> *getAxons(void) { return &axons; };
	inline std::vector<long> *getDendrites(void) { return &dendrites; };
	std::string getLocationOfNeuron(void);

	void setFiring(bool value=true);

	std::vector<long> *getAxonConnectedSynapses(void);
	bool isFromSensoryNucleus(void);

	void toJSON(std::ofstream& outstream);



private:
	void save(void);
	void commit(void);

	std::vector<long> axons;
	std::vector<long> dendrites;
public:	

	std::unordered_map<long, long> dendriteMap; 
	float threshold;
	float potential;
	unsigned long lastfired;
	bool firing;
	NeuronType neuronType;
	bool latch;// latch is used as a flip-flop to store firing status until the status is retrieved by IO
	Location3D location;
	int nucleusType; // What type of nucleus does this neuron belong to (eg 0 = interneuron, 1=sensory, etc) a sensory nuclear (T/F)?
};
