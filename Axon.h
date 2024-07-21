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

