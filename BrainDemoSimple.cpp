#include "Global.h"
#include "TR1Random.h"
#include "BrainDemoSimple.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

BrainDemoSimple::BrainDemoSimple(void)
{
}

BrainDemoSimple::~BrainDemoSimple(void)
{
}

Brain* BrainDemoSimple::create(bool rebuild)
{

	Brain* brain;

	std::stringstream ss;

	if (!rebuild) // if not rebuilding, just return brain
	{
		LOGSTREAM(ss) << " Loading brain" << std::endl;
		globalObject->log(ss);
		brain = Brain::load();
		LOGSTREAM(ss) << globalObject->brainSize() << " brains loaded" << std::endl;
		globalObject->log(ss);
}
	else
	{
		LOGSTREAM(ss) << " Creating brain" << std::endl;
		globalObject->log(ss);
		brain = Brain::create();
	}
	// SpatialDetails(float x, float y, float z, float h, float w, float d)
	SpatialDetails sdThalamus(0, 0, 2500, 5000, 5000, 5000); // Dummy test locations/size
	SpatialDetails sdAnteroventral(2500, 2500, 2500, 5000, 5000, 5000); // Dummy test locations/size
	SpatialDetails sdBasalGanglia(5000, 5000, 5000, 5000, 5000, 5000); // Dummy test locations/size
	SpatialDetails sdStriatum(7500, 7500, 7500, 5000, 5000, 5000); // Dummy test locations/size

	// Create Thalamus
	LOGSTREAM(ss) << "Create Micro Thalamus ... " << std::endl;
	globalObject->log(ss);

	Region* regionThalamus = 0L;
	if (brain->restartpoint())
	{
		regionThalamus = Region::create("regionThalamus", sdThalamus);
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
	ColumnNeuronProfile profile;
	profile.neuronMorphology.push_back(new NeuronMorphology(Pyramidal, 2, 2)); // set profile for Pyramidal neurons, 2 neurons per cluster, with 2 clusters
	
	Nucleus* nucleusAnteroventral = 0L;
	if (brain->restartpoint())
	{
		nucleusAnteroventral = Nucleus::create("nucleusAnteroventral", sdAnteroventral, SENSORY_NUCLEUS);
		regionThalamus->add(nucleusAnteroventral);
		nucleusAnteroventral->addColumns(2, profile); // 10 columns, each with 6 layers, each with 5 clusters, each with 10 neurons
		insertSynapses(nucleusAnteroventral);
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusAnteroventral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionThalamus->name << " complete with " << regionThalamus->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	///////////////////////////////////
/*
	Region* regionBasalGanglia = 0L;
	if (brain->restartpoint())
	{
		regionBasalGanglia = Region::create("regionBasalGanglia", sdBasalGanglia);
		brain->add(regionBasalGanglia);
	}
	else
	{
		globalObject->regionDB.next();
		regionBasalGanglia = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus* nucleusStriatum = 0L;
	if (brain->restartpoint())
	{
		nucleusStriatum = Nucleus::create("nucleusStriatum", sdStriatum);
		regionBasalGanglia->add(nucleusStriatum);
		nucleusStriatum->addColumns(2, profile);
		insertSynapses(nucleusStriatum);
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusStriatum = globalObject->nucleusDB.getValue();
	}
*/	
	brain->syncpoint();


	/////////////////////////////

	// Wrap up construction

	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << formatNumber(globalObject->regionsSize()) << " regions created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->nucleiSize()) << " nuclei created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->columnsSize()) << " columns created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->layersSize()) << " layers created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->clustersSize()) << " clusters created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->neuronsSize()) << " neurons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->axonsSize()) << " axons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->dendritesSize()) << " dendrites created" << std::endl;
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
		Column* column = globalObject->columnDB.getValue();

		size_t pct = (columnNum * 100) / columnCount;
		//		std::cout << " Initializing layers within column " << column->id << std::endl;
		LOGSTREAM(ss) << " Initializing layers within column " << column->id << " - (" << ++columnNum << " of " << columnCount << " - " << pct << "%) " << std::endl;
		globalObject->log(ss);

		if (brain->restartpoint())
		{
		//	column->initializeLayers(0);
		}
		brain->syncpoint();
	}

	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << " Attach regions" << std::endl;
	globalObject->log(ss);
	// Finally, attach the Regions

