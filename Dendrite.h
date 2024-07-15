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
