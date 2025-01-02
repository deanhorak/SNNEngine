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

#include "Dendrite.h"
#include "Global.h"
#include "Neuron.h"
#include "Synapse.h"
#include "Axon.h"
#include "TimedEvent.h"
#include <cmath>

Dendrite::Dendrite(Neuron *neuron, long newId, float polarity):
    Process(ComponentTypeDendrite)
{
    setPreSynapticNeuronId(neuron->id);
    setRate(DEFAULT_DENDRITE_RATE,false);
    setDistance(DEFAULT_DENDRITE_DISTANCE);

    Synapse *s = Synapse::create(this,polarity);
    this->synapseId = s->id;
    s->setOwningDendriteId(newId);
    s->setPosition(this->getDistance());
    neuron->setDirty();
}

Dendrite::~Dendrite(void)
{
}

void Dendrite::toJSON(std::ofstream& outstream)
{
    outstream << "                            { \"_type\": \"Dendrite\", \"id\": " << id << ", \"synapseId\": " << synapseId << " } " << std::endl;
}

void Dendrite::save(void)
{
    globalObject->dendriteDB.save(this);
}

void Dendrite::commit(void)
{
    globalObject->dendriteDB.addToCache(this);
}

float computeDistance(float x1, float y1, float z1, float x2, float y2, float z2) {
    return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2) + std::pow(z2 - z1, 2));
}

Dendrite *Dendrite::create(Neuron *postSynapticNeuron, Neuron *preSynapticNeuron, float polarity)
{
    long newId = globalObject->nextComponent(ComponentTypeDendrite);
    Dendrite *d = new Dendrite(preSynapticNeuron, newId,polarity);
    d->id = newId;
    d->setPostSynapticNeuronId(postSynapticNeuron->id);
    d->setPreSynapticNeuronId(preSynapticNeuron->id);

    long synapseId = d->synapseId;
    Synapse *synapse = globalObject->synapseDB.getComponent(synapseId);
    float calculatedDistance = computeDistance(preSynapticNeuron->location.x,preSynapticNeuron->location.y,preSynapticNeuron->location.z,
                                               postSynapticNeuron->location.x,postSynapticNeuron->location.y,postSynapticNeuron->location.z);
    synapse->setPosition(calculatedDistance);

    globalObject->insert(d);
    postSynapticNeuron->getDendrites()->push_back(d->id);

    return d;
}

void Dendrite::fire(void)
{
    float lclDistance = getDistance();
    float lclRate = getRate();

    Synapse *s = globalObject->synapseDB.getComponent(this->synapseId);
    long offset = ActionPotential::computeOffset(lclDistance, lclRate);
    if(offset > 0)
    {
        // Here if we wanted to do something else, we could. Currently, offset scheduling is done via Axon firing events.
        // The dendrite itself might not need to directly schedule events; Axon and Synapse handle that.
        // This fire method is currently unused or can be left as-is.
    }
}

Dendrite::Dendrite(void): 
    Process(ComponentTypeDendrite)
{
    setRate(DEFAULT_DENDRITE_RATE);
    setDistance(10.0);
}

Tuple *Dendrite::getImage(void)
{
    float lclDistance = getDistance();
    float lclRate = getRate();

    size_t size = sizeof(parentId) + sizeof(lclDistance) + sizeof(lclRate) + sizeof(postSynapticNeuronId) + sizeof(preSynapticNeuronId) + sizeof(synapseId);

    char *image = globalObject->allocClearedMemory(size);
    char *ptr = (char*)image;

    memcpy(ptr, &parentId, sizeof(parentId));       ptr += sizeof(parentId);
    memcpy(ptr,&lclDistance,sizeof(lclDistance));   ptr+=sizeof(lclDistance);
    memcpy(ptr,&lclRate,sizeof(lclRate));           ptr+=sizeof(lclRate);
    memcpy(ptr,&postSynapticNeuronId,sizeof(postSynapticNeuronId));  ptr+=sizeof(postSynapticNeuronId);
    memcpy(ptr,&preSynapticNeuronId,sizeof(preSynapticNeuronId));    ptr+=sizeof(preSynapticNeuronId);
    memcpy(ptr,&synapseId,sizeof(synapseId));       ptr+=sizeof(synapseId);

    Tuple* tuple = new Tuple();
    tuple->objectPtr = image;
    tuple->value = size;

    return tuple;
}

Dendrite *Dendrite::instantiate(long key, size_t len, void *data)
{
    (void)len;
    Dendrite *dendrite = new Dendrite();
    dendrite->id = key;

    char *ptr = (char*)data;

    float lclDistance = 0;
    float lclRate = 0;

    memcpy(&dendrite->parentId, ptr, sizeof(dendrite->parentId));    ptr += sizeof(dendrite->parentId); 
    memcpy(&lclDistance, ptr, sizeof(lclDistance));                  ptr += sizeof(lclDistance); dendrite->setDistance(lclDistance);
    memcpy(&lclRate, ptr, sizeof(lclRate));                          ptr += sizeof(lclRate); dendrite->setRate(lclRate);
    memcpy(&dendrite->postSynapticNeuronId,ptr,sizeof(dendrite->postSynapticNeuronId)); ptr+=sizeof(dendrite->postSynapticNeuronId);  
    memcpy(&dendrite->preSynapticNeuronId,ptr,sizeof(dendrite->preSynapticNeuronId));   ptr+=sizeof(dendrite->preSynapticNeuronId);  
    memcpy(&dendrite->synapseId,ptr,sizeof(dendrite->synapseId));    ptr+=sizeof(dendrite->synapseId); 

    return dendrite;
}
