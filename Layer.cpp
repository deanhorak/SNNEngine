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

#include "Layer.h"
#include "TR1Random.h"
#include "Global.h"

Layer::Layer(unsigned long parentId):
	NNComponent(ComponentTypeLayer)
{
}

Layer::~Layer(void)
{
}

/*
	std::vector<long> clusters;

	Location3D location;
	Size3D area;
*/
void Layer::toJSON(std::ofstream& outstream)
{
	std::stringstream ss;
	LOGSTREAM(ss) << "Exporting layer..." << std::endl;
	globalObject->log(ss);

	std::string sep("");
	outstream << "                { \"_type\": \"Layer\", \"id\": " << id << ", \"location\": [" << location.x << ", " << location.y << ", " << location.z << "], \"size\": [" << area.h << ", " << area.w << ", " << area.d << "], \"clusters\": [ " << std::endl;
	for (unsigned int i = 0; i < clusters.size(); i++)
	{
		outstream << sep;
		sep = ",";
		Cluster* c = globalObject->clusterDB.getComponent(clusters[i]);
		c->toJSON(outstream);
	}
	outstream << "                ] } " << std::endl;

}

void Layer::save(void)
{
	globalObject->layerDB.save(this);
}

void Layer::commit(void)
{
	globalObject->layerDB.addToCache(this);
}


Layer *Layer::create(unsigned long parentId)
{
	Layer *l = new Layer(parentId);
	l->id = globalObject->nextComponent(ComponentTypeLayer);
	globalObject->insert(l);
	return l;
}


void Layer::projectTo(Layer *layer, float sparsity)
{
	std::stringstream ss;
//	std::cout << " Connecting layer " << id << " ... " << std::endl;
	size_t cSize = clusters.size();
	size_t cSize2 = layer->clusters.size();
	for(size_t i=0;i<cSize;i++)
	{
		Cluster *cFrom = globalObject->clusterDB.getComponent(clusters[i]);
		for(size_t j=0;j<cSize2;j++)
		{
			Cluster *cTo = globalObject->clusterDB.getComponent(layer->clusters[j]);
//			LOGSTREAM(ss) << "      Projecting cluster " << cFrom->id << " (" << cFrom->neurons.size() << " neurons) to  " << cTo->id << " (" << cTo->neurons.size() << " neurons)" << std::endl;
//			globalObject->log(ss);
			cFrom->projectTo(cTo, sparsity);
		}
	}
}

void Layer::cycle(void)
{
	size_t cSize = clusters.size();
	for(size_t i=0;i<cSize;i++)
	{
		Cluster  *cl = globalObject->clusterDB.getComponent(clusters[i]);
		cl->cycle();
	}
/*
	CollectionIterator<Cluster *> itCluster(&clusters);
	for (itCluster.begin(); itCluster.more(); itCluster.next())
	{
		itCluster.value()->cycle();
	}
*/
//	std::cout << "Layer " << this->id << " cycled." << std::endl;
}

void Layer::initializeRandom(unsigned long parentId)
{
	
	size_t rnd = (size_t) tr1random->generate(1,10); // Random # of Clusters
	for(size_t i=0;i<rnd;i++) 
	{
		SpatialDetails sd(this->location, this->area);
		sd.randomizeLocation();

		Cluster *c = Cluster::create(sd,parentId);
		c->initializeRandom();
		clusters.push_back(c->id);
		this->commit();
	}
	
}

Layer *Layer::instantiate(long key, size_t len, void *data)
{
// 	size_t size = sizeof(float)+sizeof(float)+sizeof(long)+sizeof(long);
	long clusterCount = 0;
	Layer *layer = new Layer(0);
	layer->id = key;

	char *ptr = (char*)data;

	memcpy(&layer->location.x, ptr, sizeof(location.x)); 	ptr += sizeof(location.x);
	memcpy(&layer->location.y, ptr, sizeof(location.y));	ptr += sizeof(location.y);
	memcpy(&layer->location.z, ptr, sizeof(location.z));	ptr += sizeof(location.z);
	memcpy(&layer->area.h, ptr, sizeof(area.h)); 	ptr += sizeof(area.h);
	memcpy(&layer->area.w, ptr, sizeof(area.w)); 	ptr += sizeof(area.w);
	memcpy(&layer->area.d, ptr, sizeof(area.d)); 	ptr += sizeof(area.d);
	memcpy(&clusterCount,ptr,sizeof(clusterCount)); 	ptr+=sizeof(clusterCount);

	for(size_t i=0;i<clusterCount;i++)
	{
		long cid = 0;
		memcpy(&cid,ptr,sizeof(cid));
		layer->clusters.push_back(cid);
		ptr+=sizeof(cid);
	}
//	printf("instantiate: Layer %i clusterSize %i\n",(int)layer->id,(int)layer->clusters.size());
	return layer;
}

Tuple *Layer::getImage(void)
{
/* -- persisted values
	u_int32_t clusterCount;
	std::vector<long> clusters;
*/
	long clusterCount = clusters.size();
	size_t size = sizeof(location.x)+ sizeof(location.y) + sizeof(location.z) + sizeof(area.h) + sizeof(area.w) + sizeof(area.d) + sizeof(clusterCount) +(clusterCount * sizeof(long));

	char *image = globalObject->allocClearedMemory(size);
	char *ptr = (char*)image;

	memcpy(ptr, &location.x, sizeof(location.x)); 	ptr += sizeof(location.x);
	memcpy(ptr, &location.y, sizeof(location.y)); 	ptr += sizeof(location.y);
	memcpy(ptr, &location.z, sizeof(location.z)); 	ptr += sizeof(location.z);
	memcpy(ptr, &area.h, sizeof(area.h)); 	ptr += sizeof(area.h);
	memcpy(ptr, &area.w, sizeof(area.w)); 	ptr += sizeof(area.w);
	memcpy(ptr, &area.d, sizeof(area.d)); 	ptr += sizeof(area.d);
	memcpy(ptr,&clusterCount,sizeof(clusterCount)); 	ptr+=sizeof(clusterCount);

	for(size_t i=0;i<clusterCount;i++)
	{
		long k = clusters[i];
		memcpy(ptr,&k,sizeof(k));
		ptr+=sizeof(k);
	}

	Tuple* tuple = new Tuple();
	tuple->objectPtr = image;
	tuple->value = size;

	return tuple;
}


