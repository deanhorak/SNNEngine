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

/* Neuron.cpp */

#include "Neuron.h"
#include "TR1Random.h"
#include "Global.h"
#include "Axon.h"
#include "Dendrite.h"
#include "Synapse.h"
#include "SpatialDetails.h"
#include <sstream>
#include <cmath>
#include <algorithm>

extern long   PROPAGATION_DELAY_MICROSECONDS; // example external
extern double DECAY_FACTOR;                   // example external
extern float  RATE_GRADATION;                 // example external
extern long REFACTORY_PERIOD;

// Example external method from your environment
// std::vector<long> Server::getNeurons(const std::string &nucleusName, LayerType layerType);

Neuron::Neuron(unsigned long parentId, int nucleusType)
    : NNComponent(ComponentTypeNeuron)
{
    this->parentId        = parentId;
    this->nucleusType     = nucleusType;
    this->neuronPolarity  = Polarity::EXCITATORY_NEURON; // default
    this->threshold       = INITIAL_THRESHOLD;
    this->membranePotential = RESTING_POTENTIAL;
    this->firing          = false;
    this->latch           = false;
    this->lastfired       = 0;
    this->recentSpikeCount= 0.0f;
    this->lastUpdateStep  = globalObject->getCurrentTimestamp();

    // Example random location
    SpatialDetails sd(2500, 2500, 2500, 5000, 5000, 5000);
    sd.randomizeLocation();
    this->location = sd.location;
}

Neuron::~Neuron(void)
{
}

Neuron* Neuron::create(SpatialDetails details, NeuronType nType,
                       unsigned long parentId, int nucleusType)
{
    Neuron* n = new Neuron(parentId, nucleusType);
    n->id = globalObject->nextComponent(ComponentTypeNeuron);
    n->neuronType = nType;
    n->location = details.location;

    // Create an axon for the neuron
    Axon *a = Axon::create(n);
    n->axons.push_back(a->id);

    globalObject->insert(n);
    return n;
}

void Neuron::toJSON(std::ofstream &outstream)
{
    // Minimal example
    outstream << "{ \"_type\": \"Neuron\", \"id\": " << id
              << ", \"threshold\": " << threshold
              << ", \"potential\": " << membranePotential
              << ", \"polarity\": " << neuronPolarity
              << ", \"location\": [" << location.x << ", "
                                    << location.y << ", "
                                    << location.z << "] }" << std::endl;
}

void Neuron::save(void)
{
    globalObject->neuronDB.save(this);
}

void Neuron::commit(void)
{
    globalObject->neuronDB.addToCache(this);
}

Tuple* Neuron::getImage(void)
{
    long axonCount = (long)axons.size();
    long dendriteCount = (long)dendrites.size();

    size_t size = sizeof(nucleusType)
                  + sizeof(parentId)
                  + sizeof(neuronType)
                  + sizeof(threshold)
                  + sizeof(membranePotential)
                  + sizeof(neuronPolarity)
                  + sizeof(location.x)
                  + sizeof(location.y)
                  + sizeof(location.z)
                  + sizeof(axonCount)
                  + sizeof(dendriteCount)
                  + (axonCount * sizeof(long))
                  + (dendriteCount * sizeof(long));

    char *image = globalObject->allocClearedMemory(size);
    char *ptr = image;

    memcpy(ptr, &nucleusType, sizeof(nucleusType));
    ptr += sizeof(nucleusType);

    memcpy(ptr, &parentId, sizeof(parentId));
    ptr += sizeof(parentId);

    memcpy(ptr, &neuronType, sizeof(neuronType));
    ptr += sizeof(neuronType);

    memcpy(ptr, &threshold, sizeof(threshold));
    ptr += sizeof(threshold);

    memcpy(ptr, &membranePotential, sizeof(membranePotential));
    ptr += sizeof(membranePotential);

    memcpy(ptr, &neuronPolarity, sizeof(neuronPolarity));
    ptr += sizeof(neuronPolarity);

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

    for (auto &aId : axons)
    {
        memcpy(ptr, &aId, sizeof(aId));
        ptr += sizeof(aId);
    }
    for (auto &dId : dendrites)
    {
        memcpy(ptr, &dId, sizeof(dId));
        ptr += sizeof(dId);
    }

    Tuple* tuple = new Tuple();
    tuple->objectPtr = image;
    tuple->value = size;
    return tuple;
}

