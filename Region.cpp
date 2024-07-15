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

#include <time.h>

#include "Region.h"
#include "TR1Random.h"
#include "Global.h"

Region::Region(void):
	NNComponent(ComponentTypeRegion)
{
}
 
Region::~Region(void)
{
}
/*
	std::string name;

	std::vector<long> nuclei;

	Location3D location;
	Size3D area;

*/
void Region::toJSON(std::ofstream& outstream)
{
	std::stringstream ss;
	LOGSTREAM(ss) << "Exporting region..." << std::endl;
	globalObject->log(ss);

	std::string sep("");
	outstream << "    { \"_type\": \"Region\", \"_name\": \"" << name << "\", \"id\": " << id << ", \"location\": [" << location.x  << ", " << location.y << ", " << location.z << "], \"size\": [" << area.h << ", " << area.w << ", " << area.d << "],  \"nuclei\": [ " << std::endl;
	long nucsz = this->nuclei.size();
	for (unsigned int i = 0; i < nuclei.size(); i++)
	{
		outstream << sep;
		sep = ",";
		Nucleus* n = globalObject->nucleusDB.getComponent(nuclei[i]);
		n->toJSON(outstream);
	}
	outstream << "    ] } " << std::endl;

}



void Region::save(void)
{
	globalObject->regionDB.save(this);
}

void Region::commit(void)
{
	globalObject->regionDB.addToCache(this);
}


Region *Region::create(std::string name, SpatialDetails details, bool setToDirty)
{
	Region *reg = new Region();
	reg->name = name;
	reg->id = globalObject->nextComponent(ComponentTypeRegion);
	reg->location = details.location;
	reg->area= details.area;

	globalObject->insert(reg);
	if(setToDirty)
		reg->setDirty();

	std::stringstream ss;
	LOGSTREAM(ss) << "Region " << reg->name << " (" << reg->id << ") created." << std::endl;
	globalObject->log(ss);
	return reg;
}


void Region::projectTo(Region *region, float sparsity)
{
	std::stringstream ss;
	size_t nSize = nuclei.size();
	size_t nSize2 = region->nuclei.size();
	size_t nMax = nSize * nSize2;
	size_t nCount = 0;

	LOGSTREAM(ss) << "   From region contains  " << nSize << " nuclei, to region contains " << nSize2 << " nuclei .." << std::endl;
	globalObject->log(ss);

	for(size_t i=0;i<nSize;i++)
	{
		Nucleus *nuc1 = globalObject->nucleusDB.getComponent(nuclei[i]);
		for(size_t j=0;j<nSize2;j++)
		{
			Nucleus *nuc2 = globalObject->nucleusDB.getComponent(region->nuclei[j]);

			size_t pct = (nCount++)*100/nMax;
//			std::cout << "   Projecting Nucleus " << nuc1->id << " to " << nuc2->id << "..." << std::endl;
			LOGSTREAM(ss) << "   Connecting Nucleus " << nuc1->name << " [" << nuc1->id << "] to " << nuc2->name << " [" << nuc2->id << "] (" << nCount << " of " << nMax << " - " << pct << "%) .." << std::endl;
			globalObject->log(ss);

			nuc1->projectTo(nuc2,sparsity);
		}
	}
}
/*
void Region::removeDeadAPs(void)
{
	globalObject->removeDeadAPs();
}
*/

void Region::cycle(void)
{
	size_t nSize = nuclei.size();
	for(size_t i=0;i<nSize;i++)
	{
		Nucleus *nuc1 = globalObject->nucleusDB.getComponent(nuclei[i]);
		nuc1->cycle();
	}
/*
	CollectionIterator<Nucleus *> itNucleus(&nuclei);
	for (itNucleus.begin(); itNucleus.more(); itNucleus.next())
	{
		itNucleus.value()->cycle();
	}
*/
//	removeDeadAPs();
//	std::cout << "Region " << this->id << " cycled." << std::endl;

}

