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

/* Synapse.cpp */

#include "Synapse.h"
#include "Global.h"         // TODO: Adjust to your environment
#include "TR1Random.h"
#include "Dendrite.h"
#include "Neuron.h"
#include "ActionPotential.h"
#include "Axon.h"
#include <sstream>
#include <cmath>
#include <limits>
#include <cstring>
#include <algorithm>

extern long FIRING_WINDOW;               // Example external
extern float WEIGHT_GRADATION;           // Example external
extern float RATE_GRADATION;             // Example external
//extern float RESTING_POTENTIAL;          // Example external

// Example clamp function for older C++ (if not using C++17)
template <typename T>
T clampValue(T v, T lo, T hi) {
    return std::max(lo, std::min(v, hi));
}

Synapse::Synapse(Dendrite *dendrite) : NNComponent(ComponentTypeSynapse)
{
    weight = 0.1f;
    polarity = EXCITATORY_SYNAPSE;  // default polarity
    weightUpdateCounter = 0;
    trace = 0.0f;
    traceDecay = 0.95f;
    traceIncrement = 0.05f;
    position = (float)tr1random->generate(1, 100);
    setOwningDendriteId(dendrite->id);
    dendrite->setDirty();
}

Synapse::Synapse() : NNComponent(ComponentTypeSynapse)
{
    weight = 0.1f;
    polarity = EXCITATORY_SYNAPSE;  // default polarity
    weightUpdateCounter = 0;
    trace = 0.0f;
    traceDecay = 0.95f;
    traceIncrement = 0.05f;
    position = (float)tr1random->generate(1, 100);
    setOwningDendriteId(0);
}

Synapse::~Synapse(void)
{
}

void Synapse::toJSON(std::ofstream &outstream)
{
    outstream << " { \"_type\": \"Synapse\", "
              << "\"id\": " << id
              << ", \"owningDendriteId\": " << owningDendriteId
              << ", \"weight\": " << weight
              << ", \"position\": " << position
              << ", \"polarity\": " << polarity
              << " } " << std::endl;
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

float Synapse::mapRange(float value, float inputMin, float inputMax, float outputMin, float outputMax)
{
    return outputMin + ((value - inputMin) / (inputMax - inputMin)) * (outputMax - outputMin);
}

// -------------------------------------------------------------
// Trace-based STDP methods
// -------------------------------------------------------------
void Synapse::incrementTrace() {
    this->trace += this->traceIncrement;
    if (this->trace > 1.0f) {
        this->trace = 1.0f;
    }
}

void Synapse::decayTrace() {
    this->trace *= this->traceDecay;
}

// -------------------------------------------------------------
// Compute the effective weighted input from presynaptic spikes
// -------------------------------------------------------------
float Synapse::computeWeightedSpike(Neuron *postNeuron)
{
    // Summation logic similar to the original sumweightsWithSpikePropagation.
    // For demonstration, we replicate the approach of scanning *all* dendrites
    // of the postNeuron. Adjust as needed.

    float totalWeight = 0.0f;
    float selectedCount = 0.0f;

    // Potentially track min/max for advanced mapping, if you still want that
    float minWeight = std::numeric_limits<float>::max();
    float maxWeight = std::numeric_limits<float>::lowest();

    std::vector<long> *dendrites = postNeuron->getDendrites();
    for (auto &dId : *dendrites)
    {
        Dendrite *d = globalObject->dendriteDB.getComponent(dId);
        if (!d) continue;

        long preSynId = d->getPreSynapticNeuronId();
        Neuron *preNeuron = globalObject->neuronDB.getComponent(preSynId);
        if (!preNeuron) continue;

        unsigned long lastFired = preNeuron->lastfired;
        if (lastFired == 0) continue;

        float timeDiff = (float)(globalObject->getCurrentTimestamp() - lastFired);
        if (timeDiff < FIRING_WINDOW)
        {
            // get the synapse from the dendrite
            Synapse *syn = globalObject->synapseDB.getComponent(d->getSynapseId());
            if (!syn) continue;

            float attenuation = std::exp(-timeDiff / FIRING_WINDOW);
            float weightedSpike = syn->getWeight() * attenuation;

            minWeight = std::min(minWeight, weightedSpike);
            maxWeight = std::max(maxWeight, weightedSpike);

            totalWeight += weightedSpike;
            selectedCount += 1.0f;
        }
    }

    if (selectedCount > 0.0f)
    {
        float normalizedWeight = totalWeight / selectedCount;

        // Optional further mapping or scaling:
        float scaledInput = normalizedWeight; 
        // e.g. scale to a range, etc.
        // scaledInput = mapRange(normalizedWeight, minWeight, maxWeight, 0.0f, RESTING_POTENTIAL);

        return scaledInput;
    }
    else
    {
        return 0.0f;
    }
}

// -------------------------------------------------------------
// Main entry point for an incoming spike
// -------------------------------------------------------------
void Synapse::receiveAP(ActionPotential *ap)
{
    // 1) Increment local STDP trace (presynaptic event).
    incrementTrace();

    // 2) Compute the weighted input from presynaptic activity
    Dendrite *dendrite = getOwningDendrite();
    if(!dendrite) return;

    long postNeuronId = dendrite->getPostSynapticNeuronId();
    Neuron *postNeuron = globalObject->neuronDB.getComponent(postNeuronId);
    if(!postNeuron) return;

    float weightedInput = computeWeightedSpike(postNeuron);

    // 3) Pass this input to the post-synaptic neuron
    postNeuron->integrateSynapticInput(weightedInput, this->id);

    // We DO NOT trigger the neuron's fire() here.
    // The neuron decides on its own when to fire.
}

// -------------------------------------------------------------
// Serialization / Deserialization
// -------------------------------------------------------------
Tuple* Synapse::getImage(void)
{
    size_t size = sizeof(owningDendriteId) +
                  sizeof(postSynapticNeuronId) +
                  sizeof(weight) +
                  sizeof(polarity) +
                  sizeof(weightUpdateCounter) +
                  sizeof(position);

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

Synapse* Synapse::instantiate(long key, size_t len, void *data)
{
    (void)len;
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

Dendrite* Synapse::getOwningDendrite(void)
{
    return globalObject->dendriteDB.getComponent(owningDendriteId);
}

// -------------------------------------------------------------
// Set weight with clamping and optional logging
// -------------------------------------------------------------
void Synapse::setWeight(float inValue)
{
    std::lock_guard<std::mutex> lock(synapse_mutex);
    float newWeight = clampValue(inValue, MINIMUM_SYNAPSE_WEIGHT, MAXIMUM_SYNAPSE_WEIGHT);

    if (std::fabs(newWeight - weight) > 1e-9)
    {
        float oldWeight = weight;
        weight = newWeight;
        weightUpdateCounter++;
        setDirty(true);

        // Optional logging:
        if(globalObject->logEvents)
        {
            std::stringstream ss;
            Dendrite *d = getOwningDendrite();
            if(d)
            {
                ss << "synapse_weight_change: synapse=" << this->id
                   << ", dendrite=" << this->owningDendriteId
                   << ", old=" << oldWeight
                   << ", new=" << weight
                   << ", presynapticNeuron=" << d->getPreSynapticNeuronId()
                   << ", postsynapticNeuron=" << d->getPostSynapticNeuronId();
            }
            else
            {
                ss << "synapse_weight_change: synapse=" << this->id
                   << ", old=" << oldWeight << ", new=" << weight;
            }
            globalObject->writeEventLog(ss.str().c_str());
        }
    }
}
