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


void Nucleus::projectTo(Nucleus *nucleus, float sparsity)
{
	size_t cSize1 = columns.size();
	size_t cSize2 = nucleus->columns.size();
	size_t cMax = cSize1 * cSize2;
	size_t cNumber = 0;
	std::stringstream ss;
	for(size_t i=0;i<cSize1;i++)
	{
		Column *col1 = globalObject->columnDB.getComponent(columns[i]);
		for(size_t j=0;j<cSize2;j++)
		{
			Column *col2 = globalObject->columnDB.getComponent(nucleus->columns[j]);
//			size_t pct = (cNumber*100) / cMax;
//			LOGSTREAM(ss) << "      Projecting Column " << col1->id << " to " << col2->id << "... (" << cNumber++ << " of " << cMax << " - " << pct << "%) " << std::endl;
//			globalObject->log(ss);

			col1->projectTo(col2,sparsity);
		}
	}
}

void Nucleus::cycle(void)
{
	size_t cSize = columns.size();
	for(size_t i=0;i<cSize;i++)
	{
		Column *c = globalObject->columnDB.getComponent(columns[i]);
		c->cycle();
	}
/*
	CollectionIterator<Column *> itColumn(&columns);
	for (itColumn.begin(); itColumn.more(); itColumn.next())
	{
		itColumn.value()->cycle();
	}
*/
//	std::cout << "Nucleus " << this->id << " cycled." << std::endl;
}


void Nucleus::initializeRandom(void)
{
	
	size_t rnd = (size_t) tr1random->generate(1,10); // Random # of Columns
	for(size_t i=0;i<rnd;i++) 
	{
		SpatialDetails sd(this->location, this->area);
		sd.randomizeLocation();

		Column *c = Column::create(sd,this->id);
		c->initializeRandom(this->id);
		columns.push_back(c->id);
	}
	
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

	for(size_t i=0;i<columnCount;i++)
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

	for(size_t i=0;i<columnCount;i++)
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

void Nucleus::addColumns(size_t colCount, size_t clusterCount, size_t neuronCount)
{
	for(size_t i=0;i<colCount;i++) 
	{
		SpatialDetails sd(this->location, this->area);
		sd.randomizeLocation();

		Column *c = Column::create(sd,this->id);
		size_t lSize = c->layers.size();
		for(size_t k1=0;k1<lSize;k1++)
		{
			Layer *layer = globalObject->layerDB.getComponent(c->layers[k1]);
			for(size_t i5=0;i5<clusterCount;i5++) 
			{
				SpatialDetails sdc(sd);
				sdc.randomizeLocation();

				Cluster *cl = Cluster::create(sdc,this->id);
				cl->parentId = this->id;
				for(size_t i6=0;i6<neuronCount;i6++) 
				{
					Neuron *neur = Neuron::create(sdc, Pyramidal,cl->id, this->nucleusType);
					cl->getNeurons().push_back(neur->id);
					cl->setDirty();
				}
				layer->clusters.push_back(cl->id);
				layer->setDirty();
			}
		}
		columns.push_back(c->id);
	}
	setDirty(true);
}

void Nucleus::addColumns(size_t colCount, size_t layerCount, size_t clusterCount, size_t neuronCount)
{
	for(size_t i=0;i<colCount;i++) 
	{
		SpatialDetails sd(this->location, this->area);
		sd.randomizeLocation();

		Column *c = Column::create(sd,layerCount, this->id);
		size_t lSize = c->layers.size();
		for(size_t k1=0;k1<lSize;k1++)
		{
			Layer *layer = globalObject->layerDB.getComponent(c->layers[k1]);
			for(size_t i5=0;i5<clusterCount;i5++) 
			{
				SpatialDetails sdc(sd);
				sdc.randomizeLocation();

				Cluster *cl = Cluster::create(sdc,this->id);
				cl->parentId = this->id;
				for(size_t i6=0;i6<neuronCount;i6++) 
				{
					Neuron *neur = Neuron::create(sdc, Pyramidal,cl->id, this->nucleusType);
					cl->getNeurons().push_back(neur->id);
					cl->setDirty();
				}
				layer->clusters.push_back(cl->id);
				layer->setDirty();
			}
		}
		columns.push_back(c->id);
	}
	setDirty(true);
}

void Nucleus::addColumns(size_t colCount, ColumnNeuronProfile &cProfile)
{
	for(size_t i=0;i<colCount;i++) 
	{
		SpatialDetails sd(this->location, this->area);
		sd.randomizeLocation();

		Column *c = Column::create(sd,this->id);
		size_t lSize = c->layers.size();
		for(size_t k1=0;k1<lSize;k1++)
		{
			Layer *layer = globalObject->layerDB.getComponent(c->layers[k1]);
			NeuronMorphology *neuronMorphology = cProfile.neuronMorphology[k1];
			for(size_t i5=0;i5<neuronMorphology->clusterCount;i5++) 
			{
				SpatialDetails sdc(sd);
				sdc.randomizeLocation();

				Cluster *cl = Cluster::create(sdc,this->id);
				for(size_t i6=0;i6<neuronMorphology->clusterSize;i6++) 
				{
					SpatialDetails sdn(sdc);
					sdn.randomizeLocation();

					Neuron *neur = Neuron::create(sdn,neuronMorphology->type, cl->id,this->nucleusType);
					cl->getNeurons().push_back(neur->id);
					cl->setDirty();
				}
				layer->clusters.push_back(cl->id);
				layer->setDirty();
			}
		}
		columns.push_back(c->id);
	}
	setDirty(true);
}

