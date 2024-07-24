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

#include "Column.h"
#include "TR1Random.h"
#include "Global.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

Column::Column(bool createLayers, unsigned long parentId):
	NNComponent(ComponentTypeColumn)
{
	if(createLayers)
	{
		size_t layerCount = 6;												// 6 layers
		for(size_t i=0;i<layerCount;i++) 
		{
			Layer *layer1 = Layer::create(parentId);
			layers.push_back(layer1->id);
		}
	}
}

Column::Column(bool createLayers, size_t layerCount, unsigned long parentId):
	NNComponent(ComponentTypeColumn)
{
	if(createLayers)
	{
		for(size_t i=0;i<layerCount;i++) 
		{
			Layer *layer1 = Layer::create(parentId);
			layers.push_back(layer1->id);
		}
	}
}

Column::~Column(void)
{
}

/*
	std::vector<long> layers;

	Location3D location;
	Size3D area;
*/

void Column::toJSON(std::ofstream& outstream)
{
	std::stringstream ss;
	LOGSTREAM(ss) << "Exporting column..." << std::endl;
	globalObject->log(ss);

	std::string sep("");
	outstream << "            { \"_type\": \"Column\", \"id\": " << id << ", \"location\": [" << location.x << ", " << location.y << ", " << location.z << "], \"size\": [" << area.h << ", " << area.w << ", " << area.d << "], \"layers\": [ " << std::endl;
	for (unsigned int i = 0; i < layers.size(); i++)
	{
		outstream << sep;
		sep = ",";
		Layer* l = globalObject->layerDB.getComponent(layers[i]);
		l->toJSON(outstream);
	}
	outstream << "           ] } " << std::endl;

}



void Column::save(void)
{
	globalObject->columnDB.save(this);
}

void Column::commit(void)
{
	globalObject->columnDB.addToCache(this);
}

/*
Column *Column::create(SpatialDetails details, unsigned long parentId)
{
	Column *c = new Column(true,parentId);
	c->id = globalObject->nextComponent(ComponentTypeColumn);

	c->location = details.location;
	c->area = details.area;


	globalObject->insert(c);
	// connect all layers
	Layer *layer1 = globalObject->layerDB.getComponent(c->layers[0]); 
	Layer *layer2 = globalObject->layerDB.getComponent(c->layers[1]);
	Layer *layer3 = globalObject->layerDB.getComponent(c->layers[2]);
	Layer *layer4 = globalObject->layerDB.getComponent(c->layers[3]);
	Layer *layer5 = globalObject->layerDB.getComponent(c->layers[4]);
	Layer *layer6 = globalObject->layerDB.getComponent(c->layers[5]);

	layer1->projectTo(layer2); //  connect layer 1 (input) to layer 2 and 3
	layer1->projectTo(layer3);

	layer2->projectTo(layer5);
	layer3->projectTo(layer5);

	layer4->projectTo(layer2); //  connect layer 4 to layer 2,3 and 5
	layer4->projectTo(layer3);
	layer4->projectTo(layer5);

	layer5->projectTo(layer5); //  connect layer 4 to layer 2 and 3
	layer5->projectTo(layer6);

	// layer 6 is output layer


	return c;
}
*/