Neuron* Neuron::instantiate(long key, size_t len, void *data)
{
    (void)len;
    unsigned long defaultClusterId = ComponentType::ComponentTypeCluster;

    long axonCount = 0;
    long dendriteCount = 0;

    Neuron *neuron = new Neuron(defaultClusterId, 0 /*nucleusType*/);
    neuron->id = key;
    char *ptr = (char *)data;

    memcpy(&neuron->nucleusType, ptr, sizeof(neuron->nucleusType));
    ptr += sizeof(neuron->nucleusType);
    memcpy(&neuron->parentId, ptr, sizeof(neuron->parentId));
    ptr += sizeof(neuron->parentId);
    memcpy(&neuron->neuronType, ptr, sizeof(neuron->neuronType));
    ptr += sizeof(neuron->neuronType);
    memcpy(&neuron->threshold, ptr, sizeof(neuron->threshold));
    ptr += sizeof(neuron->threshold);
    memcpy(&neuron->membranePotential, ptr, sizeof(neuron->membranePotential));
    ptr += sizeof(neuron->membranePotential);
    memcpy(&neuron->neuronPolarity, ptr, sizeof(neuron->neuronPolarity));
    ptr += sizeof(neuron->neuronPolarity);
    memcpy(&neuron->location.x, ptr, sizeof(neuron->location.x));
    ptr += sizeof(neuron->location.x);
    memcpy(&neuron->location.y, ptr, sizeof(neuron->location.y));
    ptr += sizeof(neuron->location.y);
    memcpy(&neuron->location.z, ptr, sizeof(neuron->location.z));
    ptr += sizeof(neuron->location.z);
    memcpy(&axonCount, ptr, sizeof(axonCount));
    ptr += sizeof(axonCount);
    memcpy(&dendriteCount, ptr, sizeof(dendriteCount));
    ptr += sizeof(dendriteCount);

    for (int i = 0; i < axonCount; i++)
    {
        long aId;
        memcpy(&aId, ptr, sizeof(aId));
        ptr += sizeof(aId);
        neuron->axons.push_back(aId);
    }
    for (int i = 0; i < dendriteCount; i++)
    {
        long dId;
        memcpy(&dId, ptr, sizeof(dId));
        ptr += sizeof(dId);
        neuron->dendrites.push_back(dId);

        // Populate dendriteMap if needed
        Dendrite *dendrite = globalObject->dendriteDB.getComponent(dId);
        if (dendrite)
        {
            neuron->dendriteMap.insert(std::make_pair(dendrite->getPreSynapticNeuronId(), dId));
        }
    }

    return neuron;
}

void Neuron::initializeRandom(void)
{
    // Implementation optional
}

// -------------------------------------------------------------
// Connection-related
// -------------------------------------------------------------

void Neuron::connect(Neuron *preSynapticNeuron, Neuron *postSynapticNeuron, float polarity)
{
    if(preSynapticNeuron->isConnectedTo(postSynapticNeuron))
        return;

    Dendrite *dendrite = Dendrite::create(postSynapticNeuron, preSynapticNeuron,polarity);
    float thisDistance = computeDistance(preSynapticNeuron->location, postSynapticNeuron->location);
    dendrite->setDistance(thisDistance);

    preSynapticNeuron->dendriteMap.insert(std::make_pair(postSynapticNeuron->id, dendrite->id));

    size_t axonSize = preSynapticNeuron->axons.size();
    for (size_t axonsIndex = 0; axonsIndex < axonSize; axonsIndex++)
    {
        Axon *thisAxon = globalObject->axonDB.getComponent(preSynapticNeuron->axons[axonsIndex]);
        long synapseId = dendrite->getSynapseId();
        thisAxon->insertSynapse(synapseId);
        thisAxon->setDistance(thisDistance);

        Synapse *s = globalObject->synapseDB.getComponent(synapseId);
        s->setPosition(thisDistance);
    }
}

void Neuron::connectTo(Neuron *preSynapticNeuron, float polarity)
{
    // This neuron is postSynaptic, the argument is preSynaptic
    connect(preSynapticNeuron, this, polarity);
}

void Neuron::connectFrom(Neuron *postSynapticNeuron, float polarity)
{
    // This neuron is preSynaptic, the argument is postSynaptic
    connect(this, postSynapticNeuron, polarity);
}

