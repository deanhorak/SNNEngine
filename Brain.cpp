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

#include <filesystem>
#include "Global.h"

#include "Brain.h"

#include "TR1Random.h"

//#include <boost/json.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include "nlohmann/json.hpp"

extern bool keepRunning;

extern Global *globalObject;
extern int main_process(Brain *brain);
extern int neuron_process(Brain *brain);

#define STALL_OVERHEAD 100

using json = nlohmann::json;
namespace fs = std::filesystem;

///////////////////////////
class OrderPreservingJSONSerializer {
public:
    static std::string serialize(const json& j) {
        std::string result = "{";
        bool first = true;
        for (auto it = j.begin(); it != j.end(); ++it) {
            if (!first) {
                result += ", ";
            }
            first = false;
            result += "\"" + it.key() + "\":";
            result += serializeValue(it.value());
        }
        result += "}";
        return result;
    }

private:
    static std::string serializeValue(const json& value) {
        if (value.is_string()) {
            return "\"" + value.get<std::string>() + "\"";
        } else if (value.is_number()) {
            return std::to_string(value.get<double>());
        } else if (value.is_boolean()) {
            return value.get<bool>() ? "true" : "false";
        } else if (value.is_null()) {
            return "null";
        } else if (value.is_object()) {
            return serialize(value);
        } else if (value.is_array()) {
            std::string result = "[";
            bool first = true;
            for (const auto& item : value) {
                if (!first) {
                    result += ", ";
                }
                first = false;
                result += serializeValue(item);
            }
            result += "]";
            return result;
        } else {
            return "unknown";
        }
    }
};

///////////////////////////

Brain::Brain(void):
	NNComponent(ComponentTypeBrain)
{
	current_syncpoint = 0;
	timeAdjust = 0;
	globalObject->startRealTime = boost::posix_time::second_clock::local_time();
}

Brain::~Brain(void)
{
}

// Excite 'num' random neurons into firing
void Brain::excite(int num)
{

	std::stringstream ss;
	LOGSTREAM(ss) << "Excite " << num << " neurons." << std::endl;
	globalObject->log(ss);

	int totalNeurons = globalObject->neuronsSize();
	int base = globalObject->componentBase[ComponentTypeNeuron];


	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	for(int i=0;i< num;i++) {
    	// Generate a random value in the range [0, y)
    	int randomOffset = std::rand() % totalNeurons;
		// Calculate the result in the desired range [x, x + y)
    	int index = base + randomOffset;
		Neuron* neuron = globalObject->neuronDB.getComponent(index);
		neuron->fire();
	}

	LOGSTREAM(ss) << "Excitaton complete." << std::endl;
	globalObject->log(ss);
}


