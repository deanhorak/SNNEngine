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
 * without express written permission from Your Name.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * If you have any questions about this license, please contact Your Name at dean.horak@gmail.com.
 */

#include "Cluster.h"
#include "TR1Random.h"
#include "Global.h"

Cluster::Cluster(unsigned long parentId):
	NNComponent(ComponentTypeCluster)
{
	this->parentId = parentId;

}

Cluster::~Cluster(void)
{
}

void Cluster::toJSON(std::ofstream& outstream)
{
	std::stringstream ss;
	LOGSTREAM(ss) << "Exporting cluster..." << std::endl;
	globalObject->log(ss);

	std::string sep("");
	outstream << "                    { \"_type\": \"Cluster\", \"id\": " << id << ", \"location\": [" << location.x << ", " << location.y << ", " << location.z << "], \"size\": [" << area.h << ", " << area.w << ", " << area.d << "], \"neurons\": [ " << std::endl;
	for (unsigned int i = 0; i < neurons.size(); i++)
	{
		outstream << sep;
		sep = ",";
		Neuron* n = globalObject->neuronDB.getComponent(neurons[i]);
		n->toJSON(outstream);
	}
	outstream << "                    ] } " << std::endl;

}


void Cluster::save(void)
{
	globalObject->clusterDB.save(this);
}

void Cluster::commit(void)
{
	globalObject->clusterDB.addToCache(this);
}


Cluster *Cluster::create(SpatialDetails details, unsigned long parentId)
{
	Cluster *c = new Cluster(parentId);
	c->id = globalObject->nextComponent(ComponentTypeCluster);

	c->location = details.location;
	c->area = details.area;

	globalObject->insert(c);
	return c;
}


void Cluster::projectTo(Cluster *cluster, float sparsity)
{
	Cluster *clusterA = this;
	Cluster *clusterB = cluster;

	size_t aSize = clusterA->neurons.size();
	size_t bSize = clusterB->neurons.size();

	size_t aCount = (size_t)((sparsity * (float)aSize)/100.f);
	size_t bCount = (size_t)((sparsity * (float)bSize)/100.f);


	for(size_t i=0;i<aCount;i++)
	{
		Neuron *neuronA = globalObject->neuronDB.getComponent(clusterA->neurons[i]);
		for(size_t j=0;j<bCount;j++)
		{
			Neuron *neuronB = globalObject->neuronDB.getComponent(clusterB->neurons[j]);
			neuronA->projectTo(neuronB);
		}
	}
}

void Cluster::cycle(void)
{
	size_t size = neurons.size();
	for(size_t i=0;i<size;i++)
	{
		Neuron *neuron = globalObject->neuronDB.getComponent(neurons[i]);
		neuron->cycle();
	}

}

void Cluster::initializeRandom(void)
{
	
	size_t rnd = (size_t) tr1random->generate(1,10); // Random # of Neurons
	for(size_t i=0;i<rnd;i++) 
	{
		SpatialDetails sd(this->location, this->area);
		sd.randomizeLocation();

		Neuron *n = Neuron::create(sd,Pyramidal,this->id,0);
		n->initializeRandom();
		neurons.push_back(n->id);
	}
	
}

Cluster *Cluster::instantiate(long key, size_t len, void *data)
{

	long neuronCount = 0;
	long nid = 0;

	Cluster *cluster = new Cluster(0);
	cluster->id = key;

	char *ptr = (char*)data;

	memcpy(&cluster->parentId, ptr, sizeof(cluster->parentId));	ptr += sizeof(cluster->parentId);		
	memcpy(&cluster->location.x, ptr, sizeof(location.x)); 	ptr += sizeof(location.x);
	memcpy(&cluster->location.y, ptr, sizeof(location.y)); 	ptr += sizeof(location.y);
	memcpy(&cluster->location.z, ptr, sizeof(location.z)); 	ptr += sizeof(location.z);
	memcpy(&cluster->area.h, ptr, sizeof(area.h)); 			ptr += sizeof(area.h);
	memcpy(&cluster->area.w, ptr, sizeof(area.w)); 			ptr += sizeof(area.w);
	memcpy(&cluster->area.d, ptr, sizeof(area.d)); 			ptr += sizeof(area.d);
	memcpy(&neuronCount,ptr,sizeof(neuronCount));				ptr+=sizeof(neuronCount);

	for(size_t i=0;i<neuronCount;i++)
	{
		memcpy(&nid,ptr,sizeof(nid));
		cluster->getNeurons().push_back(nid);
		ptr+=sizeof(nid);
	}
	return cluster;
}

Tuple *Cluster::getImage(void)
{
	long neuronCount = neurons.size();
	size_t size = sizeof(parentId)+ sizeof(location.x)+ sizeof(location.y) + sizeof(location.z) + sizeof(area.h) + sizeof(area.w) + sizeof(area.d) + sizeof(neuronCount) +(neuronCount *sizeof(long));

	char *image = globalObject->allocClearedMemory(size);
	char *ptr = (char*)image;

	memcpy(ptr, &parentId, sizeof(parentId)); 	ptr += sizeof(parentId);
	memcpy(ptr, &location.x, sizeof(location.x)); 	ptr += sizeof(location.x);
	memcpy(ptr, &location.y, sizeof(location.y)); 	ptr += sizeof(location.y);
	memcpy(ptr, &location.z, sizeof(location.z)); 	ptr += sizeof(location.z);
	memcpy(ptr, &area.h, sizeof(area.h)); 	ptr += sizeof(area.h);
	memcpy(ptr, &area.w, sizeof(area.w)); 	ptr += sizeof(area.w);
	memcpy(ptr, &area.d, sizeof(area.d)); 	ptr += sizeof(area.d);
	memcpy(ptr,&neuronCount,sizeof(neuronCount)); 	ptr+=sizeof(neuronCount);

	for(size_t i=0;i<neuronCount;i++)
	{
		long k = neurons[i];
		memcpy(ptr,&k,sizeof(k));
		ptr+=sizeof(k);
	}

	Tuple* tuple = new Tuple();
	tuple->objectPtr = image;
	tuple->value = size;

	return tuple;

}

