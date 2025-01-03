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

#include "Nucleus.h"
#include "TR1Random.h"
#include "Global.h"

Nucleus::Nucleus(int nucleusType):
	NNComponent(ComponentTypeNucleus)
{
	//this->id = globalObject->nextComponent(ComponentTypeNucleus);
	this->nucleusType = nucleusType;
}

Nucleus::~Nucleus(void)
{
}
/*
std::string name;

	std::vector<long> columns;

	Location3D location;
	Size3D area;

	int nucleusType;
*/
void Nucleus::toJSON(std::ofstream& outstream)
{
	std::stringstream ss;
	LOGSTREAM(ss) << "Exporting nucleus..." << std::endl;
	globalObject->log(ss);

	std::string sep("");
	outstream << "        {  \"_type\": \"Nucleus\", \"_name\": \"" << name << "\", \"id\": " << id << ",  \"NucleusType\": " << nucleusType << ",  \"location\": [" << location.x << ", " << location.y << ", " << location.z << "], \"size\": [" << area.h << ", " << area.w << ", " << area.d << "], \"columns\": [" << std::endl;
	for (unsigned int i = 0; i < columns.size(); i++)
	{
		outstream << sep;
		sep = ",";
		Column* c = globalObject->columnDB.getComponent(columns[i]);
		c->toJSON(outstream);
	}
	outstream << "        ] } " << std::endl;

}


void Nucleus::save(void)
{
	globalObject->nucleusDB.save(this);
}

void Nucleus::commit(void)
{
	globalObject->nucleusDB.addToCache(this);
}



Nucleus *Nucleus::create(std::string name, SpatialDetails details, int nucleusType, bool setToDirty)
{
	Nucleus *n = new Nucleus(nucleusType);
	n->name = name;

	n->location = details.location;
	n->area = details.area;
	n->parentId=0;
	n->id = globalObject->nextComponent(ComponentTypeNucleus);

	globalObject->insert(n);
	if(setToDirty)
		n->setDirty();

	std::stringstream ss;
	LOGSTREAM(ss) << "  Nucleus " << n->name << " (" << n->id << ") created." << std::endl;
	globalObject->log(ss);
	return n;
}


void Nucleus::receiveInputFrom(Nucleus *nucleus, float sparsity, float polarity)
{
	size_t cSize1 = columns.size();
	size_t cSize2 = nucleus->columns.size();
	//size_t cMax = cSize1 * cSize2;
	//size_t cNumber = 0;
	std::stringstream ss;
	for(size_t i=0;i<cSize1;i++)
	{
		long c1id = columns[i];

		Column *col1 = globalObject->columnDB.getComponent(c1id);
		for(size_t j=0;j<cSize2;j++)
		{
			long c2id = nucleus->columns[j];

			Column *col2 = NULL;
			if(c1id == c2id) 
				col2 = col1;
			else
				col2 = globalObject->columnDB.getComponent(c2id);
//			size_t pct = (cNumber*100) / cMax;
//			LOGSTREAM(ss) << "      Column " << col1->id << " receiving input from " << col2->id << "... (" << cNumber++ << " of " << cMax << " - " << pct << "%) " << std::endl;
//			globalObject->log(ss);

			col1->receiveInputFrom(col2,sparsity,polarity);
		}
	}
}

long Nucleus::getStartNeuron(void)
{
	size_t cSize = columns.size();
	if(cSize==0)
		return 0;

	long cId = columns[0];
	Column *col = globalObject->columnDB.getComponent(cId);
	return col->getStartNeuron();
}

long Nucleus::getEndNeuron(void)
{
	size_t cSize = columns.size();
	if(cSize==0)
		return 0;

	long cId = columns[cSize-1];
	Column *col = globalObject->columnDB.getComponent(cId);
	return col->getEndNeuron();
}