void Neuron::receiveInputFrom(Neuron *targetNeuron, float polarity)
{
    // Another name for connect
    connect(targetNeuron, this, polarity);
}

bool Neuron::isConnectedTo(Neuron *neuron)
{
    Dendrite *dendrite = globalObject->findConnectingDendrite(this, neuron);
    return (dendrite != nullptr);
}

long Neuron::getCurrentTimestep(void)
{
    return globalObject->getCurrentTimestamp();
}

// For demonstration
long Neuron::getNucleusId(void)
{
    // Example logic
    // This depends on your existing data structures
    return 0; 
}

// -------------------------------------------------------------
// Firing logic with refractory period & adaptive threshold
// -------------------------------------------------------------
Range Neuron::fire(void)
{
    Range range;
    range.low  = 1;
    range.high = 1;

    long now = globalObject->getCurrentTimestamp();

    // 1) Check refractory period
    if ((now - lastfired) < REFACTORY_PERIOD) {
        // Still in refractory period; skip
        return range;
    }

    // 2) Check threshold
    if (membranePotential >= threshold)
    {
        // Actually fire
        lastfired = now;
        setFiring(true);
        recordSpike();

        // Increase threshold after spiking (adaptive)
        threshold += THRESHOLD_INCREMENT;
        threshold = std::clamp(threshold, MIN_THRESHOLD, MAX_THRESHOLD);

        // Log or do any output logic
        globalObject->lastFiredNeuron = this;

        // Fire axons
        size_t aSize = axons.size();
        long lowestOffset = 9999999;
        long highestOffset= -9999999;
        for (size_t i = 0; i < aSize; i++)
        {
            Axon *a = globalObject->axonDB.getComponent(axons[i]);
            Range offset = a->fire();
            if(lowestOffset > offset.low) 
                lowestOffset = offset.low;
            if(highestOffset < offset.high)
                highestOffset = offset.high;
        }
        range.low  = lowestOffset;
        range.high = highestOffset;

        // Reset membrane potential
        membranePotential = RESTING_POTENTIAL;

        // Optionally, apply local STDP to all incoming synapses
        applySTDPToIncomingSynapses();
    }
    else
    {
        // If we didn't fire, threshold decays back down slightly
        threshold -= THRESHOLD_DECAY;
        threshold = std::clamp(threshold, MIN_THRESHOLD, MAX_THRESHOLD);
    }

    return range;
}

// -------------------------------------------------------------
// Membrane potential management
// -------------------------------------------------------------
void Neuron::setMembranePotential(float inPotential)
{
    float newPotential = std::clamp(inPotential, MINIMUM_MEMBRANE_POTENTIAL, MAXIMUM_MEMBRANE_POTENTIAL);
    if (fabs(newPotential - membranePotential) > 1e-9)
    {
        membranePotential = newPotential;
        setDirty(true);
    }
}

void Neuron::leakMembranePotential()
{
    membranePotential = RESTING_POTENTIAL
        + (membranePotential - RESTING_POTENTIAL) * LEAK_FACTOR;
}

// -------------------------------------------------------------
// Integrate synaptic input (called from Synapse::receiveAP)
// -------------------------------------------------------------
void Neuron::integrateSynapticInput(float input, long synapseId)
{
    // Basic example: just add to current potential
    // Optionally incorporate a leak or weighting factor
    float newPotential = membranePotential + std::max(input, 0.0f);

    setMembranePotential(newPotential);
    // Decide if we want to spontaneously fire here or not.
    // Typically, we let the main update loop or a "step()" function call fire().
    // But you can do an immediate check if you prefer a purely event-based approach.
    // fire(); // If you want to check threshold immediately after each input
}

void Neuron::setFiring(bool value)
{
    if (firing != value)
    {
        // Optionally log
        if (globalObject->setLogFiring)
        {
            globalObject->logFiring(this, value);
        }
        firing = value;
        setDirty(true);
        if (value) {
            latch = true;
        }
    }
}

