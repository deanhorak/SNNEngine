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

#include "Global.h"
#include "TR1Random.h"
#include "BrainDemo5.h"

BrainDemo5::BrainDemo5(void)
{
}

BrainDemo5::~BrainDemo5(void)
{
}

Brain * BrainDemo5::create(bool rebuild) 
{

	Brain *brain; 

	std::stringstream ss;

	if(!rebuild) // if not rebuilding, just return brain
	{
		LOGSTREAM(ss) << " Loading brain" << std::endl;
		globalObject->log(ss);
		brain = Brain::load();
	} 
	else 
	{
		LOGSTREAM(ss) << " Creating brain" << std::endl;
		globalObject->log(ss);
		brain = Brain::create();
	}

	SpatialDetails sd(1, 2, 3, 4, 5, 6); // Dummy test locations/size

	// Create Thalamus
	LOGSTREAM(ss) << "Create Micro Thalamus ... " << std::endl;
	globalObject->log(ss);

	Region *regionThalamus = 0L;
	if(brain->restartpoint())
	{
		regionThalamus = Region::create("regionThalamus", sd);
		brain->add(regionThalamus);
	} 
	else 
	{
		globalObject->regionDB.next();
		regionThalamus = globalObject->regionDB.getValue();
		LOGSTREAM(ss) << "regionThalamus globalObject->regionDB.getValue() " << std::endl;
		globalObject->log(ss);
	}
	brain->syncpoint();


	// Create Thalamic Nuclei
	ColumnNeuronProfile profile; // default profile for all layers is Pyramidal neurons, 100 neurons per cluster, with 10 clusters

	Nucleus *regionDigits = 0L;
	if(brain->restartpoint())
	{
		regionDigits = Nucleus::create("regionDigits", sd);
		regionThalamus->add(regionDigits);
		regionDigits->addColumns(10,6,5,10); // 10 columns, each with 6 layers, each with 5 clusters, each with 10 neurons
	} 
	else 
	{
		globalObject->nucleusDB.next();
		regionDigits = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionThalamus->name << " complete with " << regionThalamus->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	Region *regionBasalGanglia = 0L;
	if(brain->restartpoint())
	{
		regionBasalGanglia = Region::create("regionBasalGanglia", sd);
		brain->add(regionBasalGanglia);
	}
	else
	{
		globalObject->regionDB.next();
		regionBasalGanglia = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusStriatum = 0L;
	if(brain->restartpoint())
	{
		nucleusStriatum = Nucleus::create("nucleusStriatum", sd);
		regionBasalGanglia->add(nucleusStriatum);
		nucleusStriatum->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusStriatum = globalObject->nucleusDB.getValue();
	} 
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionBasalGanglia->name << " complete with " << regionBasalGanglia->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);



	// Wrap up construction

	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << globalObject->regionsSize() << " regions created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->nucleiSize() << " nuclei created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->columnsSize() << " columns created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->layersSize() << " layers created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->clustersSize() << " clusters created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->neuronsSize() << " neurons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->axonsSize() << " axons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << " Attaching layers within columns " << std::endl;
	globalObject->log(ss);
	// Now, attach layers within a column
//	CollectionIterator<Column *> itColumn(Global::getColumnsCollection());

	size_t columnCount = globalObject->columnsSize();
	size_t columnNum = 0;

	for (globalObject->columnDB.begin(); globalObject->columnDB.more(); globalObject->columnDB.next())
	{
		Column *column = globalObject->columnDB.getValue();

		size_t pct = (columnNum*100) / columnCount;
//		std::cout << " Initializing layers within column " << column->id << std::endl;
		LOGSTREAM(ss) << " Initializing layers within column " << column->id << " - (" << columnNum++ << " of " << columnCount << " - " << pct << "%) " << std::endl;
		globalObject->log(ss);

		if(brain->restartpoint())
		{
		//	column->initializeLayers(0);
		}
		brain->syncpoint();
	}

	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << globalObject->regionsSize() << " regions created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->nucleiSize() << " nuclei created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->columnsSize() << " columns created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->layersSize() << " layers created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->clustersSize() << " clusters created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->neuronsSize() << " neurons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->axonsSize() << " axons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << " Attach regions" << std::endl;
	globalObject->log(ss);
// Finally, attach the Regions


	// Thamamus project to all other regions
	LOGSTREAM(ss) << "regionThalamus->projectTo(regionBasalGanglia)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionBasalGanglia,10.f);
	}
	brain->syncpoint();

	// all other regions project to thamamus
	LOGSTREAM(ss) << "regionBasalGanglia->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionBasalGanglia->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "------------------------------------------------------" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->dendritesSize() << " dendrites created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->synapsesSize() << " synapses created" << std::endl;
	globalObject->log(ss);

	unsigned long zeroCount = 0;
//	CollectionIterator<Axon *> itAxon(Global::getAxonsCollection());
//	for (itAxon.begin(); itAxon.more(); itAxon.next())
	for (globalObject->axonDB.begin(); globalObject->axonDB.more(); globalObject->axonDB.next())
	{
		if(globalObject->axonDB.getValue()->getSynapses()->size()==0) 
			zeroCount++;
	}
	LOGSTREAM(ss) << zeroCount << " axons have no synapses." << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << "SNNEngine ready to accept input..." << std::endl;
	globalObject->log(ss);

	globalObject->flush();
	return brain;
}

void BrainDemo5::step(Brain *brain)
{
		(void)brain;

//	std::cout << "Current timestamp " << globalObject->current_timestep << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;


	std::stringstream ss;

}