Nucleus *Nucleus::instantiate(long key, size_t len, void *data)
{

	long columnCount = 0;
	Nucleus *nucleus = new Nucleus(0);
	nucleus->id = key;

	char *ptr = (char*)data;

	memcpy(&nucleus->parentId, ptr, sizeof(nucleus->parentId)); 	ptr += sizeof(nucleus->parentId);
	memcpy(&nucleus->location.x, ptr, sizeof(nucleus->location.x)); 	ptr += sizeof(nucleus->location.x);
	memcpy(&nucleus->location.y, ptr, sizeof(nucleus->location.y)); 	ptr += sizeof(nucleus->location.y);
	memcpy(&nucleus->location.z, ptr, sizeof(nucleus->location.z)); 	ptr += sizeof(nucleus->location.z);
	memcpy(&nucleus->area.h, ptr, sizeof(area.h)); 	ptr += sizeof(area.h);
	memcpy(&nucleus->area.w, ptr, sizeof(area.w)); 	ptr += sizeof(area.w);
	memcpy(&nucleus->area.d, ptr, sizeof(area.d)); 	ptr += sizeof(area.d);
	memcpy(&columnCount,ptr,sizeof(columnCount)); 	ptr+=sizeof(columnCount);

	for(size_t i=0;i<(size_t)columnCount;i++)
	{
		long cid = 0;
		memcpy(&cid,ptr,sizeof(cid));
		nucleus->columns.push_back(cid);
		ptr+=sizeof(cid);
	}

	char* end = (char*)data + len;
	size_t sz = (end - ptr) + 1; // compute size of string +1
	char* buff = (char*)globalObject->allocClearedMemory(sz);
	strncpy(buff, ptr, sz);
	std::string temp(buff);
	nucleus->name = temp;

//	printf("instantiate: Nucleus %i columnssize %i\n",(int)nucleus->id,(int)nucleus->columns.size());

	return nucleus;
}

Tuple *Nucleus::getImage(void)
{
/* -- persisted values
	u_int32_t columnCount;
	std::vector<long> columns;
*/
//	printf("GetImage: Nucleus %i columnssize %i\n",(int)this->id,(int)columns.size());

	long columnCount = columns.size();
	size_t namesize = name.length()+1;
	size_t size = sizeof(parentId) + sizeof(location.x) + sizeof(location.y) + sizeof(location.z) +
		sizeof(area.h) + sizeof(area.w) + sizeof(area.d) + sizeof(columnCount) + (columnCount * sizeof(long)) + namesize;
	
		
	char *image = globalObject->allocClearedMemory(size);
	char *ptr = (char*)image;

	memcpy(ptr, &parentId, sizeof(parentId)); ptr += sizeof(parentId);
	memcpy(ptr, &location.x, sizeof(location.x)); 	ptr += sizeof(location.x);
	memcpy(ptr, &location.y, sizeof(location.y)); 	ptr += sizeof(location.y);
	memcpy(ptr, &location.z, sizeof(location.z)); 	ptr += sizeof(location.z);
	memcpy(ptr, &area.h, sizeof(area.h)); 	ptr += sizeof(area.h);
	memcpy(ptr, &area.w, sizeof(area.w)); 	ptr += sizeof(area.w);
	memcpy(ptr, &area.d, sizeof(area.d)); 	ptr += sizeof(area.d);
	memcpy(ptr, &columnCount,sizeof(columnCount)); 	ptr+=sizeof(columnCount);

	for(size_t i=0;i<(size_t)columnCount;i++)
	{
		long k = columns[i];
		memcpy(ptr,&k,sizeof(k));
		ptr+=sizeof(k);
	}


	strncpy(ptr,name.c_str(),namesize);
	ptr += (namesize + 1);

	Tuple* tuple = new Tuple();
	tuple->objectPtr = image;
	tuple->value = size;

	return tuple;
}

void Nucleus::addColumns(size_t colCount, size_t layerCount, size_t clusterCount, size_t neuronCount)
{
	for(size_t i=0;i<colCount;i++) 
	{
		SpatialDetails sdr(this->location, this->area);
		sdr.randomizeLocation();
		Column *c = Column::create(sdr,layerCount, this->id);
		size_t lSize = c->layers.size();
		for(size_t k1=0;k1<lSize;k1++)
		{
			Layer *layer = globalObject->layerDB.getComponent(c->layers[k1]);
			for(size_t i5=0;i5<clusterCount;i5++) 
			{

				Cluster *cl = Cluster::create(sdr,this->id);
				cl->parentId = this->id;
				float offset = (float) ((abs(area.h) / layerCount) / 2);
				for(size_t i6=0;i6<neuronCount;i6++) 
				{
					float newX = sdr.location.x + ((float)(tr1random->generate(1, (int)abs(area.w))));
					float newY = sdr.location.y+(offset*(float)k1);
					float newZ = sdr.location.z + ((float)(tr1random->generate(1, (int)abs(area.d))));
					Location3D loc2(newX,newY,newZ);
					SpatialDetails sdd(loc2,this->area);
					Neuron *neur = Neuron::create(sdd, Pyramidal,cl->id, this->nucleusType);
					cl->getNeurons().push_back(neur->id);
					cl->setDirty();

//					std::cout << "Neuron (" << sdd.location.x << "," << sdd.location.y << "," << sdd.location.z << "), ";
//					std::cout << this->name << " (" << this->location.x << "," << this->location.y << "," << this->location.z << ") " << std::endl;
				}
				layer->clusters.push_back(cl->id);
				layer->setDirty();
			}
		}
		columns.push_back(c->id);
	}
	setDirty(true);
}
