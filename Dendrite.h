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
#include <map>
#include "Process.h"
#include "ActionPotential.h"

class Synapse;
class Neuron;
class Dendrite: public Process
{
	Dendrite(void);
	Dendrite(Neuron *neuron);
	Dendrite(Neuron *neuron, long newId);
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const size_t version)
	{
		ar & boost::serialization::base_object<NNComponent>(*this);
        ar & preSynapticNeuronId;
		ar & synapseId;
	}

public:
	virtual ~Dendrite(void);
	static Dendrite *create(Neuron *postSynapticNeuron,Neuron *preSynapticNeuron);
	void fire(void);
	Tuple *getImage(void);
	static Dendrite *instantiate(long key, size_t len, void *data);

	inline bool isSameNeuron(long nid) { return preSynapticNeuronId == nid; };
	inline void setPreSynapticNeuronId(long nId) { this->preSynapticNeuronId = nId; };
	inline long getPreSynapticNeuronId(void) { return this->preSynapticNeuronId; };
	inline void setPostSynapticNeuronId(long nId) { this->postSynapticNeuronId = nId; };
	inline long getPostSynapticNeuronId(void) { return this->postSynapticNeuronId; };
	inline long getSynapseId(void) { return synapseId; };
	void toJSON(std::ofstream& outstream);


private:
	void save(void);
	void commit(void);
	long preSynapticNeuronId;
	long postSynapticNeuronId;
	long synapseId;
};
