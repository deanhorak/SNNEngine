#include "Global.h"
#include "TR1Random.h"
#include "BrainDemo6.h"

BrainDemo6::BrainDemo6(void)
{
}

BrainDemo6::~BrainDemo6(void)
{
}

Brain * BrainDemo6::create(bool rebuild)
{

	Brain *brain;

	std::stringstream ss;

	if (!rebuild) // if not rebuilding, just return brain
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
	if (brain->restartpoint())
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

	Nucleus *nucleusAnteroventral = 0L;
	if (brain->restartpoint())
	{
		nucleusAnteroventral = Nucleus::create("nucleusAnteroventral", sd, SENSORY_NUCLEUS); // Sensory neurons
		regionThalamus->add(nucleusAnteroventral);
		nucleusAnteroventral->addColumns(10, profile); // 10 columns, each with 6 layers, each with 5 clusters, each with 10 neurons
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusAnteroventral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionThalamus->name << " complete with " << regionThalamus->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	Region *regionBasalGanglia = 0L;
	if (brain->restartpoint())
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
	if (brain->restartpoint())
	{
		nucleusStriatum = Nucleus::create("nucleusStriatum", sd, INTER_NUCLEUS);
		regionBasalGanglia->add(nucleusStriatum);
		nucleusStriatum->addColumns(10, profile);
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

		size_t pct = (columnNum * 100) / columnCount;
		//		std::cout << " Initializing layers within column " << column->id << std::endl;
		LOGSTREAM(ss) << " Initializing layers within column " << column->id << " - (" << columnNum++ << " of " << columnCount << " - " << pct << "%) " << std::endl;
		globalObject->log(ss);

		if (brain->restartpoint())
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
	if (brain->restartpoint())
	{
		regionThalamus->projectTo(regionBasalGanglia, 10.f);
	}
	brain->syncpoint();

	// all other regions project to thamamus
	LOGSTREAM(ss) << "regionBasalGanglia->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if (brain->restartpoint())
	{
		regionBasalGanglia->projectTo(regionThalamus, 10.f);
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
		if (globalObject->axonDB.getValue()->getSynapses()->size() == 0)
			zeroCount++;
	}
	LOGSTREAM(ss) << zeroCount << " axons have no synapses." << std::endl;
	globalObject->log(ss);

	globalObject->flush();
	return brain;
}

void BrainDemo6::step(Brain *brain)
{
	//	std::cout << "Current timestamp " << globalObject->current_timestep << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;

/*
	std::stringstream ss;

	if (globalObject->actionPotentialsSize()>0) {
		LOGSTREAM(ss) << "Current timestamp " << globalObject->current_timestep << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;
		globalObject->log(ss);
	}
*/

}