void Brain::validateAndFormatJSON(void) {
    try {
        // Read the JSON file
		std::string inputFilename = std::string(DB_PATH) +  BRAINDEMONAME + std::string("/") + std::string("temp.json").c_str();
		std::string outputFilename = std::string(DB_PATH) +  BRAINDEMONAME + std::string("/") + std::string("serialized.json").c_str();


        std::ifstream inputFile(inputFilename);
        json inputJSON;
        inputFile >> inputJSON;

        // Write the formatted JSON to the output file
        std::ofstream outputFile(outputFilename);
        outputFile << std::setw(4) << inputJSON << std::endl;
		inputFile.close();
		outputFile.close();
		fs::remove(inputFilename);

        std::cout << "JSON file validated and formatted successfully!" << std::endl;
    } catch (const json::parse_error& e) {
        std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


void Brain::exportJSON(void)
{
	std::stringstream ss;
	LOGSTREAM(ss) << "Beginning export to JSON..." << std::endl;
	globalObject->log(ss);


	std::ofstream ofs(std::string(DB_PATH) +  BRAINDEMONAME + std::string("/") + std::string("temp.json").c_str());
	toJSON(ofs);
	ofs.close();

	validateAndFormatJSON();

	LOGSTREAM(ss) << "Export to JSON complete..." << std::endl;
	globalObject->log(ss);


}

void Brain::toJSON(std::ofstream& outstream)
{
	outstream << "{ \"_type\": \"Brain\", \"id\": " << id << ", \"regions\": [ " << std::endl;
	std::string sep("");

	for (unsigned int i = 0; i < regions.size(); i++)
	{
		outstream << sep;
		sep = ",";
		Region* r = globalObject->regionDB.getComponent(regions[i]);
		r->toJSON(outstream);
	}
	outstream << "] } " << std::endl;

}

void Brain::save(void)
{
	globalObject->brainDB.save(this);
}


Brain *Brain::create(bool setToDirty)
{
	tr1random = new TR1Random();
	globalObject = new Global();
	Brain *brain = new Brain();
	brain->id = globalObject->nextComponent(ComponentTypeBrain);
	globalObject->insert(brain);
	if(setToDirty)
		brain->setDirty();
	brain->timeAdjust = 0;
	return brain;
}

Brain* Brain::loadFromJSON(void)
{
	tr1random = new TR1Random();
	globalObject = new Global();
	globalObject->brainDB.begin();
	// begin loading JSON
	std::string jsonfilename(std::string(DB_PATH) +  BRAINDEMONAME + std::string("/") + "serialized.json");

	boost::property_tree::ptree pt;
	boost::property_tree::read_json(jsonfilename, pt);
	std::string test("");

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt) {
		boost::property_tree::basic_ptree<std::string, std::string, std::less<std::string>> x = v.second;

	}


	Brain* brain = globalObject->brainDB.getValue();
	globalObject->syncpoint = globalObject->readSyncpoint();

	return brain;
}

Brain *Brain::load(void)
{
	tr1random = new TR1Random();
	globalObject = new Global();
	//globalObject->brainDB.begin(); 
	long brainId = globalObject->componentBase[ComponentTypeBrain];
	Brain *brain = globalObject->brainDB.getComponent(brainId);
	globalObject->syncpoint = globalObject->readSyncpoint();

	for(size_t i=0;i<CTYPE_COUNT;i++) 
	{
		long thisCount = 0;
		switch (i) {
//	1 = ComponentDB<Brain> brainDB;
			case 1:
				thisCount = (long)globalObject->brainDB.size();
				break;
//	2 = ComponentDB<Region> regionDB;
			case 2:
				thisCount = (long)globalObject->regionDB.size();
				break;
//	3 = ComponentDB<Nucleus> nucleusDB;
			case 3:
				thisCount = (long)globalObject->nucleusDB.size();
				break;
//	4 = ComponentDB<Column> columnDB;
			case 4:
				thisCount = (long)globalObject->columnDB.size();
				break;
//	5 = ComponentDB<Layer> layerDB;
			case 5:
				thisCount = (long)globalObject->layerDB.size();
				break;
//	6 = ComponentDB<Cluster> clusterDB;
			case 6:
				thisCount = (long)globalObject->clusterDB.size();
				break;
//	7 = ComponentDB<Neuron> neuronDB;
			case 7:
				thisCount = (long)globalObject->neuronDB.size();
				break;
//	8 = ComponentDB<Axon> axonDB;
			case 8:
				thisCount = (long)globalObject->axonDB.size();
				break;
//	9 = ComponentDB<Dendrite> dendriteDB;
			case 9:
				thisCount = (long)globalObject->dendriteDB.size();
				break;
//	10 = ComponentDB<Synapse> synapseDB;
			case 10:
				thisCount = (long)globalObject->synapseDB.size();
				break;
			default:
				break;
		}
		if(thisCount>0) 
		{
			globalObject->componentCounter[i] = globalObject->componentBase[i] + thisCount;
		}
	}

	return brain;
}

/*
void Brain::initializeRandom(void)
{
	std::string dummyStr;
	size_t rnd = (size_t) tr1random->generate(1,3); // Random # of regions
	SpatialDetails sd(1, 2, 3, 4, 5, 6); // Dummy test locations/size
	for(size_t i=0;i<rnd;i++)
	{
		Region *r = Region::create(dummyStr,sd);
		r->initializeRandom();
		regions.push_back(r->id);
	}

}
*/

void Brain::adjustSynapses(void)
{
}

void Brain::removePreviousTimedEvents(void)
{

//	long totalEvents = globalObject->getTotalEvents();
//	printf("removePreviousTimedEvents: Total events %ld\r", totalEvents);

	size_t intervalOffsetValue = globalObject->current_timestep % MAX_TIMEINTERVAL_BUFFER_SIZE;

//	std::cout << "removePreviousTimedEvents Locking teVector_mutex[" << intervalOffsetValue << "]" << std::endl;
	boost::mutex::scoped_lock amx(*(globalObject->teVector_mutex[intervalOffsetValue]));

	std::vector<TimedEvent *> *teVector = &globalObject->timeIntervalEvents[intervalOffsetValue];
	std::vector<TimedEvent *> newVector;
	std::vector<TimedEvent *> oldVector;
	for(size_t i = 0;i<teVector->size();i++)
	{
			TimedEvent *te = (*teVector)[i];
			if(te->slice < globalObject->current_timestep - 4000L) { // If aged out (over 4s ago), don't keep it. 
				oldVector.push_back(te); // Save this event
			} 
			else 
			{
				newVector.push_back(te); // Save this event
			}
	}
	teVector->clear(); // Clear out the vector
	// add only new events
	for(size_t i = 0; i< newVector.size();i++)
	{
		TimedEvent *te = newVector[i];
		teVector->push_back(te);
	}

	// remove old events
	size_t numberOfEvents = oldVector.size();
	if(numberOfEvents>0) {
//		std::cout << "\nRemoving " << numberOfEvents << " TimedEvents " << std::endl;
		for(size_t i = 0; i< numberOfEvents;i++)
		{
			TimedEvent *te = oldVector[i];
			delete te;
		}
	}
//	std::cout << "removePreviousTimedEvents UnLocking teVector_mutex[" << intervalOffsetValue << "]" << std::endl;

}

void Brain::step(void)
{

	globalObject->cycle();

	removePreviousTimedEvents();

	globalObject->increment();

//	startRealTime = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::ptime thisTime = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration duration = thisTime - globalObject->startRealTime;

	long milliseconds = duration.total_milliseconds(); // + timeAdjust;
	long waittime = (globalObject->current_timestep - milliseconds);  

	if(globalObject->current_timestep > milliseconds) 
	{
		//printf("wait %ld\n", waittime);
		if (waittime > STALL_OVERHEAD) {
/*
			if(globalObject->logEvents) 
			{	
				std::stringstream ss;
				ss << "Brain_step: waittime=" << waittime;
				globalObject->writeEventLog(ss.str().c_str());
			}
*/
			boost::this_thread::sleep(boost::posix_time::milliseconds(waittime));
/*
			if(globalObject->logEvents) 
			{	
				std::stringstream ss;
				ss << "Brain_step: waiting complete";
				globalObject->writeEventLog(ss.str().c_str());
			}
*/
		}
	}

//	removeDeadAPs(); // remove dead APs and unfire firing axons
	
}

/*
void Brain::removeDeadAPs(void)
{
	globalObject->removeDeadAPs();
}
*/

Brain *Brain::instantiate(long key, size_t len, void *data)
{
// 	size_t size = sizeof(float)+sizeof(float)+sizeof(long)+sizeof(long);
	(void)len;
	Brain *brain = new Brain();
	brain->id = key;

	char *ptr = (char*)data;

	long regionCount = 0;
	memcpy(&regionCount,ptr,sizeof(regionCount)); ptr+=sizeof(regionCount);
	for(size_t i=0;i<(size_t)regionCount;i++)
	{
		long rid = 0;
		memcpy(&rid,ptr,sizeof(rid));
		brain->regions.push_back(rid);
		ptr+=sizeof(rid);
	}
	return brain;
}

Tuple *Brain::getImage(void)
{
/* -- persisted values
	size_t regionCount;
	std::vector<long> regions;
*/
	long regionCount = (u_int32_t)regions.size();

	size_t size = sizeof(regionCount)+(regionCount * sizeof(long));

	char *image = globalObject->allocClearedMemory(size);
	char *ptr = (char*)image;


	memcpy(ptr,&regionCount,sizeof(regionCount)); 	ptr+=sizeof(regionCount);

	for(size_t i=0;i<(size_t)regionCount;i++)
	{
		long k = regions[i];
		memcpy(ptr,&k,sizeof(k));
		ptr+=sizeof(k);
	}

	Tuple* tuple = new Tuple();
	tuple->objectPtr = image;
	tuple->value = size;


	return tuple;
}

void Brain::shutdown(void)
{
	globalObject->shutdown();
	delete tr1random;
	delete globalObject;
}

void Brain::add(Region *reg)
{
	long value = reg->id;
	regions.push_back(value);
}


void Brain::syncpoint(void) 
{ 
	current_syncpoint++;
	if(globalObject->syncpoint<current_syncpoint)
	{
		globalObject->flush();
		globalObject->writeSyncpoint(current_syncpoint);
	}
}

bool Brain::restartpoint(void) 
{ 

	if(!keepRunning)
	{
		std::stringstream ss;
		LOGSTREAM(ss) << "Shutdown detected at syncpoint " << current_syncpoint << "..." << std::endl;
		globalObject->log(ss);

		shutdown();
		exit(0);
	}

	bool restart = true;
	if(globalObject->syncpoint>=0)
	{
		if(globalObject->syncpoint>current_syncpoint)
		{
			restart = false;
		}

	}
	if(!restart)
	{
		std::stringstream ss;
		LOGSTREAM(ss) << "Skipping restart syncpoint  " << current_syncpoint << "..." << std::endl;
		globalObject->log(ss);
	}
	return restart; 
}

void Brain::report(void) 
{ 

	std::string rept = getReport();

	globalObject->log((char *)rept.c_str());
}

void Brain::longReport(void)
{

	std::string rept = getLongReport();

	globalObject->log((char*)rept.c_str());
}

std::string Brain::getReport(void)
{ 

	std::stringstream ss;
	std::string returnString;

	size_t layerCacheSize = globalObject->layerDB.cacheSize();
	size_t clusterCacheSize = globalObject->clusterDB.cacheSize();
	size_t columnCacheSize = globalObject->columnDB.cacheSize();
	size_t nucleusCacheSize = globalObject->nucleusDB.cacheSize();
	size_t regionCacheSize = globalObject->regionDB.cacheSize();
	size_t neuronCacheSize = globalObject->neuronDB.cacheSize();
	size_t axonCacheSize = globalObject->axonDB.cacheSize();
	size_t dendriteCacheSize = globalObject->dendriteDB.cacheSize();
	size_t synapseCacheSize = globalObject->synapseDB.cacheSize();

	size_t layerSize = globalObject->layerDB.size();
	size_t clusterSize = globalObject->clusterDB.size();
	size_t columnSize = globalObject->columnDB.size();
	size_t nucleusSize = globalObject->nucleusDB.size();
	size_t regionSize = globalObject->regionDB.size();
	size_t neuronSize = globalObject->neuronDB.size();
	size_t axonSize = globalObject->axonDB.size();
	size_t dendriteSize = globalObject->dendriteDB.size();
	size_t synapseSize = globalObject->synapseDB.size();

	size_t layerSaves = globalObject->layerDB.saves();
	size_t clusterSaves = globalObject->clusterDB.saves();
	size_t columnSaves = globalObject->columnDB.saves();
	size_t nucleusSaves = globalObject->nucleusDB.saves();
	size_t regionSaves = globalObject->regionDB.saves();
	size_t neuronSaves = globalObject->neuronDB.saves();
	size_t axonSaves = globalObject->axonDB.saves();
	size_t dendriteSaves = globalObject->dendriteDB.saves();
	size_t synapseSaves = globalObject->synapseDB.saves();

	size_t layerReimages = globalObject->layerDB.reimages();
	size_t clusterReimages = globalObject->clusterDB.reimages();
	size_t columnReimages = globalObject->columnDB.reimages();
	size_t nucleusReimages = globalObject->nucleusDB.reimages();
	size_t regionReimages = globalObject->regionDB.reimages();
	size_t neuronReimages = globalObject->neuronDB.reimages();
	size_t axonReimages = globalObject->axonDB.reimages();
	size_t dendriteReimages = globalObject->dendriteDB.reimages();
	size_t synapseReimages = globalObject->synapseDB.reimages();

	LOGSTREAM(ss) << "Structure: size/cachesize/saves/re-images/cache misses" << std::endl;
	returnString += ss.str();
	LOGSTREAM(ss) << "Region: " << regionSize << "/" << regionCacheSize << "/" << regionSaves << "/" << regionReimages << "/" << globalObject-> regionDB.getCacheMissCount()  << std::endl;
	returnString += ss.str();
	LOGSTREAM(ss) << "Nucleus: " << nucleusSize << "/" << nucleusCacheSize  << "/" << nucleusSaves << "/" << nucleusReimages << "/" << globalObject->regionDB.getCacheMissCount() << std::endl;
	returnString += ss.str();
	LOGSTREAM(ss) << "Column: " << columnSize << "/" << columnCacheSize  << "/" << columnSaves << "/" << columnReimages << "/" << globalObject->regionDB.getCacheMissCount() << std::endl;
	returnString += ss.str();
	LOGSTREAM(ss) << "Layer: " << layerSize << "/" << layerCacheSize  << "/" << layerSaves << "/" << layerReimages << "/" << globalObject->regionDB.getCacheMissCount() << std::endl;
	returnString += ss.str();
	LOGSTREAM(ss) << "Cluster: " << clusterSize << "/" << clusterCacheSize  << "/" << clusterSaves << "/" << clusterReimages << "/" << globalObject->regionDB.getCacheMissCount() << std::endl;
	returnString += ss.str();
	LOGSTREAM(ss) << "Neuron: " << neuronSize << "/" << neuronCacheSize  << "/" << neuronSaves << "/" << neuronReimages << "/" << globalObject->regionDB.getCacheMissCount() << std::endl;
	returnString += ss.str();
	LOGSTREAM(ss) << "Axon: " << axonSize << "/" << axonCacheSize  << "/" << axonSaves << "/" << axonReimages << "/" << globalObject->regionDB.getCacheMissCount() << std::endl;
	returnString += ss.str();
	LOGSTREAM(ss) << "Dendrite: " << dendriteSize << "/" << dendriteCacheSize  << "/" << dendriteSaves << "/" << dendriteReimages << "/" << globalObject->regionDB.getCacheMissCount() << std::endl;
	returnString += ss.str();
	LOGSTREAM(ss) << "Synapse: " << synapseSize << "/" << synapseCacheSize  << "/" << synapseSaves << "/" << synapseReimages << "/" << globalObject->regionDB.getCacheMissCount() << std::endl;
	returnString += ss.str();

	LOGSTREAM(ss) << "---------" << std::endl;
	returnString += ss.str();

//	LOGSTREAM(ss) << "APs: " << globalObject->actionPotentialsSize() << std::endl;
//	returnString += ss.str();

	long startTS = globalObject->current_timestep;
	long endTS = startTS +  MAX_TIMEINTERVAL_BUFFER_SIZE;
	std::string sep = "";
	bool found = false;
	long totalAPs = 0;
	long totalOnes = 0;

	for (long x = startTS; x < endTS; x++)
	{
		size_t slot = x % MAX_TIMEINTERVAL_BUFFER_SIZE;

		std::cout << "getReport Locking teVector_mutex[" << slot << "]" << std::endl;
		boost::mutex::scoped_lock amx(*(globalObject->teVector_mutex[slot])); // Dangerous/potential deadlock candidate

		std::vector<TimedEvent *> *teVector = &globalObject->timeIntervalEvents[slot];
		long sz = teVector->size();
		if (sz > 0)
		{

			if (sz == 1)
			{ // just increment the counters; don't display the "1"s because there are so many
				found = true;
				totalAPs++;
				totalOnes++;
			}
			else
			{
				long offset = x - startTS;
				LOGSTREAM(ss) << sep << "AP Offset: " << offset << ":" << sz;
				returnString += ss.str();
				sep = ",";
				found = true;
				totalAPs += sz;
			}
		}
		std::cout << "getReport UnLocking teVector_mutex[" << slot << "]" << std::endl;
	}

	if (!found)
	{
		LOGSTREAM(ss) << "No active APs " << std::endl;
		returnString += ss.str();
	}
	else
	{
		LOGSTREAM(ss) << std::endl;
		returnString += ss.str();
		LOGSTREAM(ss)  <<  "Total APs: " << totalAPs <<  ", " << totalOnes <<  " of which are single AP events." << std::endl;
		returnString += ss.str();
	}

	LOGSTREAM(ss) << "---------" << std::endl;
	returnString += ss.str();

	return returnString;
}

std::string Brain::getLongReport(void)
{

	std::stringstream ss;
	std::string returnString;

	returnString = getReport();
	return returnString;
}

void Brain::startServer(void) 
{ 
	networkServer.start();

	std::thread t1(main_process, this);
    t1.detach(); // Wait for the new thread to finish execution
}

void Brain::stopServer(void)
{
	networkServer.stop();
}

void Brain::startNeuronProcessing(void) 
{ 
	neuronProcessor.start();

}

void Brain::stopNeuronProcessing(void)
{
	neuronProcessor.stop();
}