void Neuron::applySTDP(std::pair<std::vector<Neuron *> *, std::vector<Neuron *> *> *neurons, long learningInterval)
{
    std::vector<Neuron *> *preNeurons = neurons->first;
    std::vector<Neuron *> *postNeurons = neurons->second;

    // Use defined constants A_PLUS, A_MINUS, TAU_PLUS, TAU_MINUS
    double A_plus = A_PLUS;  
    double A_minus = A_MINUS; 
    double tau_plus = TAU_PLUS;             
    double tau_minus = TAU_MINUS;            

    auto adjustSynWeightLocal = [&](Synapse* synapse, double deltaT, double A, double tau, float polarity) {
        float weightChange = A * std::exp(-deltaT / tau) * polarity;
        float newWeight = synapse->getWeight();
        if(!std::isnan(weightChange) && !std::isinf(weightChange))
        {
            newWeight += weightChange;
        }
        float baseline = 0.01f;  
        newWeight = std::clamp(newWeight + baseline, MINIMUM_SYNAPSE_WEIGHT, MAXIMUM_SYNAPSE_WEIGHT);
        synapse->setWeight(newWeight);
    };

    // Long-Term Potentiation (LTP)
    for (Neuron* preNeuron : *preNeurons) {
        if (preNeuron->id != this->id) {
            Dendrite* preDendrite = globalObject->findConnectingDendrite(this, preNeuron);
            if (preDendrite) {
                Synapse* synapse = globalObject->synapseDB.getComponent(preDendrite->getSynapseId());
                long deltaT = this->lastfired - preNeuron->lastfired;
                if (deltaT > 0) { 
                    adjustSynWeightLocal(synapse, deltaT, A_plus, tau_plus, synapse->polarity);
                }
            }
        }
    }

    // Long-Term Depression (LTD)
    for (Neuron* postNeuron : *postNeurons) {
        if (postNeuron->id != this->id) {
            Dendrite* postDendrite = globalObject->findConnectingDendrite(this, postNeuron);
            if (postDendrite) {
                Synapse* synapse = globalObject->synapseDB.getComponent(postDendrite->getSynapseId());
                long deltaT = postNeuron->lastfired - this->lastfired;
                if (deltaT > 0) {
                    adjustSynWeightLocal(synapse, deltaT, A_minus, tau_minus, -synapse->polarity);
                }
            }
        }
    }

    // After STDP, apply synaptic scaling
    applySynapticScaling(SYNAPTIC_SCALING_FACTOR);

}


// -------------------------------------------------------------
// Simple STDP using local synapse trace or time-differences
// -------------------------------------------------------------
void Neuron::applySTDPToIncomingSynapses()
{
    // Example approach: for each dendrite/synapse, see if presyn fired recently.
    for (auto &dId : dendrites)
    {
        Dendrite *d = globalObject->dendriteDB.getComponent(dId);
        if(!d) continue;

        Synapse *s = globalObject->synapseDB.getComponent(d->getSynapseId());
        if(!s) continue;

        Neuron *preNeuron = globalObject->neuronDB.getComponent(d->getPreSynapticNeuronId());
        if(!preNeuron) continue;

        long deltaT = (long)(this->lastfired - preNeuron->lastfired);

        // If deltaT > 0 => presyn fired before postsyn => LTP
        // If deltaT < 0 => postsyn fired first => LTD

        // This is your existing formula or logic:
        if (deltaT > 0)
        {
            float weightChange = A_PLUS * std::exp(-std::fabs(deltaT) / TAU_PLUS) * s->getPolarity();
            float newW = s->getWeight() + weightChange + 0.01f; // example baseline
            newW = std::clamp(newW, MINIMUM_SYNAPSE_WEIGHT, MAXIMUM_SYNAPSE_WEIGHT);
            s->setWeight(newW);
        }
        else if (deltaT < 0)
        {
            float weightChange = A_MINUS * std::exp(-std::fabs(deltaT) / TAU_MINUS) * -s->getPolarity();
            float newW = s->getWeight() + weightChange;
            newW = std::clamp(newW, MINIMUM_SYNAPSE_WEIGHT, MAXIMUM_SYNAPSE_WEIGHT);
            s->setWeight(newW);
        }

        // Optionally incorporate synapse->trace:
        // e.g., newW += s->trace * LTP_factor; s->trace = 0.0f; ...
        s->decayTrace(); // if you want to continuously decay the trace
    }

    // Possibly apply synaptic scaling if needed
    applySynapticScaling(SYNAPTIC_SCALING_FACTOR);
}