/*
		// Thamamus project to all other regions
	LOGSTREAM(ss) << "regionThalamus->projectTo(regionBasalGanglia)" << std::endl;
	globalObject->log(ss);
	if (brain->restartpoint())
	{
		regionThalamus->projectTo(regionBasalGanglia, 95.f);
	}
	brain->syncpoint();

	// all other regions project to thamamus
	LOGSTREAM(ss) << "regionBasalGanglia->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if (brain->restartpoint())
	{
		regionBasalGanglia->projectTo(regionThalamus, 95.f);
	}
	brain->syncpoint();
*/
	LOGSTREAM(ss) << "------------------------------------------------------" << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << formatNumber(globalObject->regionsSize()) << " regions created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->nucleiSize()) << " nuclei created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->columnsSize()) << " columns created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->layersSize()) << " layers created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->clustersSize()) << " clusters created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->neuronsSize()) << " neurons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->axonsSize()) << " axons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->dendritesSize()) << " dendrites created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << formatNumber(globalObject->synapsesSize()) << " synapses created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << "------------------------------------------------------" << std::endl;
	globalObject->log(ss);

	// Make final Adustments
	finalAdjustments(ss);

	globalObject->flushAll();

	if(!rebuild) 
		globalObject->loadAll();

	LOGSTREAM(ss) << "SNNEngine ready to accept input..." << std::endl;
	globalObject->log(ss);

	return brain;
}

void BrainDemoSimple::finalAdjustments(std::stringstream &ss)
{
	LOGSTREAM(ss) << "Making final adjustments..." << std::endl;
	globalObject->log(ss);

	unsigned long zeroCount = 0;
	for (globalObject->axonDB.begin(); globalObject->axonDB.more(); globalObject->axonDB.next())
	{
		Axon* ax = globalObject->axonDB.getValue();
		if (ax->getSynapses()->size() == 0)
		{
			zeroCount++;
		}
	}

	LOGSTREAM(ss) << formatNumber(zeroCount) << " axons have no synapses before fixup." << std::endl;
	globalObject->log(ss);

	unsigned long added = 0;
	if (zeroCount > 0) {

		float pct = 0;
		float index = 1;
		//std::vector<unsigned long, unsigned long> newSynapses;
		//	CollectionIterator<Axon *> itAxon(Global::getAxonsCollection());
		//	for (itAxon.begin(); itAxon.more(); itAxon.next())
		size_t axonCount = globalObject->axonDB.size();

		for (globalObject->axonDB.begin(); globalObject->axonDB.more(); globalObject->axonDB.next())
		{
			Axon* ax = globalObject->axonDB.getValue();
			if (ax->getSynapses()->size() == 0)
			{
				// An axon with zero synapses found. Create a synapse for this guy to somewhere... anywhere
	//////////////////////////
				long neuronId = ax->neuronId;
				Neuron* neu = globalObject->neuronDB.getComponent(neuronId);				// we have the neuron in question
				long parentClusterId = neu->parentId;
				Cluster* cl = globalObject->clusterDB.getComponent(parentClusterId);			// we have the cluster that this neuron belongs to
				size_t cSize = cl->getNeurons().size();
				for (size_t i = 0; i < cSize; i++)										// Loop through each neuron in this cluster
				{
					long nId = cl->getNeurons()[i];
					if (nId > 0)
					{
						if (nId != neuronId)										// Ignore if we're looking at our own neuron
						{	// Valud neuronId and not equal to current one
							Neuron* neuX = globalObject->neuronDB.getComponent(nId);			// Get the neuron we want to attach to....
							if (neuX != NULL)												// Make sure it's valid
							{
								added++;
								Dendrite* den = Dendrite::create(neuX, neu);				// Create a new dendrite
                                neuX->dendriteMap.insert(std::make_pair(neu->id, den->id));
								Synapse* s = Synapse::create(den);					// Add a synapse to the dendrite
								long sid = s->id;
								ax->insertSynapse(sid);								// Attatch this new synapse to our axon
							}
							else
							{
								LOGSTREAM(ss) << "Neuron id " << formatNumber(nId) << " in cluster  " << formatNumber(parentClusterId) << " not found" << std::endl;
								globalObject->log(ss);
							}
						}
					}
					else
					{
						LOGSTREAM(ss) << "Zero neuronId found in cluster  " << formatNumber(parentClusterId) << " not found" << std::endl;
						globalObject->log(ss);
					}
				}
				//////////////////////////
			}
			pct = index / axonCount;
			printf("Pct Complete %f\r", pct * 100.0f);

			index++;
		}
	}

	// Wrap up...

	LOGSTREAM(ss) << formatNumber(added) << " synapses added." << std::endl;
	globalObject->log(ss);


	zeroCount = 0;
	for (globalObject->axonDB.begin(); globalObject->axonDB.more(); globalObject->axonDB.next())
	{
		Axon* ax = globalObject->axonDB.getValue();
		if (ax->getSynapses()->size() == 0)
		{
			zeroCount++;
		}
	}

	LOGSTREAM(ss) << formatNumber(zeroCount) << " axons have no synapses after fixup." << std::endl;
	globalObject->log(ss);


	LOGSTREAM(ss) << "Final adjustments complete..." << std::endl;
	globalObject->log(ss);

}

