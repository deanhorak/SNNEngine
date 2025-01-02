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

#include "Axon.h"
#include "TR1Random.h"
#include "Global.h"
#include "Neuron.h"
#include "Synapse.h"
#include "Dendrite.h"
#include "TimedEvent.h"
#include <iostream>

// External globals (if any)
extern long REFACTORY_PERIOD;

Axon::Axon(Neuron *neuron) : Process(ComponentTypeAxon)
{
    this->neuronId = neuron->id;
    setRate(DEFAULT_AXON_RATE,false);
    float dist = DEFAULT_AXON_DISTANCE;
    setDistance(dist);
    this->parentId = this->neuronId;
    neuron->setDirty();
}

Axon::~Axon(void)
{
}

void Axon::toJSON(std::ofstream &outstream)
{
    std::string sep("");
    outstream << "                            { \"_type\": \"Axon\", \"id\": " << id << ", \"neuronId\": " << neuronId << ", \"synapses\": [" << std::endl;
    for (unsigned int i = 0; i < synapses.size(); i++)
    {
        outstream << sep;
        sep = ",";
        Synapse *s = globalObject->synapseDB.getComponent(synapses[i]);
        s->toJSON(outstream);
    }
    outstream << "                            ] } " << std::endl;
}

void Axon::save(void)
{
    globalObject->axonDB.save(this);
}

void Axon::commit(void)
{
    globalObject->axonDB.addToCache(this);
}

Axon *Axon::create(Neuron *neuron)
{
    Axon *a = new Axon(neuron);
    a->id = globalObject->nextComponent(ComponentTypeAxon);
    globalObject->insert(a);
    return a;
}

void Axon::initializeRandom(void)
{
    // Optionally create random synapses if desired
    // Currently no random init logic here
}

void Axon::insertSynapse(long synapseId)
{
    bool found = false;
    size_t synapseCount = synapses.size();
    for(size_t i=0;i<synapseCount;i++)
    {
        if(synapseId == synapses[i])    
            found = true;
    }
    if(!found)
    {
        synapses.push_back(synapseId);
        setDirty();
    }
}

Range Axon::fire(void)
{
    Range range;
    if(globalObject->logEvents) 
    {
        std::stringstream ss;
        ss << "axon_fire: neuron=" << this->neuronId;
        globalObject->writeEventLog(ss.str().c_str());
    }

    long highestOffset = -MAX_TIMEINTERVAL_OFFSET;
    long lowestOffset = MAX_TIMEINTERVAL_OFFSET;
    size_t synapseSize = this->synapses.size();

    for(size_t synapseIndex = 0;synapseIndex < synapseSize; synapseIndex++) 
    {
        long synapseId = this->synapses[synapseIndex];
        Synapse *thisSynapse = globalObject->synapseDB.getComponent(synapseId);

        long dendriteId = thisSynapse->getOwningDendriteId();
        Dendrite *thisDendrite = globalObject->dendriteDB.getComponent(dendriteId);

        float pos = thisSynapse->getPosition();
        float lclRate = thisDendrite->getRate();
        long offset = ActionPotential::computeOffset(pos, lclRate) + REFACTORY_PERIOD;

        if(offset > 0)
        {
            if(lowestOffset > offset) 
                lowestOffset = offset;
            if(highestOffset < offset)
                highestOffset = offset;

            long timeslice = globalObject->getCurrentTimestamp()+ offset;
            TimedEvent::create(timeslice, thisDendrite, thisSynapse->id);

            if(globalObject->logEvents) 
            {
                std::stringstream ss;
                ss << "timed_event: synapse=" << thisSynapse->id << ", dendrite=" << thisDendrite->id << ", neuron=" << thisDendrite->getPostSynapticNeuronId() << ", presynaptic_neuron=" << thisDendrite->getPreSynapticNeuronId() << ", offset=" << offset << ", timeslice=" << timeslice;
                globalObject->writeEventLog(ss.str().c_str());
            }
        } 
    }

    range.low = lowestOffset;
    range.high = highestOffset;
    return range;
}

Tuple *Axon::getImage(void)
{
    long synapseCount = (long)synapses.size();
    float lclDistance = getDistance();
    float lclRate = getRate();

    size_t size = sizeof(parentId) + sizeof(lclDistance) + sizeof(lclRate) + sizeof(neuronId) +
                  sizeof(synapseCount) + (synapseCount * sizeof(long));

    char *image = globalObject->allocClearedMemory(size);
    char *ptr = (char *)image;

    memcpy(ptr, &parentId, sizeof(parentId));
    ptr += sizeof(parentId);
    memcpy(ptr, &lclDistance, sizeof(lclDistance));
    ptr += sizeof(lclDistance);
    memcpy(ptr, &lclRate, sizeof(lclRate));
    ptr += sizeof(lclRate);
    memcpy(ptr, &neuronId, sizeof(neuronId));
    ptr += sizeof(neuronId);
    memcpy(ptr, &synapseCount, sizeof(synapseCount));
    ptr += sizeof(synapseCount);

    for (size_t i = 0; i < (size_t)synapseCount; i++)
    {
        long k = synapses[i];
        memcpy(ptr, &k, sizeof(k));
        ptr += sizeof(k);
    }

    Tuple *tuple = new Tuple();
    tuple->objectPtr = image;
    tuple->value = size;
    return tuple;
}

Axon *Axon::instantiate(long key, size_t len, void *data)
{
    (void)len; 
    long lclsynapseCount = 0;
    long lclneuronId = 0;
    long lclpId = 0;
    float lclDistance = 0;
    float lclRate = 0;

    Axon *axon = new Axon();
    axon->id = key;

    char *ptr = (char *)data;
    memcpy(&lclpId, ptr, sizeof(lclpId));
    ptr += sizeof(lclpId);
    memcpy(&lclDistance, ptr, sizeof(lclDistance));
    ptr += sizeof(lclDistance);
    memcpy(&lclRate, ptr, sizeof(lclRate));
    ptr += sizeof(lclRate);
    memcpy(&lclneuronId, ptr, sizeof(lclneuronId));
    ptr += sizeof(lclneuronId);
    memcpy(&lclsynapseCount, ptr, sizeof(lclsynapseCount));
    ptr += sizeof(lclsynapseCount);

    axon->parentId = lclpId;
    axon->setDistance(lclDistance);
    axon->setRate(lclRate);
    axon->neuronId = lclneuronId;

    if (lclsynapseCount > 1000000 || lclsynapseCount < 0) {
        std::cout << "getImage: Axon " << axon->id << " has " << lclsynapseCount << " synapse\n";
    }
    else
    {
        for (size_t i = 0; i < (size_t)lclsynapseCount; i++)
        {
            long thisKey;
            memcpy(&thisKey, ptr, sizeof(long));
            ptr += sizeof(long);
            axon->synapses.push_back(thisKey);
        }
    }

    axon->setDirty(false);
    return axon;
}

Axon::Axon(void) : Process(ComponentTypeAxon)
{
    // Default constructor if needed
}