Column *Column::create(SpatialDetails details, size_t layerCount, unsigned long parentId)
{
	Column *c = new Column(true,layerCount, parentId);
	c->id = globalObject->nextComponent(ComponentTypeColumn);

	c->location = details.location;
	c->area = details.area;


	globalObject->insert(c);

	switch(layerCount)
	{
		case 1:
		{
			c->inputLayer = 1;
			c->outputLayer = 1;
			break;
		}
		case 2:
		{
			Layer *layer1 = globalObject->layerDB.getComponent(c->layers[0]); 
			Layer *layer2 = globalObject->layerDB.getComponent(c->layers[1]);

			layer1->projectTo(layer2); //  connect layer 1 (input) to layer 2 and 3
			layer2->projectTo(layer1);

			c->inputLayer = 1;
			c->outputLayer = 2;

	
			break;
		}
		case 3:
		{
			Layer *layer1 = globalObject->layerDB.getComponent(c->layers[0]); 
			Layer *layer2 = globalObject->layerDB.getComponent(c->layers[1]);
			Layer *layer3 = globalObject->layerDB.getComponent(c->layers[2]);

			layer1->projectTo(layer2); //  connect layer 1 (input) to layer 2 and 3
			layer2->projectTo(layer1);
			layer2->projectTo(layer3);
			layer1->projectTo(layer3); 
			layer3->projectTo(layer2);

			c->inputLayer = 1;
			c->outputLayer = 3;
			break;
		}
		case 4:
		{
			Layer *layer1 = globalObject->layerDB.getComponent(c->layers[0]); 
			Layer *layer2 = globalObject->layerDB.getComponent(c->layers[1]);
			Layer *layer3 = globalObject->layerDB.getComponent(c->layers[2]);
			Layer *layer4 = globalObject->layerDB.getComponent(c->layers[3]);

			layer1->projectTo(layer2); //  connect layer 1 (input) to layer 2 and 3
			layer1->projectTo(layer3);

			layer3->projectTo(layer4);

			layer4->projectTo(layer2); //  connect layer 4 to layer 2,3 and 5
			layer4->projectTo(layer3);
			layer4->projectTo(layer1);

			c->inputLayer = 1;
			c->outputLayer = 4;
			break;
		}
		case 5:
		{
			Layer *layer1 = globalObject->layerDB.getComponent(c->layers[0]); 
			Layer *layer2 = globalObject->layerDB.getComponent(c->layers[1]);
			Layer *layer3 = globalObject->layerDB.getComponent(c->layers[2]);
			Layer *layer4 = globalObject->layerDB.getComponent(c->layers[3]);
			Layer *layer5 = globalObject->layerDB.getComponent(c->layers[4]);

			layer1->projectTo(layer2); //  connect layer 1 (input) to layer 2 and 3
			layer1->projectTo(layer3);

			layer2->projectTo(layer5);
			layer3->projectTo(layer5);

			layer4->projectTo(layer2); //  connect layer 4 to layer 2,3 and 5
			layer4->projectTo(layer3);
			layer4->projectTo(layer5);

			layer5->projectTo(layer1); //  connect layer 4 to layer 2 and 3
			layer5->projectTo(layer2);
			layer5->projectTo(layer3);
			layer5->projectTo(layer4);

			c->inputLayer = 1;
			c->outputLayer = 5;
			break;
		}
		case 6:
		{
			Layer *layer1 = globalObject->layerDB.getComponent(c->layers[0]); 
			Layer *layer2 = globalObject->layerDB.getComponent(c->layers[1]);
			Layer *layer3 = globalObject->layerDB.getComponent(c->layers[2]);
			Layer *layer4 = globalObject->layerDB.getComponent(c->layers[3]);
			Layer *layer5 = globalObject->layerDB.getComponent(c->layers[4]);
			Layer *layer6 = globalObject->layerDB.getComponent(c->layers[5]);

			layer1->projectTo(layer2); //  connect layer 1 (input) to layer 2 and 3
			layer1->projectTo(layer3);

			layer2->projectTo(layer5);
			layer3->projectTo(layer5);

			layer4->projectTo(layer2); //  connect layer 4 to layer 2,3 and 5
			layer4->projectTo(layer3);
			layer4->projectTo(layer5);

			layer5->projectTo(layer5); //  connect layer 4 to layer 2 and 3
			layer5->projectTo(layer6);

			c->inputLayer = 1;
			c->outputLayer = 6;
			break;
		}
	}


	return c;
}


/*
void Column::connectTo(Column *column)
{
	// Connect source layer1 (output) to targer layer4 (input)
	Layer *layerA = globalObject->layerDB.getComponent(layers[0]);
	Layer *layerB = globalObject->layerDB.getComponent(column->layers[3]);
	layerA->connectTo(layerB);
}
*/