void Neuron::applySynapticScaling(float scalingFactor)
{
    if (std::fabs(scalingFactor - 1.0f) < 1e-9f) {
        return; // no scaling needed
    }
    for (auto &dId : dendrites)
    {
        Dendrite *d = globalObject->dendriteDB.getComponent(dId);
        if (!d) continue;

        Synapse *s = globalObject->synapseDB.getComponent(d->getSynapseId());
        if (!s) continue;

        float newW = s->getWeight() * scalingFactor;
        newW = std::clamp(newW, MINIMUM_SYNAPSE_WEIGHT, MAXIMUM_SYNAPSE_WEIGHT);
        s->setWeight(newW);
    }
}

// -------------------------------------------------------------
// Rate-based homeostasis
// -------------------------------------------------------------
float Neuron::getEstimatedFiringRate()
{
    unsigned long now = globalObject->getCurrentTimestamp();
    float elapsed = (float)(now - lastUpdateStep + 1);
    float rate = recentSpikeCount / elapsed;

    // Decay recent spike count over time
    recentSpikeCount *= 0.9f;
    lastUpdateStep = now;

    return rate;
}

void Neuron::recordSpike()
{
    recentSpikeCount += 1.0f;
}

// -------------------------------------------------------------
// Misc. checks
// -------------------------------------------------------------
bool Neuron::isContainedIn(std::vector<long> neurons, Neuron *thisNeuron)
{
    for (auto &nid : neurons)
    {
        if (thisNeuron->id == nid) return true;
    }
    return false;
}

std::string Neuron::generateOutputFiringString(std::vector<long> neurons, Neuron *thisNeuron)
{
    std::string outString;
    for (auto &nid : neurons)
    {
        Neuron *n = globalObject->neuronDB.getComponent(nid);
        if (!n) {
            outString += "?";
            continue;
        }
        if (n->id == thisNeuron->id) {
            outString += "1";
        } else {
            outString += (n->isFiring() ? "X" : "0");
        }
    }
    return outString;
}

bool Neuron::isFiring(float delay) 
{
    if (delay == 0) {
        return firing;
    }
    else
    {
        long timediff = globalObject->getCurrentTimestamp() - lastfired;
        long diff = std::abs(timediff);
        if (delay > diff)
            return true;
        else
            return false;
    }
}


bool Neuron::isFromSensoryNucleus()
{
    // Example check
    return (this->nucleusType == /*SENSORY_NUCLEUS*/ 1);
}

bool Neuron::isAssociated(long synapseId)
{
    // See if synapseId belongs to one of this neuron's dendrites
    for (auto &dId : dendrites)
    {
        Dendrite *dend = globalObject->dendriteDB.getComponent(dId);
        if (!dend) continue;
        if (synapseId == dend->getSynapseId()) {
            return true;
        }
    }
    return false;
}

std::vector<long>* Neuron::getAxonConnectedSynapses(void)
{
    std::vector<long> *connectedSynapses = new std::vector<long>();
    for (auto &axId : axons)
    {
        Axon *a = globalObject->axonDB.getComponent(axId);
        if (!a) continue;
        std::vector<long> *synList = a->getSynapses();
        connectedSynapses->insert(connectedSynapses->end(), synList->begin(), synList->end());
    }
    return connectedSynapses;
}

float Neuron::computeDistance(Location3D pointA, Location3D pointB)
{
    return std::sqrt(
        std::pow(pointB.x - pointA.x, 2) +
        std::pow(pointB.y - pointA.y, 2) +
        std::pow(pointB.z - pointA.z, 2));
}

float Neuron::nextDistance(void)
{
    // If you want to increment distance each time you add a dendrite
    static float distanceValue = 10.0f;
    distanceValue += 5.0f; // example step
    return distanceValue;
}

std::string Neuron::getLocationOfNeuron(void)
{
    // Example placeholder
    return "UnknownLocation";
}

// -------------------------------------------------------------
// Example STDP helper
// -------------------------------------------------------------
void Neuron::applySynapticWeightDelta(Synapse *synapse, float deltaT, float A, float tau, float polarity)
{
    float weightChange = A * std::exp(-std::fabs(deltaT) / tau) * polarity;
    float newW = synapse->getWeight() + weightChange;
    newW = std::clamp(newW, MINIMUM_SYNAPSE_WEIGHT, MAXIMUM_SYNAPSE_WEIGHT);
    synapse->setWeight(newW);
}
