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
#include <boost/thread.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include "NNComponent.h"
#include "ActionPotential.h"
#include "Dendrite.h"

class Synapse: public NNComponent
{
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const size_t version)
	{
		ar & boost::serialization::base_object<NNComponent>(*this);
		ar & owningDendriteId;
        ar & weight;
        ar & position;
	}

public:
	virtual ~Synapse(void);
	static Synapse *create(Dendrite *dendrite, float polar);
	void receiveAP(ActionPotential *ap);
	Tuple *getImage(void);
	static Synapse *instantiate(long key, size_t len, void *data);

//	inline Dendrite *getOwningProcess(void) { return owningprocess; };
	Dendrite *getOwningDendrite(void);
	inline long getOwningDendriteId(void) { return owningDendriteId; };
	inline void setOwningDendriteId(long newId) 
	{ 
		owningDendriteId = newId; 
		setDirty(true); 
/*
		if(id == 1001200000) 
		{
			std::cout << "Synapse 1001200000 being set to " << owningprocessId << std::endl;
		}
*/
	};
	inline float getWeight(void) { return weight; };
	void setWeight(float value);
	inline float getPosition(void) { return position; };
	inline void setPosition(float value) { position=value; setDirty(true); };
	void toJSON(std::ofstream& outstream);
	float sumweights(Neuron *neuron);

	unsigned long postSynapticNeuronId;

	float polarity;

private:

	Synapse(Dendrite* dendrite);
	Synapse(void);

	void save(void);
	void commit(void);


	//	Dendrite *owningprocess;
	unsigned long owningDendriteId;
	float weight;
	float position;

	float weightUpdateCounter;

	boost::mutex synapse_mutex; 

	
};