void Column::projectTo(Column *targetColumn, float sparsity)
{
	std::stringstream ss;
	// Connect source layer1 (output) to target layer4 (input)

	Column *sourceColumn = this;

	Layer *sourceLayer[6];
	Layer *targetLayer[6];
	for(int i=0;i<6;i++)
	{
		sourceLayer[i] = NULL;
		targetLayer[i] = NULL;
	}


	int sourceLayerCount = layers.size();

	for(int i=0;i<sourceLayerCount;i++)
	{
		sourceLayer[i] = globalObject->layerDB.getComponent(sourceColumn->layers[i]); // layer 1;
	}

	int targetLayerCount = targetColumn->layers.size();

	for(int i=0;i<targetLayerCount;i++)
	{
		targetLayer[i] = globalObject->layerDB.getComponent(targetColumn->layers[i]); // layer 1;
	}

/*
	for(int i=0;i<sourceLayerCount;i++)
	{
		for(int j=0;j<targetLayerCount;j++)
		{
			LOGSTREAM(ss) << "  Projecting source layer " << i+1 << " to target layer " << j+1 << std::endl;
			globalObject->log(ss);
			sourceLayer[i]->projectTo(targetLayer[j],sparsity);

		}
	}
*/

	int input = targetColumn->inputLayer - 1;
	Layer *tLayer = targetLayer[input];

	int output = this->outputLayer -1;
	Layer *sLayer = sourceLayer[output];

	if(input!=output) 
	{
		LOGSTREAM(ss) << "      Projecting neurons in source layer " << input << " to target layer " << output << std::endl;
		globalObject->log(ss);
		sLayer->projectTo(tLayer,sparsity,EXCITATORY); // excitatory

		LOGSTREAM(ss) << "      Projecting neurons target layer " << output << " to source layer " << input << std::endl;
		globalObject->log(ss);
		sLayer->projectTo(tLayer,sparsity,EXCITATORY); 
	}
	else
	{
		LOGSTREAM(ss) << "      Source layer same as target layer, so no projections " << std::endl;
	}
}

void Column::cycle(void)
{
	size_t layerCount = layers.size();												
	for(size_t i=0;i<layerCount;i++) 
	{
		Layer *layer = globalObject->layerDB.getComponent(layers[i]);
		layer->cycle();
	}
//	std::cout << "Column " << this->id << " cycled." << std::endl;
}

void Column::initializeRandom(unsigned long parentId)
{
	(void)parentId;	
}

Column *Column::instantiate(long key, size_t len, void *data)
{
	(void)len;
	Column *column = new Column(false,0);
	column->id = key;

	char *ptr = (char*)data;

	long layerCount = 0;
	memcpy(&layerCount,ptr,sizeof(layerCount)); 	ptr+=sizeof(layerCount);

	for(size_t i=0;i<(size_t)layerCount;i++)
	{
		long lid = 0;
		memcpy(&lid,ptr,sizeof(lid));
		column->layers.push_back(lid);
		ptr+=sizeof(lid);
	}
//	printf("instantiate: Column %i layersize %i\n",(int)column->id,(int)column->layers.size());
	column->inputLayer =1;
	column->outputLayer = column->layers.size();
	return column;
}

Tuple *Column::getImage(void)
{


	long layerCount = layers.size();
	size_t size = sizeof(layerCount) + (layerCount * sizeof(layerCount));

	char* image = globalObject->allocClearedMemory(size);
	char* ptr = (char*)image;

	memcpy(ptr,&layerCount,sizeof(layerCount)); ptr+=sizeof(layerCount);

	for(size_t i=0;i<(size_t)layerCount;i++)
	{
		long k = layers[i];
		memcpy(ptr,&k,sizeof(k));
		ptr+=sizeof(k);
	}

	Tuple* tuple = new Tuple();
	tuple->objectPtr = image;
	tuple->value = size;

	return tuple;
}

