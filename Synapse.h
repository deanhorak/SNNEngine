/*
 * Proprietary License
 * 
 * Copyright (c) 2024-2025 Dean S Horak
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
#include <boost/asio.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include "NNComponent.h"
#include "ActionPotential.h"
#include "Dendrite.h"
#include "FloatTuple.h"

class Synapse: public NNComponent
{
    friend class boost::serialization::access;
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

    Dendrite *getOwningDendrite(void);
    inline long getOwningDendriteId(void) { return owningDendriteId; };
    inline void setOwningDendriteId(long newId) { 
        owningDendriteId = newId; 
        setDirty(true); 
    };

    inline float getWeight(void) { return weight; };
    void setWeight(float value);
    inline float getPosition(void) { return position; };
    inline void setPosition(float value) { position=value; setDirty(true); };

    float getPolarity() const { return polarity; }

    void toJSON(std::ofstream& outstream);
    //FloatTuple sumweightsWithSpikePropagation(Neuron *neuron);

        // Renamed from sumweightsWithSpikePropagation to computeWeightedSpike
    // Returns the effective weighted input from all presynaptic activity
    float computeWeightedSpike(Neuron *postNeuron);

    float mapRange(float value, float inputMin = 0, float inputMax = 100000, float outputMin = -65.0, float outputMax = 65.0);

    void incrementTrace();
    void decayTrace();

    unsigned long postSynapticNeuronId;
    float polarity;

    float trace;  
    float traceDecay; 
    float traceIncrement; 

private:
    Synapse(Dendrite* dendrite);
    Synapse(void);

    void save(void);
    void commit(void);

    unsigned long owningDendriteId;
    float weight;
    float position;

    float weightUpdateCounter;

//    boost::mutex synapse_mutex; 
        // Thread safety
    std::mutex synapse_mutex;

};