void Region::initializeRandom(void)
{
	size_t rnd = (size_t) tr1random->generate(1,2); // Random # of Nuclei

	SpatialDetails sd(this->location,this->area);

	for(size_t i=0;i<rnd;i++) 
	{
		SpatialDetails sdn(sd);
		sdn.randomizeLocation();

		Nucleus *n = Nucleus::create("dummy",sdn);
		n->initializeRandom();
		nuclei.push_back(n->id);
	}

}

Region *Region::instantiate(long key, size_t len, void *data)
{
// 	u_int32_t size = sizeof(float)+sizeof(float)+sizeof(long)+sizeof(long);


	std::stringstream ss;
	long nucleiCount = 0;

	Region *region = new Region();
	region->id = key;

	char *ptr = (char*)data;

	memcpy(&region->location.x, ptr, sizeof(location.x)); 	ptr += sizeof(location.x);
	memcpy(&region->location.y, ptr, sizeof(location.y)); 	ptr += sizeof(location.y);
	memcpy(&region->location.z, ptr, sizeof(location.z)); 	ptr += sizeof(location.z);
	memcpy(&region->area.h, ptr, sizeof(area.h)); 	ptr += sizeof(area.h);
	memcpy(&region->area.w, ptr, sizeof(area.w)); 	ptr += sizeof(area.w);
	memcpy(&region->area.d, ptr, sizeof(area.d)); 	ptr += sizeof(area.d);
	memcpy(&nucleiCount, ptr, sizeof(nucleiCount)); 	ptr +=sizeof(nucleiCount);

	for(size_t i=0;i<nucleiCount;i++)
	{
		long nid = 0;
		memcpy(&nid,ptr,sizeof(nid));
		region->nuclei.push_back(nid);
		ptr+=sizeof(nid);
	}

	char* end = (char *)data + len;
	size_t sz = (end - ptr)+1; // compute size of string +1
	char* buff = (char *)globalObject->allocClearedMemory(sz);
	strncpy(buff,ptr,sz);
	std::string temp(buff);
	region->name = temp;

//	LOGSTREAM(ss) << "Region instantiated with  " << nucleiCount << " nuclei " << std::endl;
//	globalObject->log(ss);

	return region;
}

Tuple *Region::getImage(void)
{
/* -- persisted values
	u_int32_t nucleiCount;
	std::vector<long> nuclei;
*/
//	printf("GetImage: Region %i nucleisize %i\n",(int)this->id,(int)nuclei.size());

	long nucleiCount = nuclei.size();
	size_t namesize = name.length()+1;
	size_t size = sizeof(location.x) + sizeof(location.y) + sizeof(location.z) + sizeof(area.h) + sizeof(area.w) + sizeof(area.d) + sizeof(nucleiCount) + (nucleiCount * sizeof(long)) + namesize;
		
	char *image = globalObject->allocClearedMemory(size);
	char *ptr = (char*)image;

	memcpy(ptr, &location.x, sizeof(location.x)); 	ptr += sizeof(location.x);
	memcpy(ptr, &location.y, sizeof(location.y)); 	ptr += sizeof(location.y);
	memcpy(ptr, &location.z, sizeof(location.z)); 	ptr += sizeof(location.z);
	memcpy(ptr, &area.h, sizeof(area.h)); 	ptr += sizeof(area.h);
	memcpy(ptr, &area.w, sizeof(area.w)); 	ptr += sizeof(area.w);
	memcpy(ptr, &area.d, sizeof(area.d)); 	ptr += sizeof(area.d);
	memcpy(ptr,&nucleiCount,sizeof(nucleiCount)); 	ptr+=sizeof(nucleiCount);

	for(size_t i=0;i<nucleiCount;i++)
	{
		long k = nuclei[i];
		memcpy(ptr,&k,sizeof(k));
		ptr+=sizeof(k);
	}
	strncpy(ptr,name.c_str(),namesize);
	ptr += (namesize+1);

	Tuple* tuple = new Tuple();
	tuple->objectPtr = image;
	tuple->value = size;

	return tuple;
}

void Region::add(Nucleus *nucleus)
{
	nuclei.push_back(nucleus->id);
	setDirty(true);
}