void BrainDemoSimple::step(Brain* brain)
{
	//	std::cout << "Current timestamp " << globalObject->current_timestep << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;


	std::stringstream ss;
	/****
		if(globalObject->actionPotentialsSize()>0) {
			LOGSTREAM(ss) << "Current timestamp " << globalObject->current_timestep << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;
			globalObject->log(ss);
		}
	****/

}

std::string BrainDemoSimple::formatNumber(unsigned long long number) {
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << number;
	return ss.str();

}

// This kludge is to sprinkle synapses among the clusters within nucleus, and within the neurons within clusters
void BrainDemoSimple::insertSynapses(Nucleus* nuc) {
	for (long i = 0; i < nuc->columns.size(); i++) {
		long colId = nuc->columns[i];
		Column* col = globalObject->columnDB.getComponent(colId);
		for (long j = 0; j < col->layers.size(); j++) {
			long layerId = col->layers[j];
			Layer *lay = globalObject->layerDB.getComponent(layerId);
			for (long k = 0; k < lay->clusters.size(); k++) {
				long clusterId = lay->clusters[k];
				Cluster* clu = globalObject->clusterDB.getComponent(clusterId);
				for (long l = 0; l < clu->getNeurons().size(); l++) {
					long neuronId = clu->getNeurons()[l];
					Neuron* neu = globalObject->neuronDB.getComponent(neuronId);
					std::vector<long> *axons = neu->getAxons();
					for (long m = 0; m < axons->size(); m++) {
						long axonId = (*axons)[m];
						Axon* axon = globalObject->axonDB.getComponent(axonId);
						// We know have the axon which we will be attaching synapses to
						// This is a place holder for a more sophisticated algorythm to assign synapses. Perhaps future models with 3D information loaded in (SpatialDetails)
						// can place synapses where axons and dendrites come in close proximity
						//
						// But for now, let's just do random
						//
						// Iterate over neurons in this cluster and associate X% of them, selected at random, with the dendrites of other neurons
						// 
						int percent = 95; // start with 98%
						//
						boost::random::mt19937 gen;

						for (long ll = 0; ll < clu->getNeurons().size(); ll++) {
							long neuronId2 = clu->getNeurons()[ll];
							if (neuronId2 != neu->id) { // If not ourself
								Neuron* neu2 = globalObject->neuronDB.getComponent(neuronId2);
								boost::random::uniform_int_distribution<> dist(0, 100);
								int value = dist(gen);
								if (value <= percent) 
								{
									if(!neu->isConnectedTo(neu2)) {
										neu->connectTo(neu2);
									}
								}
							}

						}

					}

				}
			}
		}
	}
}

// This kludge is to sprinkle synapses between nucleus's A and B
void BrainDemoSimple::insertSynapses(Nucleus* nucA, Nucleus* nucB) {
	// null for now
}

Brain* BrainDemoSimple::createFromJSON(void) 
{
	Brain* brain;

	std::stringstream ss;

	LOGSTREAM(ss) << " Loading brain from JSON" << std::endl;
	globalObject->log(ss);
	brain = Brain::loadFromJSON();
	return brain;
}

