#pragma once
#include <map>
#include <vector>
#include "Process.h"
#include "Synapse.h"
#include "ActionPotential.h"

class Axon: public Process
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const size_t version)
	{
		ar & boost::serialization::base_object<NNComponent>(*this);
        ar & neuronId;

        for(unsigned int i=0;i<synapses.size();i++)
        {
        	ar & synapses[i];
        }
	}

public:
	virtual ~Axon(void);
	static Axon *create(Neuron *neuron);
	void initializeRandom(void);
	long fire(void);
	Tuple *getImage(void);
	static Axon *instantiate(long key, size_t len, void *data);
	inline std::vector<long> *getSynapses(void) { return &synapses; };
	void insertSynapse(long synapseId); 
	void toJSON(std::ofstream& outstream);

	long neuronId;

private:

	Axon(void);
	Axon(Neuron* neuron);

	void save(void);
	void commit(void);

	std::vector<long> synapses;
};

